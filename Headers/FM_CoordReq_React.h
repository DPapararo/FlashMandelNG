/*
 *  FlashMandel - FM_CoordReq_React.h
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
 *    $Id: FM_CoordReq_React.h,v 1.1 2004/03/01 17:25:18 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_CoordReq_React.h,v $
 *    Revision 1.2  2020/12/17 00:00:00  eschwan
 *    Made internal functions static
 *
 *    Revision 1.1  2004/03/01 17:25:18  eschwan
 *    First tracked version
 *
 *    Revision 2.0  2001/10/13 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#include <exec/types.h>

#ifndef FM_COORD_REQ_REACT_H
#define FM_COORD_REQ_REACT_H

#define MAXCHARS     20

/* Prototypes */
int16 Do_CoordinatesRequest(struct Window *Win, uint8 *PubScreenName, int16 Left, int16 Top);

#endif /* FM_COORD_REQ_REACT_H */
