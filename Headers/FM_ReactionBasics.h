/*
 *  FlashMandel - FM_Reaction_Basics.h
 *
 *  Copyright (C) 2002 - 2004  Edgar Schwan
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
 *    $Id: FM_ReactionBasics.h,v 1.1 2004/03/01 17:25:59 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_ReactionBasics.h,v $
 *    Revision 1.1  2004/03/01 17:25:59  eschwan
 *    First tracked version
 *
 *    Revision 2.2  2002/02/05 00:00:00  eschwan
 *    Last no-cvs-release.
 *
 *    Revision 2.0  2002/01/01 00:00:00  eschwan
 *    New arguments in OpenReactionWindow().
 */

#ifndef FM_REACTION_BASICS_H
#define FM_REACTION_BASICS_H

#include <intuition/classusr.h>
#include <libraries/resource.h>

/* Prototypes */
BOOL InitReaction(char *ScrPubName);
void FreeReaction(void);
struct ReactWinData *OpenReactionWindow(RESOURCEID WinResID, RESOURCEID GadResID, char *ScrPubName);
void CloseReactionWindow(struct ReactWinData *rwd);
STRPTR GetDefStr(LONG id);
ULONG DisplayError(struct Window *win, LONG id, ULONG ErrorLevel);
void CloseDownDisplay(struct Screen *screen);
void LocalizeMenu(struct NewMenu *nm);
void UpdatePrecMenuItem(WORD p, struct Menu *menu, struct ILBMInfo *ilbm);

/* Structures */
struct ReactWinData {
	Object 			  *RWD_WindowObject;
	struct Gadget   **RWD_GadgetArray;
	struct Window	  *RWD_IWindow;
	ULONG					RWD_WSignals;
	};

/* Defines */
#define RESULT_NOTHING 0
#define RESULT_OK 1
#define RESULT_CANCEL -1
#define RESULT_ERROR -2

#define LAST_CAT_ENTRY Prec_TXT_Cancel

#define CATSTR(id)        GetCatalogStr(CatalogPtr, id, GetDefStr(id))
#define CATSTRSTR(id,str) GetCatalogStr(CatalogPtr, id, (STRPTR) str)

#endif /* FM_REACTION_BASICS_H */
