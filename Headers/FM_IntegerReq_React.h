/*
 *  FlashMandel - FM_IntegerReq_React.h
 *
 *  Copyright (C) 2002 - 2020  Edgar Schwan
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
 *    $Id: FM_IntegerReq_React.h,v 1.1 2004/03/01 17:25:37 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_IntegerReq_React.h,v $
 *    Revision 1.2  2020/12/14 00:00:00  eschwan
 *    Made internal functions static
 *
 *    Revision 1.1  2004/03/01 17:25:37  eschwan
 *    First tracked version
 *
 *    Revision 2.0  2001/10/13 00:00:00  eschwan
 *    Last no-cvs-release.
 * 
 *    Revision 2.1  2019/02/09 00:00:00  dpapararo
 *    changed return value Do_IterationsRequest() from ULONG to LONG
  */

#ifndef FM_INTEGER_REQ_REACT_H
#define FM_INTEGER_REQ_REACT_H

/* Prototypes */
ULONG Do_CycleDelayRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top, ULONG Value);
LONG Do_IterationsRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top, LONG Value);
ULONG Do_PrecisionRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top, ULONG Value);

#endif /* FM_INTEGER_REQ_REACT_H */
