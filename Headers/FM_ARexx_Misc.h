/*
 *  FlashMandel - FM_ARexx_Misc.h
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
 *    $Id: FM_ARexx_Misc.h,v 1.1 2004/03/01 17:24:50 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_ARexx_Misc.h,v $
 *    Revision 1.1  2004/03/01 17:24:50  eschwan
 *    First tracked version
 *
 *    Revision 1.0  2002/02/05 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifndef FM_AREXX_MISC_H
#define FM_AREXX_MISC_H

#include <exec/ports.h>
#include <rexx/storage.h>
#include <classes/arexx.h>

/* Prototypes */
BOOL LaunchARexxScript(struct ILBMInfo *Ilbm);
BOOL CreateARexxMenu(struct DiskObject *dobj);
BOOL SetARexxMenu(struct ILBMInfo *Ilbm);
void RemoveARexxMenu(struct ILBMInfo *Ilbm);
void FreeARexxMenu(void);
void HandleARexxMenu(struct ILBMInfo *Ilbm, UWORD MenuCode);
void LaunchIt(struct ILBMInfo *Ilbm, char *cmd, char *dir);

/* Defines */
#define REXX_MENU_ID 0x10000000
#define REXX_MENU_CMD_MASK 0x0000FFFF
#define REXX_MENUITEM_LAUNCH (REXX_MENU_ID | 0x00010000)

#endif /* FM_AREXX_MISC_H */
