/*
 *  FlashMandel - myMath.h
 *
 *  Copyright (C) 2003 - 2004  Edgar Schwan
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted, 
 *  provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear 
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed 
 *  as patches to released version.
 *  
 *  This software is provided "as is" without express or implied warranty.
 */
/*
 *    $Id: myMath.h,v 1.2 2004/09/15 00:00:00 eschwan Exp $
 *
 *    Part of myMath-Library
 *
 *    $Log: myMath.h,v $
 *    Revision 1.3  2020/03/19 00:00:00  dpapararo
 *    Removed all PPC calls
 *
 *    Revision 1.2  2004/09/15 00:00:00  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 19:42:11  eschwan
 *    First tracked version
 *
 *    Revision 1.0  2003/09/09 00:00:00  eschwan
 *    Initial release.
 */

#ifndef _MY_MATH_H
#define _MY_MATH_H

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>

#define MYMATH_DEFBASE 100
#define IsMinListEmpty(x) ( ((x)->mlh_TailPred) == (struct MinNode *)(x) )

//typedef UBYTE BufType;

struct MyMath_struct {
	struct MinNode	    mymath_node;
	UBYTE				mymath_type;
	BYTE				mymath_sign;
	LONG				mymath_exp;
	ULONG				mymath_len;
	UBYTE	 			*mymath_buffer;
	ULONG				mymath_bsize;
	};

#define MYMATHTYPE_TEMPVAR 1
#define MYMATHTYPE_USERVAR 2
#define MYMATHTYPE_CONSTVAR 3

typedef struct MyMath_struct *MyMath;

struct MyMathHandle {
	UBYTE			mmh_errorcode;
	UWORD			mmh_base;
	ULONG			mmh_maxdigits;
	struct MinList	mmh_varlist;
	UBYTE		   *mmh_mul_div;
	UBYTE		   *mmh_mul_rem;
	MyMath			mmh_temp1;
	MyMath			mmh_temp2;
	MyMath			mmh_temp_conv;
	MyMath			mmh_const_1;
	MyMath			mmh_const_2;
	MyMath			mmh_const_3;
	MyMath			mmh_const_4;
	MyMath			mmh_const_5;
	};

/* prototypes */
APTR myMath_Init(ULONG maxdigits);
VOID myMath_Release(APTR handle);
MyMath myMath_Allocate(APTR handle);
VOID myMath_Free(MyMath a);
BOOL myMath_Add(APTR handle, MyMath r, MyMath a, MyMath b);
BOOL myMath_Sub(APTR handle, MyMath r, MyMath a, MyMath b);
BOOL myMath_Mul(APTR handle, MyMath r, MyMath a, MyMath b);
BOOL myMath_Div(APTR handle, MyMath r, MyMath a, MyMath b); /* NOT IMPLEMENTED YET */
BOOL myMath_Round(APTR handle, MyMath r, MyMath a, ULONG p); /* NOT IMPLEMENTED YET */
BOOL myMath_Abs(APTR handle, MyMath r, MyMath a); /* NOT IMPLEMENTED YET */
LONG myMath_Cmp(APTR handle, MyMath a, MyMath b);
BOOL myMath_Dbl2MyMath(APTR handle, MyMath r, DOUBLE a);
DOUBLE myMath_MyMath2Dbl(APTR handle, MyMath a);
BOOL myMath_Str2MyMath(APTR handle, MyMath r, UBYTE *s);
BOOL myMath_MyMath2Str(APTR handle, UBYTE *r, MyMath a, ULONG size);
int myMath_GetErrorCode(APTR handle);
BOOL myMath_SetPrecision(APTR handle, ULONG p);
ULONG myMath_GetPrecision(APTR handle);
VOID myMath_Copy(APTR handle, MyMath r, MyMath a);
#endif /* _MY_MATH_H */
