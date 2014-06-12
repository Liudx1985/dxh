/*
 *	Liudx create this file for json-rpc serialize frame
 *  2013.6.5  version 0.0.2. 	
 	*remove MACROS :JSON_SERIALIZE_OBJ & JSON_SERIALIZE_ARRAY, 
 	 use JSON_SERIALIZE_TYPE to handle with all object type & basical & sequence type

 *  2013.6.14 version 0.0.3.
 	*rewrite the basical type system
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/foreach.hpp>
#include <boost/bind/apply.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost::property_tree;
using boost::true_type;
using boost::false_type;
using boost::enable_if; //  NOTICE for C++11, remove these using declarations.
using namespace std;

#ifdef _NO_JSON_SERIALIZE // hide the declarations.
#	define BEGIN_JSON_SERIALIZE()
#	define 	JSON_SERIALIZE_TYPE(key, v)
#	define END_JSON_SERIALIZE()
#endif

// test if C is a vector<T> or list <T>
template <typename T, typename C>
struct is_sequence_imp
{
	enum {value = false};
};

template <typename T>
struct is_sequence_imp<T, typename std::vector<T> >
{
	enum {value = true};
};


template <typename T>
struct is_sequence_imp<T, typename std::list<T> >
{
	enum {value = true};
};

// template <typename T>
// struct is_sequence_imp<T, typename std::queue<T> >
// {
// 	enum {value = true};
// };

template <typename C, class Enable = void>
struct is_sequence
{
	typedef boost::false_type value_type;
};


template <typename C>
struct is_sequence<C,
	typename enable_if<is_sequence_imp<typename C::value_type, C>/*::value*/ >::type>
	/*C++1x standard declare the enable_if<bool, T> clearly, but boost declares
	enable_if_c<Cond.value, T>::value as enable_if , so comment it now*/
{
	typedef true_type value_type;
};

template <typename T>
struct is_json_type{
	typedef boost::false_type value_type;
};

#define DECLARE_JSON_TYPE(T)\
	template<>struct is_json_type<T>{typedef boost::true_type value_type;};

DECLARE_JSON_TYPE(char)
DECLARE_JSON_TYPE(unsigned char)
DECLARE_JSON_TYPE(short)
DECLARE_JSON_TYPE(unsigned short)
DECLARE_JSON_TYPE(int)
DECLARE_JSON_TYPE(unsigned int)
DECLARE_JSON_TYPE(long)
DECLARE_JSON_TYPE(unsigned long)
DECLARE_JSON_TYPE(float)
DECLARE_JSON_TYPE(double)

DECLARE_JSON_TYPE(std::string)



//Declares
// implement basic template:
template <typename T, bool bFundamental>
inline bool SerializeTypeImpl(ptree &pt, std::string const &key, T &v, bool bSave
						  , const boost::integral_constant<bool, bFundamental> &);

// Common type
template <typename T>
inline bool SerializeTypeImpl(ptree &pt, std::string const &key, T &v, bool bSave
							  , const boost::true_type &)
{
	if (bSave)
	{
		pt.put(key, boost::lexical_cast<std::string>(v));
	}
	else
	{
		v = pt.get<T>(key);
	}
	return true;
}

// Common Object 
template <typename T>
inline bool SerializeTypeImpl(ptree &pt, std::string const &key, T &v, bool bSave
							  , const boost::false_type &)
{
	// oBj
	if (bSave)
	{
		ptree child;
		v.Serialize(child, true);
		pt.add_child(key, child);
	}
	else
	{
		ptree child = pt.get_child(key);
		v.Serialize(child, false);
	}
	return true;	
}

// Default serialize for list or vector<T>.
template <typename T, bool bFundamental>
inline bool SerializeArrayImpl(ptree &pt, std::string const &key, T &v, bool bSave,
							   const boost::integral_constant<bool, bFundamental> &);

// simple type like int,float,char, std::stirng etc.
template <typename T>
inline bool SerializeArrayImpl(ptree &pt, std::string const &key, T &ary, bool bSave,
					  const boost::true_type &)
{
	if (bSave)
	{
		boost::property_tree::ptree array;
		BOOST_FOREACH(T::value_type &v, ary)
		{
			array.push_back(std::make_pair("", boost::lexical_cast<std::string>(v)));
		}
		pt.add_child(key, array);
	}
	else
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &sub, pt.get_child(key))
		{
			assert(sub.first.empty()); // array elements have no names
			ary.push_back(boost::lexical_cast<T::value_type>(sub.second.data()));
		}
	}
	return true;
}

