
#ifndef _BIND_MEM_FUN_H_
#define _BIND_MEM_FUN_H_

///////////////////////////////////////////////////////////////////////////////
// bind_mem_fun.h
// -*- c++ -*-
// Author : Liudx
// Email: Liudx1985@gmail.com
// Date: 2011-7-11
// -----------------------------------------------------------------------

/*
This is a modern reflect & delegate method

!! STRONGLY Recommend You use the boost.function or boost::singal2 instead this Classes !!
Check the boost::bind & boost::mem_fn for more information about binding a member funtion.

	$$ DEMO  $$

#include <iostream>
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/bind/apply.hpp>
#include <boost/lambda/lambda.hpp>

class A
{
public:
	int fun(int a){
		std::cout << "calling A::fun(" << a << ")\n";
	}
};

class B
{
public:
	boost::function<int(int)> delegate_; // slot
	void bind(A& a)
	{
		delegate_ = boost::bind(&A::fun, &a, _1); // signal
	}

	int fun(int a){return delegate_(a);}
};

int main()
{
	A a;
	B b;
	b.bind(a);
	b.fun(3);// Call a.fun(3)  indirectly.
}
*/


#if defined(_MSC_VER) && (_MSC_VER > 1000)
#	pragma once
#endif

#include <functional>

template <typename T>
class bind_mem_fun_t{};

/**
\ R C::F()
\ you may define more argument yourself.
*/
template <typename ReturnType, typename ClassType>
struct bind_mem_fun_t<ReturnType (ClassType::*)()>
{
	typedef ReturnType (ClassType::*MemFnc)();
	MemFnc fn_;
	ClassType *a_;
public:
	bind_mem_fun_t(ClassType *a, MemFnc f):a_(a), fn_(f)
	{
	}

	bind_mem_fun_t(bind_mem_fun_t const & rhs)
	{
		if (this != &rhs)
		{
			*this = rhs;
		}
	}

	bind_mem_fun_t& operator = (bind_mem_fun_t const & rhs)
	{
		if (this != &rhs)
		{
			fn_ = rhs.fn_;
			a_ = rhs.a_;
		}
		return *this;
	}
	/*
	For compatiable with the std: the call may look like _ret_(a_)*/
	typename std::mem_fun_t<ClassType, ReturnType> get_mem_fn()
	{
		return std::mem_fun_t<ClassType, ReturnType>(fn_);
	}

	ReturnType operator ()()
	{
		return (a_->*fn_)();
	}
};

/**
\ R C::F(Arg1)
\ you may define more argument yourself.
*/

template <typename ReturnType, typename ClassType, typename ArgType>
struct bind_mem_fun_t<ReturnType (ClassType::*)(ArgType)>
{
	typedef ReturnType (ClassType::*MemFnc)(ArgType);
	MemFnc fn_;
	ClassType *a_;
public:
	bind_mem_fun_t(ClassType *a, MemFnc f):a_(a), fn_(f)
	{
	}

	bind_mem_fun_t(bind_mem_fun_t const & rhs)
	{
		if (this != &rhs)
		{
			*this = rhs;
		}
	}

	bind_mem_fun_t& operator = (bind_mem_fun_t const & rhs)
	{
		if (this != &rhs)
		{
			fn_ = rhs.fn_;
			a_ = rhs.a_;
		}
		return *this;
	}
	/*
	For compatiable with the std:*/
	typename std::mem_fun_t<ClassType, ReturnType> get_mem_fn()
	{
		return std::mem_fun_t<ClassType, ReturnType>(fn_);
	}
	ReturnType operator ()(ArgType t)
	{
		return (a_->*fn_)(t);
	}
};

/**
\ R C::F(Arg1, Arg2)
\ you may define more argument yourself.
*/
template <typename ReturnType, typename ClassType, typename ArgType1, typename ArgType2>
struct bind_mem_fun_t<ReturnType (ClassType::*)(ArgType1, ArgType2)>
{
	typedef ReturnType (ClassType::*MemFnc)(ArgType1, ArgType2);
	MemFnc fn_;
	ClassType *a_;
public:
	bind_mem_fun_t(ClassType *a, MemFnc f):a_(a), fn_(f)
	{
	}

	bind_mem_fun_t(bind_mem_fun_t const & rhs)
	{
		if (this != &rhs)
		{
			*this = rhs;
		}
	}

	bind_mem_fun_t& operator = (bind_mem_fun_t const & rhs)
	{
		if (this != &rhs)
		{
			fn_ = rhs.fn_;
			a_ = rhs.a_;
		}
		return *this;
	}

	typename std::mem_fun_t<ClassType, ReturnType> get_mem_fn()
	{
		return std::mem_fun_t<ClassType, ReturnType>(fn_);
	}

	ReturnType operator ()(ArgType1 t1, ArgType2 t2)
	{
		return (a_->*fn_)(t1, t2);
	}
};

/** Ulitily Function like the std::mem_fun
 \ R F()
*/
template <typename ReturnType, typename ClassType>
inline
bind_mem_fun_t<ReturnType (ClassType::*)()>
bind_mem_fun(ClassType *a, ReturnType (ClassType::*f)())
{
	return bind_mem_fun_t<ReturnType (ClassType::*)()>(a, f);
}

/**
\ R F(Arg1)
*/
template <typename ReturnType, typename ClassType, typename ArgType>
inline
bind_mem_fun_t<ReturnType (ClassType::*)(ArgType)>
bind_mem_fun(ClassType *a, ReturnType (ClassType::*f)(ArgType))
{
	return bind_mem_fun_t<ReturnType (ClassType::*)(ArgType)>(a, f);
}

/**
\ R F(Arg1, Arg2)
*/
template <typename ReturnType, typename ClassType, typename ArgType1, typename ArgType2>
inline
bind_mem_fun_t<ReturnType (ClassType::*)(ArgType1, ArgType2)>
bind_mem_fun(ClassType *a, ReturnType (ClassType::*f)(ArgType1, ArgType2))
{
	return bind_mem_fun_t<ReturnType (ClassType::*)(ArgType1, ArgType2)>(a, f);
}



//-----------------------------------------------------------
#endif // ! sentry
