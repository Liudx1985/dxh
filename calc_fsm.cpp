/*
 *  authour :liudx1985@gmail.com, @copyright 2016
 */
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <tuple>
#include <limits>
#include <cmath>
#include <functional>
#include <Windows.h>
using namespace std;


#ifndef __in
#	define __in
#endif
#ifndef __out
#	define __out
#endif
/* AST define
* num = [0x][0-9][.][0-9] #float number or hex integer or integer number
* term = num|"("expr")"
* factor = term|term "*" term|term "/" term;
* expr = factor "+" factor|factor "-" factor
*/
// binary functions
double match_number(string &strInput, string::iterator &itC);
double match_factor(string &strInput, string::iterator &itC);
double match_expr(string &strInput, string::iterator &itC);
// eval core
double eval(__in string &&strInput) {
    string::iterator b = (strInput.begin());
	return match_expr((strInput),b);
}
double eval(__in string &strInput) {
    string::iterator b = (strInput.begin());
	return match_expr((strInput),b);
}
// split the string into two part .
void split_op(__in const string &strInput, __in char ch,
	__out string &strFront, __out string &strBack) {
	// get the first sep.
	int wrap = 0;
	string::const_reverse_iterator rC = strInput.rbegin();
	for (; rC != strInput.rend(); ++rC) {
			// find the OPER:'+/-/*'
		if (*rC == ')') {
			++wrap;
		}
		else if (*rC == '(') {
			--wrap;
		}
		else if (wrap == 0 && *rC == ch) {
			int n = strInput.rend() - rC;
			strFront = strInput.substr(0, n - 1);
			strBack = strInput.substr(strInput.rend() - rC);
			return;
		}
	}
}

double match_expr(__in string &strInput, __out string::iterator &itC) {
	// match number or {`x` `op` `y`}, x y is fract
	string strF, strB;
	split_op(strInput, '+', strF, strB);
	if (!strF.empty() && !strB.empty()) {
		string::iterator itF = strF.begin();
		string::iterator itB = strB.begin();
		return match_expr(strF, itF) +
			match_expr(strB, itB);
	}
	split_op(strInput, '-', strF, strB);
	if (!strF.empty() && !strB.empty()) {
		string::iterator itF = strF.begin();
		string::iterator itB = strB.begin();
		return match_expr(strF, itF) -
			match_expr(strB, itB);
	}
	split_op(strInput, '*', strF, strB);
	if (!strF.empty() && !strB.empty()) {
		string::iterator itF = strF.begin();
		string::iterator itB = strB.begin();
		return match_expr(strF, itF) *
			match_expr(strB, itB);
	}
	split_op(strInput, '/', strF, strB);
	if (!strF.empty() && !strB.empty()) {
		string::iterator itF = strF.begin();
		string::iterator itB = strB.begin();
		return match_expr(strF, itF) /
			match_expr(strB, itB);
	}
	split_op(strInput, '^', strF, strB);
	if (!strF.empty() && !strB.empty()) {
		string::iterator itF = strF.begin();
		string::iterator itB = strB.begin();
		return pow(match_expr(strF, itF),
			match_expr(strB, itB));
	}
	return match_factor(strInput, itC);
}

double match_factor(__in string &strInput, __out string::iterator &itC) {
	// match factor `(expr)` | `number`;
	double f = 0;
	while (itC != strInput.end() && *itC == ' ') {
		++itC; //skip all whitespace
	};
	if (itC == strInput.end()) {
		// all ''
		return f;
	}
	if (*itC == '(') {
		int wrap = 1;
		string::iterator rC = ++itC;
		for (; rC != strInput.end(); ++rC) {
			// find the +/-/*//
			if (*rC == '(')
				++wrap;
			else if (*rC == ')')
				--wrap;
			if (wrap == 0 && *rC == ')')
				break;
		}
		string strExpr;
		strExpr.assign(itC, rC);
		string::iterator b = strExpr.begin();
		f = match_expr(strExpr, b);
		while (*itC == ' ') {
			++itC; //skip all whitespace
		};
		itC = rC + 1;
	}
	else
		f = match_number(strInput, itC);
	while (itC != strInput.end() && *itC == ' ') {
		++itC; //skip all whitespace
	};
	return f;
}

double match_number(__in string &strInput, __out string::iterator &itC) {
	// match number only.support hex radix numbers.
	double f = 0;
	int iDot = 0;
	bool bNeg = false;
	char radix = 10; // radix
	char arRC[] = "0123456789ABCDEF";

	while (itC != strInput.end() && *itC == ' ') {
		++itC; //skip all whitespace
	};
	if (*itC == '-') {
        ++itC;
		bNeg = true;
	}
	if (*itC == '0') {
	    ++itC;
        if (*itC == 'x'){
            ++itC;
            radix = 16;
       }
	}
	string::iterator itDot = std::find(itC, strInput.end(), '.');
	for (; itC != itDot; ++itC) {
        const char *pcChar = strchr(arRC, (*itC));
		if (NULL == pcChar || pcChar >= (arRC + radix))
		{
			break; // invalid charactors
		}
		else {
			int n = (pcChar) - arRC;
			f = f * radix + n;
		}
	}
	if (itDot != strInput.end() && radix == 10) { // hex not have dot number.
		float p = 0;
		for (++itDot; itDot != strInput.end(); ++itDot) {
			if (*itDot >= '0' && *itDot <= '9') {
				int n = (*itDot) - '0';
				p = p * radix + n;
			}
			else {
				break;
			}
		}
		int iP = 1 + itC - itDot;
		itC = itDot;
		f = f + p * pow(radix, iP);
	}

	while (itC != strInput.end() && *itC == ' ') {
		++itC; //skip all whitespace
	};
	if (bNeg)
		f = -f;
	return f;
}

#define STR(x) #x
#define DEBUG_ASSERT(x, r) {\
double f = x;\
(f == r) ? cout << "test pass:\t" << STR(x) << '=' << f << '\n':\
cout << "!test failed:\t" << STR(x) << '=' << f << ", != " << r << '\n';\
}\

int main()
{
	DEBUG_ASSERT(eval("(0x10-12) + 26"), 30);
	DEBUG_ASSERT(eval("10 - 12 + 26 / 2"), 11);
	DEBUG_ASSERT(eval("10-12 + 26"), 24);
	DEBUG_ASSERT(eval("10+(-10 + 24)"), 24);
	DEBUG_ASSERT(eval("10* (10 - 4 ^ 1)"), 60);
	DEBUG_ASSERT(eval("(10* 10) - 40"), 60);
	DEBUG_ASSERT(eval("(2+3)*4-(7-3)*2"), 12);

	std::string strCin;
	while (true) {
		cout << ">>";
		std::getline(std::cin, strCin);
		if (strCin.empty()) {
			continue;
		}
		__int64 begin = GetTickCount();
		cout << eval(strCin);
		cout << "\n" << (GetTickCount() - begin) << "ms\n";
	}
}