// simple struct array. {std::vector<int> _xxx;}
template <typename T>
inline bool SerializeArrayImpl(ptree &pt, std::string const &key, T &ary, bool bSave,
					  const boost::false_type &)
{
	if (bSave)
	{
		boost::property_tree::ptree array;
		BOOST_FOREACH(T::value_type &v, ary)
		{
			ptree child;
			v.Serialize(child, true);
			array.push_back(std::make_pair("", child));
		}
		pt.add_child(key, array);
	}
	else
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &sub, pt.get_child(key))
		{
			assert(sub.first.empty());
			T::value_type child;
			child.Serialize(sub.second, false);
			ary.push_back(child);
		}
	}
	return true;
}

template <typename T>
inline bool SerializeImpl(ptree &pt, std::string const &key, T &v, bool bSave
						  , const false_type &)
{
	return SerializeTypeImpl(pt, key, v, bSave
		, is_json_type<T>::value_type());
}

template <typename T>
inline bool SerializeImpl(ptree &pt, std::string const &key, T &v, bool bSave
						  , const true_type &)
{
	return SerializeArrayImpl(pt, key, v, bSave
		, is_json_type<T::value_type>::value_type());
}

template <typename T>
inline 
void Serialize(ptree &pt, std::string const &key, T &v, bool bSave){
	typedef typename is_sequence<T>::value_type vt; // GCC can recognize only this form.
	SerializeImpl(pt, key, v, bSave, vt());
}

#define _STR(str) std::string(#str)

#define BEGIN_JSON_SERIALIZE()\
	bool Serialize(ptree &pt, bool bSave) {\
	try {\

// key name, value.
#define JSON_SERIALIZE_TYPE(key, v)\
	::Serialize(pt, _STR(key), v, bSave);

#define END_JSON_SERIALIZE()\
	}\
	catch (std::exception &e){\
		std::cerr << e.what() << std::endl;\
		return false;\
	}\
	return true;\
}\

// defined in 
// JSON-RPC: http://www.jsonrpc.org/specification
struct json_rpc_request_header
{
	std::string _version;
	std::string _method;
	// params: A Structured value that holds the parameter values to be used during the invocation of the method. This member MAY be omitted.
	int _id;
	json_rpc_request_header() : _version("2.0")
	{
	}

	BEGIN_JSON_SERIALIZE()\
		JSON_SERIALIZE_TYPE(jsonrpc, _version)\
		JSON_SERIALIZE_TYPE(method, _method)\
		JSON_SERIALIZE_TYPE(id, _id)\
	END_JSON_SERIALIZE()
};


// response
struct response_error
{
	// 	-32700	Parse error	Invalid JSON was received by the server.
	// 			An error occurred on the server while parsing the JSON text.
	// 	-32600	Invalid Request	The JSON sent is not a valid Request object.
	// 	-32601	Method not found	The method does not exist / is not available.
	// 	-32602	Invalid params	Invalid method parameter(s).
	// 	-32603	Internal error	Internal JSON-RPC error.
	// 	-32000 to -32099	Server error	Reserved for implementation-defined server-errors.
	int _code;
	std::string _message;
	std::string _data;	// A Primitive or Structured value that contains additional information about the error.
	// This may be omitted.
	// The value of this member is defined by the Server (e.g. detailed error information, nested errors etc.).
	response_error(int code, std::string message):_code(code), _message(message)
	{
	}
	BEGIN_JSON_SERIALIZE()
		JSON_SERIALIZE_TYPE(code, _code)
		JSON_SERIALIZE_TYPE(message, _message)
		//JSON_SERIALIZE_TYPE(data, _data)
	END_JSON_SERIALIZE()
};

enum
{
	err_Parse,
	err_invalid_req,
	err_method_notfound,
	err_invalid_pram,
	err_internal,
	//...
	err_server_default,
};

struct _error_prompt
{
	int _code;
	const char *_msg;
}gs_jsonrpc_error[] =
{
	{-32700, ("Parse error")},			//	Invalid JSON was received by the server.		An error occurred on the server while parsing the JSON text.
	{-32600, ("Invalid Request")},		//	The JSON sent is not a valid Request object.
	{-32601, ("Method not found")},		//  The method does not exist / is not available.
	{-32602, ("Invalid params")},		//	Invalid method parameter(s).
	{-32603, ("Internal error")},		// Internal JSON-RPC error.
	//...
	{-32099, ("Server error")},
};

