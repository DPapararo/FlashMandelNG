/*
 *  FlashMandel - FM_ConfirmReq_React.h
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
 *    $Id: FM_ConfirmReq_React.h,v 1.1 2004/03/01 17:25:10 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_ConfirmReq_React.h,v $
 *    Revision 1.2  2020/12/14 00:00:00  eschwan
 *    Made internal functions static
 *
 *    Revision 1.1  2004/03/01 17:25:10  eschwan
 *    First tracked version
 *
 *    Revision 2.0  2001/10/13 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifndef FM_CONFIRM_REQ_REACT_H
#define FM_CONFIRM_REQ_REACT_H

/* Prototypes */
BOOL Do_PrintRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top);
BOOL Do_ExitRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top);
BOOL Do_NewScrRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top);
BOOL Do_RenderRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top);
BOOL Do_OverwriteRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top);

#endif /* FM_CONFIRM_REQ_REACT_H */
