/*
 *  FlashMandel - FM_ARexx_React.h
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
 *    $Id: FM_ARexx_React.h,v 1.2 2004/10/04 00:00:00 eschwan Exp $
 *
 *    .
 *
 *    $Log: FM_ARexx_React.h,v $
 *    Revision 1.3  2020/12/12 00:00:00  eschwan
 *    Modified for "no reactor" code
 *
 *    Revision 1.2  2004/10/04 00:00:00  eschwan
 *    Added prototypes
 *
 *    Revision 1.1  2004/03/01 17:24:59  eschwan
 *    First tracked version
 *
 *    Revision 1.2  2002/02/05 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifndef FM_AREXX_REACT_H
#define FM_AREXX_REACT_H

#include <exec/ports.h>
#include <rexx/storage.h>
#include <classes/arexx.h>
#include <libraries/resource.h>

#include "FlashMandel.h"
// #include "CompilerSpecific.h"

/* Prototypes */
BOOL InitARexx(void);
void FreeARexx(void);

ULONG HandleARexxEvents(struct ILBMInfo *Ilbm);

/* Defines */

#define MIN_COLOR_REMAP 1
#define MAX_COLOR_REMAP 8

/* Structures */
struct FM_ARexxEvent {
	struct Node	FMRXC_Node;
	ULONG			FMRXC_Command;
	APTR			FMRXC_Args;
	};

/* Command-Structures */
#if 1
struct FM_RxCmd_TEST {
	BOOL		FMRC_TEST_Switch;
	STRPTR	FMRC_TEST_String;
	LONG		FMRC_TEST_Number;
	STRPTR	FMRC_TEST_NeedStr;
	STRPTR	FMRC_TEST_Follow;
	};
#endif

struct FM_RxCmd_QUIT {
	BOOL		FMRC_QUIT_Force;
	BOOL		FMRC_QUIT_Fast;
	};

struct FM_RxCmd_LOADPIC {
	STRPTR	FMRC_LOADPIC_Path;
	BOOL 		FMRC_LOADPIC_Fast;
	};

struct FM_RxCmd_SET_SCREENATTRS {
	ULONG		FMRC_SETSCR_ModeID;
	WORD 		FMRC_SETSCR_Width;
	WORD 		FMRC_SETSCR_Height;
	WORD 		FMRC_SETSCR_Depth;
	WORD 		FMRC_SETSCR_OScanType;
	BOOL		FMRC_SETSCR_Fast;
	};

struct FM_RxCmd_SET_FONT {
	STRPTR	FMRC_SETFNT_FontName;
	UWORD		FMRC_SETFNT_Size;
	UWORD		FMRC_SETFNT_Style;
	BOOL 		FMRC_SETFNT_Fast;
	};

#if 0
struct FM_RxCmd_ZOOM {
	LONG 		FMRC_ZOOM_Left;
	LONG 		FMRC_ZOOM_Top;
	LONG 		FMRC_ZOOM_Width;
	LONG 		FMRC_ZOOM_Height;
	LONG 		FMRC_ZOOM_Repeats;
	LONG 		FMRC_ZOOM_Steps;
	BOOL 		FMRC_ZOOM_Fast;
	};

struct FM_RxCmd_PRINT {
	BOOL 		FMRC_PRINT_Force;
	};
#endif

   /* prototypes */

/* FM_ARexx_React.c */
BOOL AddARexxEvent(ULONG command, APTR args);
void RemoveARexxEvent(struct FM_ARexxEvent *event);
BOOL IsNumber(char *string);
void ARexxFunc_Test(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_About(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_Quit(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SysInfo(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_Render(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_Zoom(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_Help(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_LoadPicture(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SavePicture(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_LoadPalette(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SavePalette(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_Print(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_ShowTitle(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_ShowElapsed(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_ShowPreview(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_Undo(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_DoColorcycling(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_StartTimer(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_StopTimer(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_PlaySound(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_StopSound(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetIterations(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetPower(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetPriority(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetColorRemap(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetFractalType(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetCPUType(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetDelay(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));

/* FM_ARexx_React2.c */
void ARexxFunc_SetCoordinates(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetCoordinates(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetIterations(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetPower(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetPriority(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetColorRemap(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetFractalType(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetCPUType(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetFrame(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetFrame(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetDelay(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetPalette(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetPalette(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetScreenAttrs(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetScreenAttrs(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_SetFont(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));
void ARexxFunc_GetFont(REG(a0, struct ARexxCmd *cmd), REG(a1, struct RexxMsg *rm));

#endif /* FM_AREXX_REACT_H */