#define BEGIN_JSONRPC_RES_SERIALIZE()\
	BEGIN_JSON_SERIALIZE()\
	JSON_SERIALIZE_TYPE(jsonrpc, _version)\
	if (_error.get()){\
		response_error &err = *_error;\
		JSON_SERIALIZE_TYPE(error, err)\
	}else\
{\

// between BEGIN_JSONRPC_RES_SERIALIZE END_JSONRPC_RES_SERIALIZE, u can write the serialize for [result]

#define END_JSONRPC_RES_SERIALIZE()\
	JSON_SERIALIZE_TYPE(id, _id)\
	}\
	END_JSON_SERIALIZE()\

//////////////////////////////////////////////////////////////////////////
struct json_rpc_response_header
{
	std::string _version;
	// _result : The value of this member is determined by the method invoked on the Server.
	std::string _id; // int & 'null'.
	std::auto_ptr<response_error> _error; // "error": {"code": -32601, "message": "Method not found"}
	json_rpc_response_header() : _version("2.0")
	{
	}
	BEGIN_JSONRPC_RES_SERIALIZE(pt)
	END_JSONRPC_RES_SERIALIZE()
};

//////////////////////////////////////////////////////////////////////////
struct complex_real
{
	int _real;
	int _img;
	std::string _name;
	complex_real(int r = 0, int i = 0): _real(r), _img(i){}
	BEGIN_JSON_SERIALIZE()
		JSON_SERIALIZE_TYPE(real, _real)
		JSON_SERIALIZE_TYPE(image, _img)
		JSON_SERIALIZE_TYPE(name, _name)
	END_JSON_SERIALIZE()
};

struct add_request
{
	std::vector<complex_real> _param;
	BEGIN_JSON_SERIALIZE()
		JSON_SERIALIZE_TYPE(param, _param)
	END_JSON_SERIALIZE()
};

struct add_response: public json_rpc_response_header
{
	complex_real _result;
	add_response():_result(0, 0){}
	BEGIN_JSONRPC_RES_SERIALIZE(pt)
		JSON_SERIALIZE_TYPE(result, _result)
	END_JSONRPC_RES_SERIALIZE()
};

//////////////////////////////////////////////////////////////////////////
struct sum_request
{
	std::vector<std::string> _param;
	BEGIN_JSON_SERIALIZE()
		JSON_SERIALIZE_TYPE(param, _param);
	END_JSON_SERIALIZE()
};

struct sum_result
{	
	sum_result(sum_request &req)
	{
		_sum = 0;
		_average = 0;
		BOOST_FOREACH(std::string &s, req._param)
		{
			_sum += boost::lexical_cast<int>(s);
		}
		_average = (float)_sum / (req._param.size());
	}
	int _sum;
	float _average;
	BEGIN_JSON_SERIALIZE()
		JSON_SERIALIZE_TYPE(sum, _sum)
		JSON_SERIALIZE_TYPE(average, _average)
	END_JSON_SERIALIZE()
};

struct sum_response: public json_rpc_response_header
{
	sum_result _result;
	sum_response(sum_request &req) : _result(req)
	{
	}
	BEGIN_JSONRPC_RES_SERIALIZE(pt)
		JSON_SERIALIZE_TYPE(result, _result)
	END_JSONRPC_RES_SERIALIZE()
};

void format_error(std::string id, int iErrorIdx, ptree &pt)
{
	json_rpc_response_header res;
	res._id = id;
	res._error.reset(new response_error(gs_jsonrpc_error[iErrorIdx]._code
		, gs_jsonrpc_error[iErrorIdx]._msg));
	res.Serialize(pt, true);
}

// dispatch, get the result
std::string handle_json_rpc_request(ptree &pt)
{	
	ptree pt_ret;
	json_rpc_request_header header;
	header.Serialize(pt, false);
	if (header._method == "add")
	{
		add_request req;
		if (req.Serialize(pt, false))
		{
			// handle
			add_response res;
			BOOST_FOREACH(complex_real &c, req._param)
			{
				res._result._real += c._real;
				res._result._img += c._img;
			}
			res._id = boost::lexical_cast<std::string>(header._id);
			res.Serialize(pt_ret, true);
		}
	}
	else if (header._method == "sum")
	{
		sum_request req;
		if (req.Serialize(pt, false))
		{
			if (!req._param.empty())
			{
				sum_response res(req);
				res._id = boost::lexical_cast<std::string>(header._id);
				res.Serialize(pt_ret, true);
			}
			else	// failed: prarm not valid.
			{
				format_error(boost::lexical_cast<std::string>(header._id)
					, err_invalid_pram, pt_ret);
			}
		}
	}
	else // failed : method not found 
	{
		format_error(boost::lexical_cast<std::string>(header._id)
			, err_method_notfound, pt_ret);
	}
	std::stringstream output;
	write_json(output, pt_ret);
	return output.str();
}

