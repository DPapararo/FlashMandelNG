/*
 *  FlashMandel - FM_PalettePref_React.h
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
 *    $Id: FM_PalettePref_React.h,v 1.1 2004/03/01 17:25:45 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_PalettePref_React.h,v $
 *    Revision 1.2  2020/12/15 00:00:00  eschwan
 *    Made internal functions static
 *
 *    Revision 1.1  2004/03/01 17:25:45  eschwan
 *    First tracked version
 *
 *    Revision 2.1  2002/06/08 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifndef FM_PALETTE_PREF_REACT_H
#define FM_PALETTE_PREF_REACT_H

// #define MIN(a,b)    ((a)<(b)?(a):(b))
// #define MAX(a,b)    ((a)>(b)?(a):(b))

/* Structures */
struct RGB {
	ULONG RGB_Red;
	ULONG RGB_Green;
	ULONG RGB_Blue;
	};

struct loadrgb_head {
	UWORD		lrgb_count;
	UWORD		lrgb_first;
	};

struct loadrgb {
	struct loadrgb_head	loadrgb_lrgb;
	struct RGB				loadrgb_palette;
	};

/* Prototypes */
BOOL Do_PalettePrefRequest(struct Window *Win, char *PubScreenName, WORD Left, WORD Top, struct loadrgb *Palette32);

#endif /* FM_PALETTE_PREF_REACT_H */
