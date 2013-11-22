/*
 *	Logic.hpp
 *
 *	DATE :	2010.6.8
 *
 *  Contributors:
 *  Created by dxliu <junjie@163.com.cn>
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef _LOGIC_HPP_
#define _LOGIC_HPP_

namespace ns_logic
{
//#include <cstdio>

#define ANSI            /* Comment out for UNIX version     */

#ifdef ANSI             /* ANSI compatible version          */
#	include <stdarg.h>
	static bool one_true (bool first, ... ); // one true, others false.
	static bool one_false(bool first, ... ); // one false,others true
	static bool equal_bools(bool first, ...); // all of true / flase
#else                   /* UNIX compatible version          */
#	include <varargs.h>
	static bool one_true( va_list );
	static bool one_false( va_list );
#endif


//  call like this one_true(b1, b2, ..., -1);

	/* Returns the average of a variable list of integers. */
#ifdef ANSI             /* ANSI compatible version    */

	// return true when one of the bools variant is true.the variable arguments list should end with -1.
	// like : one_true(b1, b2, .., -1);
	static bool one_true( bool first, ... )
	{
		int count = 0, sum = 0, i = first;
		va_list marker;

		va_start( marker, first );     /* Initialize variable arguments. */
		while( i != -1 )
		{
			sum += i;
			count++;
			i = va_arg( marker, int);
		}
		va_end( marker );              /* Reset variable arguments.      */
		return ( sum == 1);
	}

	static bool one_false( bool first, ... )
	{
		int count = 0, sum = 0, i = first;
		va_list marker;

		va_start( marker, first );     /* Initialize variable arguments. */
		while( i != -1 )
		{
			sum += (i==false)?1:0;
			count++;
			i = va_arg( marker, int);
		}
		va_end( marker );              /* Reset variable arguments.      */
		return ( sum == 1);
	}

	// Return true if all true or all false,else false .
	static bool equal_bools(bool first, ...)
	{
		int count = 0, sum = 0, i = first;
		bool bRet = true;
		va_list marker;

		va_start( marker, first );     /* Initialize variable arguments. */
		while( i != -1 ) {
			if (i ^ (int)first) {
				bRet = false;
				break;
			}
			i = va_arg( marker, int);
		}
		va_end( marker );          /* Reset variable arguments.      */
		return ( bRet );
	};

#else       /* UNIX compatible version must use old-style definition.  */
	static bool one_true( va_alist )
		va_dcl
	{
		int i, count, sum;
		va_list marker;

		va_start( marker );            /* Initialize variable arguments. */
		for( sum = count = 0; (i = va_arg( marker, int)) != -1; count++ )
			sum += i;
		va_end( marker );              /* Reset variable arguments.      */
		return (sum == 1);
	}

	static bool one_false( va_alist )
		va_dcl
	{
		int i, count, sum;
		va_list marker;

		va_start( marker );            /* Initialize variable arguments. */
		for( sum = count = 0; (i = va_arg( marker, int)) != -1; count++ )
			sum += false==i?1:0;
		va_end( marker );              /* Reset variable arguments.      */
		return (sum == 1);
	}
#endif

};

#endif //_INC_LOGICAL_HPP_