std::string handle_json_rpc_batch_request(ptree &pt)
{
	std::string output = "[\n";
	// fetch batch request root
	BOOST_FOREACH(boost::property_tree::ptree::value_type &sub, pt.get_child(" "))
	{
		assert(sub.first.empty());
		output += handle_json_rpc_request(sub.second);
		output += " ,\n";
	}
	output += "]\n";
	return output;
}

// if json-rpc request body start with '{', just a single call
#ifdef _TEST_CODE
void test_basic()
{
	//
	cout << "test sum\n";

	// request sum
	ptree pt_sum;
	Serialize(pt_sum, _STR(jsonrpc), _STR(2.0), true);
	Serialize(pt_sum, _STR(method), _STR(sum), true);
	Serialize(pt_sum, _STR(id), _STR(2), true);
	std::vector<int> vecParam;
	vecParam.push_back(1);
	vecParam.push_back(2);
	vecParam.push_back(3);
	vecParam.push_back(4);
	vecParam.push_back(5);
	vecParam.push_back(6);
	vecParam.push_back(7);
	vecParam.push_back(8);
	vecParam.push_back(9);
	vecParam.push_back(10);
	Serialize(pt_sum, _STR(param), vecParam, true);
	write_json(cout, pt_sum);

	cout << handle_json_rpc_request(pt_sum);

	// request add:
	cout << "---------end of Sum------\n";

	ptree pt_add;
	Serialize(pt_add, _STR(jsonrpc), _STR(2.0), true);
	Serialize(pt_add, _STR(method), _STR(add), true);
	Serialize(pt_add, _STR(id), _STR(2), true);
	//Serialize(pt_add, std::string("param.first"), "4", true);
	std::vector<complex_real> param;
	param.push_back(complex_real(1, 2));
	param.push_back(complex_real(1, -1));
	param.push_back(complex_real(3, 3));
	param.push_back(complex_real(4, -2));

	Serialize(pt_add, _STR(param), param, true);
	write_json(cout, pt_add);

	cout << handle_json_rpc_request(pt_add);
}

/* if start with '[' , should be a batch call. like:[{..1},{..2},{..3},..], but
  ptree only recognize array in object	
	so just pre-translate the json-rpc batch request string
	[
		{1},
		{2}
	]
	--> append \{" ":\ at begin, \}\at end
	{
		"":
		[
			{1},
			{2}
		]
	}
	then get the batch params.
	*/
const std::string gs_batchreq_prefix = "{\" \":";
const std::string gs_batchreq_postfix = "}";

void test_batch()
{
	ptree pt_sum_batch;
	ptree pt_sum1;
	Serialize(pt_sum1, _STR(jsonrpc), _STR(2.0), true);
	Serialize(pt_sum1, _STR(method), _STR(sum), true);
	Serialize(pt_sum1, _STR(id), _STR(1), true);
	std::vector<int> vecParam;
	vecParam.push_back(1);
	vecParam.push_back(2);
	vecParam.push_back(3);
	vecParam.push_back(4);
	vecParam.push_back(5);
	Serialize(pt_sum1, _STR(param), vecParam, true);

	ptree pt_sum2;
	Serialize(pt_sum2, _STR(jsonrpc), _STR(2.0), true);
	Serialize(pt_sum2, _STR(method), _STR(sum), true);
	Serialize(pt_sum2, _STR(id), _STR(201), true);
	vecParam.clear();
	vecParam.push_back(10);
	vecParam.push_back(20);
	vecParam.push_back(30);
	vecParam.push_back(40);
	vecParam.push_back(50);
	Serialize(pt_sum2, _STR(param), vecParam, true);
	boost::property_tree::ptree array;
	array.push_back(std::make_pair("", pt_sum1));
	array.push_back(std::make_pair("", pt_sum2));

	pt_sum_batch.add_child(" ", array);
	
	write_json(cout, pt_sum_batch);
	cout << "\nresponse of batch:\n" << handle_json_rpc_batch_request(pt_sum_batch) << endl;
}

int main(int argc, char *argv[])
{
	test_batch();	
	return 0;
}

#endif // TEST CODE
