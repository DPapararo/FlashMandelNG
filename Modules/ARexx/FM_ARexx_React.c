/*
 *  FlashMandel - FM_ARexx_React.c
 *
 *  Copyright (C) 2002 - 2022  Edgar Schwan
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
 *    $Id: FM_ARexx_React.c,v 2.1 dpapararo Exp $
 *
 *    Functions for Reaction-ARexx.
 *
 *    $Log: FM_ARexx_React.c,v $
 *
 *    Revision 2.1  2022/1/04 00:00:00  dpapararo
 *	  Implemented OS4 API Calls where possible
 *	  Cleaned code
 *
 *    Revision 2.0  2021/1/24 00:00:00  dpapararo
 *    Removed "COLORS" global var
 *
 *    Revision 1.9  2021/1/17 00:00:00  dpapararo
 *    Modified DrawFractal function parameters
 *   
 *    Revision 1.8  2020/12/11 00:00:00  eschwan
 *    Completed "no reactor" code.
 *
 *    Revision 1.7  2020/03/19 00:00:00  dpapararo
 *    Various fixes.

 *    Revision 1.6  2005/02/26 00:00:00  eschwan
 *    MANDChunk is now a pointer.
 *
 *    Revision 1.5  2004/10/22 00:00:00  eschwan
 *    Bugfix in IsNumber()
 *
 *    Revision 1.4  2004/10/05 00:00:00  eschwan
 *    Moved some functions to "FM_ARexx_React2.c"
 *
 *    Revision 1.3  2004/09/30 00:00:00  eschwan
 *    Added no-reactor-support
 *
 *    Revision 1.2  2004/06/29 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:23:10  eschwan
 *    First tracked version
 *
 *    Revision 1.7  2002/11/26 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifdef __amigaos4__
#define __USE_INLINE__
#endif /* __amigaos4__ */

#include <exec/ports.h>
#include <intuition/classusr.h>
#include <reaction/reaction.h>
#include <classes/arexx.h>
#define NO_PROTOS
#include <iffp/ilbmapp.h>
#undef NO_PROTOS
#include <devices/timer.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/soundclass.h>
#include <libraries/asl.h>
#include <dos/dostags.h>

#ifdef __GNUC__
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/rexxsyslib.h>
#include <proto/iffparse.h>
#define __NOLIBBASE__
#include <proto/timer.h>
#undef __NOLIBBASE__
#include <proto/datatypes.h>
#include <proto/arexx.h>
#ifndef __amigaos4__
#include <clib/alib_protos.h>
#endif /* !__amigaos4__ */
#else /* __GNUC__ */
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/locale_protos.h>
#include <clib/rexxsyslib_protos.h>
#include <clib/iffparse_protos.h>
#include <clib/timer_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/alib_protos.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/rexxsyslib_pragmas.h>
#include <pragmas/iffparse_pragmas.h>
#include <pragmas/timer_pragmas.h>
#include <pragmas/datatypes_pragmas.h>

#include <clib/arexx_protos.h>
#include <pragmas/arexx_pragmas.h>
#endif /* __GNUC__ */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "FM_ARexx_React.h"
#include "FM_ReactionBasics.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include "FM_SysInfoReq_React.h"
#include "FM_ConfirmReq_React.h"
#include "FM_CoordReq_React.h"
#include "FM_InfoReq_React.h"
#include "FM_IntegerReq_React.h"
#include "FM_PalettePref_React.h"
#include "Headers/FlashMandel.h"

#ifndef __amigaos4__
#include "Compilerspecific.h"
#else /* !__amigaos4__ */
#include <amiga_compiler.h>
#endif /* !__amigaos4__ */

#ifdef FM_AREXX_SUPPORT

extern struct Catalog *CatalogPtr;
#ifndef __amigaos4__
extern struct Library *DataTypesBase;
extern struct Library *RexxSysBase;
#else /* !__amigaos4__ */
struct Library *TimerBase = NULL;
struct TimerIFace *ITimer = NULL;
struct TimeRequest *TimerIO = (struct TimeRequest *) NULL;
struct MsgPort *TimerMP = (struct MsgPort *) NULL;
int32 TimerError = -1L;
#endif /* !__amigaos4__ */

struct Library *ARexxBase = NULL;
#ifdef __amigaos4__
struct ARexxIFace *IAREXX = NULL;
#endif /* __amigaos4__ */

uint32 ARexxError = 0;

extern struct LoadSaveFMChunk *LSFMChunk;
extern char USERNAME_STRING[MAX_FILELEN], COPYRIGHT_STRING[BARLEN];
extern char FMSCREENNAME[MAXPUBSCREENNAME + 1], MYPATH[MAX_PATHLEN];
extern char MYFONT[MAX_FILELEN];
extern uint8 *PIXMEM, *GFXMEM, *ARGBMEM, *RGBMEM;
extern uint32 *RNDMEM, PIXELVECTOR[4];
extern int16 UNDOCOUNTER;
extern int16 ZOOMLINE[5 * 2];
extern struct ILBMInfo MYILBM;
extern struct BitMap *MYBITMAP;
extern struct Border MYBORDER;
extern uint32 ELAPSEDTIME, MASK;
extern uint16 ZOOMPOINTER [];
extern struct Menu *MAINMENU;
extern int16 res;
extern struct MandelChunk *MANDChunk;
extern uint32 *PALETTE;
extern CPTR *VINFO;
extern struct Chunk FLASHMANDEL_CHUNK;
extern struct Chunk USERNAME_CHUNK;
extern struct Chunk COPYRIGHT_CHUNK;
extern int32 PRIORITY, DELAY;
extern int16 MX1, MY1, MX2, MY2, W, H;
extern struct TextAttr MYFONTSTRUCT;
extern int16 ForceAbort;
extern uint32 UPOWER;

/* global vars */
Object *AREXXOBJ = NULL;
uint32 AREXXSIGNAL = -1;
struct List AREXXEVENTLIST = { 0 };

/* prototypes */
int16 AddARexxEvent (uint32 command, APTR args);
void RemoveARexxEvent (struct FM_ARexxEvent *event);
int16 IsNumber (char *string);

/* AddARexxEvent(): Add an event to the ARexxEvent-List.

        SYNOPSIS: int16 success = AddARexxEvent
                        (
                        uint32       command;
                        APTR        args;
                        );

        INPUTS:     command:
                            ID of the ARexx-command.

                        args:
                            Pointer to the command-specific arguments.

        RETURNS:        success:
                            Is TRUE. if successfull.
*/
int16 AddARexxEvent (uint32 command, APTR args)
{
  int16 success = FALSE;
  struct FM_ARexxEvent *event = NULL;

  	if (event = (struct FM_ARexxEvent *) AllocVec (sizeof (struct FM_ARexxEvent),
				 MEMF_ANY | MEMF_CLEAR))
    {
      	event->FMRXC_Command = command;
      	event->FMRXC_Args = args;

      	AddTail (&AREXXEVENTLIST, (struct Node *) event);
      	success = TRUE;
    }
  
  	return (success);
}

/* RemoveARexxEvent(): Remove an event from the ARexxEvent-List.

        SYNOPSIS: void = RemoveARexxEvent
                        (
                        struct FM_ARexxEvent        *event;
                        );

        INPUTS:     event:
                            Pointer to the event-structure.

        RETURNS:        -
*/
void RemoveARexxEvent (struct FM_ARexxEvent *event)
{
  	Remove ((struct Node *) event);
  	if (event->FMRXC_Args) FreeVec (event->FMRXC_Args);
  	FreeVec ((APTR) event);
}

/* IsNumber(): Check, if string is a number (float or int).

        SYNOPSIS: int16 isnum = IsNumber
                        (
                        char        *string;
                        );

        INPUTS:     string:
                            Pointer to a string.

        RETURNS:        isnum:
                            Is TRUE, if string is a number.
*/
int16 IsNumber (STRPTR string)
{
  int16 Success = FALSE;

  	if (!(*string) && (!string)) goto ExitIsNumber;
  	if ((*string == '+') || (*string == '-')) string++;
  
  	while (*string)
    {
      	if (!(isdigit (*string))) 
			if (*string != '.') goto ExitIsNumber;
      	
		string++;
    }

	Success = TRUE;
	  	
ExitIsNumber:	
	return (Success);
}

/***  ARexx-functions  ***/
/* ARexxFunc_Test(): Handle ARexx-Command 'TEST'.   Template: "SWITCH/S,STRING/K,NUMBER/N,NEEDSTR/A,FOLLOW/F" */
void ARexxFunc_Test (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  struct FM_RxCmd_TEST *args = NULL;
  uint32 size1 = 0, size2 = 0, size3 = 0, memsize;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003;	/* no memory available */

  	if (cmd->ac_ArgList[1]) size1 = Strlen ((const STRPTR) cmd->ac_ArgList[1]) + 1;
  	if (cmd->ac_ArgList[3]) size2 = Strlen ((const STRPTR) cmd->ac_ArgList[3]) + 1;
  	if (cmd->ac_ArgList[4]) size3 = Strlen ((const STRPTR) cmd->ac_ArgList[4]) + 1;
  	memsize = sizeof (struct FM_RxCmd_TEST) + size1 + size2 + size3;

  	if (args = (struct FM_RxCmd_TEST *) AllocVec (memsize, MEMF_ANY | MEMF_CLEAR))
    { /* Copy arguments */
      	if (cmd->ac_ArgList[0]) args->FMRC_TEST_Switch = (int16) (cmd->ac_ArgList[0]);
      	if (size1)
		{
	  		STRPTR string = (STRPTR) ((uint32) args + sizeof (struct FM_RxCmd_TEST));
	  		//Strlcpy (string, (const STRPTR) cmd->ac_ArgList[1], sizeof (string));
	  		strcpy (string, (const STRPTR) cmd->ac_ArgList[1]);	  
	  		args->FMRC_TEST_String = string;
		}
      	
		if (cmd->ac_ArgList[2]) args->FMRC_TEST_Number = *(int32 *) (cmd->ac_ArgList[2]);
      	if (size2)
		{
	  		STRPTR string = (STRPTR) ((uint32) args + sizeof (struct FM_RxCmd_TEST) + size1);
	  		// Strlcpy (string, (const STRPTR) cmd->ac_ArgList[3], sizeof (string));
	  		strcpy (string, (const STRPTR) cmd->ac_ArgList[3]);
	  		args->FMRC_TEST_NeedStr = string;
		}
      	
		if (size3)
		{	  
			STRPTR string =
	    	(STRPTR) ((uint32) args + sizeof (struct FM_RxCmd_TEST) + size1 + size2);
	  		// Strlcpy (string, (const STRPTR) cmd->ac_ArgList[4], sizeof (string));
	  		strcpy (string, (const STRPTR) cmd->ac_ArgList[4]);
	  		args->FMRC_TEST_Follow = string;
		}

      	if (AddARexxEvent (0xFFFFFFFF, (APTR) args))
		{
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
      	
		else FreeVec (args);
    }
}

/****   PROJECT   ****/
/* ARexxFunc_About(): Handle ARexx-Command 'ABOUT'.    Template: - */
void ARexxFunc_About (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
#if 0				/* external */
  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003;	/* no memory available */
  	if (AddARexxEvent (ABOUT_MSG, NULL))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
#else /* internal */
  	ModifyIDCMP (MYILBM.win, NULL);
  	ClearMenuStrip (MYILBM.win);
#ifdef FM_REACT_SUPPORT
  	Do_InfoRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
  	About (MYILBM.win);
#endif /* FM_REACT_SUPPORT */
  	ResetMenuStrip (MYILBM.win, MAINMENU);
  	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
#endif
}

/* ARexxFunc_SysInfo(): Handle ARexx-Command 'SYSINFO'.    Template: - */
void ARexxFunc_SysInfo (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
#if 0				/* external */
  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003;	/* no memory available */
  	if (AddARexxEvent (SYSINFO_MSG, NULL))
    {
      cmd->ac_RC = RC_OK;
      cmd->ac_RC2 = 0;
    }
#else /* internal */
  	ModifyIDCMP (MYILBM.win, NULL);
  	ClearMenuStrip (MYILBM.win);
#ifdef FM_REACT_SUPPORT
  	Do_SysInfoRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
  	SystemInfo (MYILBM.win);
#endif /* FM_REACT_SUPPORT */
  	ResetMenuStrip (MYILBM.win, MAINMENU);
  	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
#endif
}

/* ARexxFunc_Help(): Handle ARexx-Command 'HELP'.   Template: - */
void ARexxFunc_Help (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
#if 0
  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003;	/* no memory available */
  	if (AddARexxEvent (SHOWGUIDE_MSG, NULL))
    {
      cmd->ac_RC = RC_OK;
      cmd->ac_RC2 = 0;
    }
#else
  	if (!(system (CATSTR (TXT_CMD_Guide))))
    {
      cmd->ac_RC = RC_ERROR;
      cmd->ac_RC2 = ERR10_012;	/* error return from function */
    }
#endif
}

/* ARexxFunc_LoadPicture(): Handle ARexx-Command 'LOAD_PICTURE'.   Template: "PATH,FAST/S" */
void ARexxFunc_LoadPicture (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR path = (STRPTR) cmd->ac_ArgList[0];
  int16 fast = (int16) (cmd->ac_ArgList[1]);

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003;	/* no memory available */
#if 1				/* external */
  {
    struct FM_RxCmd_LOADPIC *args = NULL;
    uint32 size1 = 0, memsize = 0;

    if (path) size1 = Strlen ((const STRPTR) path) + 1;
    
	else
    {
		if (! (FileRequest (MYILBM.win, CATSTR (TXT_LoadPictureTitle), "Pictures", PICTURES_DRAWER, FALSE)))
		{
	    	cmd->ac_RC = RC_WARN;
	    	cmd->ac_RC2 = 0;
	    	return;
	  	}
		
		else
	  	{
	    	path = (STRPTR) &MYPATH;
	    	size1 = Strlen ((const STRPTR) path) + 1;
	  	}
    }
    
	memsize = sizeof (struct FM_RxCmd_LOADPIC) + size1;

    if (args = (struct FM_RxCmd_LOADPIC *) AllocVec (memsize, MEMF_ANY | MEMF_CLEAR))
    {	/* Copy arguments */
		args->FMRC_LOADPIC_Fast = fast;
	
		if (size1)
	  	{
	    	STRPTR string = (STRPTR) ((uint32) args + sizeof (struct FM_RxCmd_LOADPIC));
	    	// Strlcpy (string, (const STRPTR) path, sizeof (path));
	    	strcpy (string, (const STRPTR) path);			
	    	args->FMRC_LOADPIC_Path = string;
	  	}

		// args->FMRC_LOADPIC_Path = NULL;

		if (AddARexxEvent (LOADPICTURE_MSG, (APTR) args))
	  	{
	    	cmd->ac_RC = RC_OK;
	    	cmd->ac_RC2 = 0;
	  	}
		
		else FreeVec (args);
    }
  }
#else /* internal */
  {
    int32 Error;

    cmd->ac_RC = RC_FATAL;
    cmd->ac_RC2 = ERR10_003; /* no memory available */
    if (path)
    {
		PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
	  
	    SaveCoords (MYILBM.win);
		
	    if (!(Error = QueryMandFile (&MYILBM, LSFMChunk, path)))
	    {
			if (!(fast)) Fade (MYILBM.win, ARGBMEM, RGBMEM, PALETTE, 25L, 1L, TOBLACK);
			
			CloseDisplay (&MYILBM);
					
			if (!MakeDisplay (&MYILBM))
		  	{
		    	DisplayError (MYILBM.win, TXT_ERR_MakeDisplay, 20L);
				// ARexxFunc_Quit(cmd, rm);
		    	return;
		  	}
			
			MASK &= ~ZMASK;
			
			if (TMASK & MASK) ShowTitle (MYILBM.scr, FALSE);
			
			if (LoadMandPic (&MYILBM, path)) DisplayError (MYILBM.win, TXT_ERR_LoadMandPic, 5L);
			
			else
		  	{
		    	cmd->ac_RC = RC_OK;
		    	cmd->ac_RC2 = 0;
		  	}
			
			if (TMASK & MASK) ShowTitle (MYILBM.scr, TRUE);
			GetRGB32 (MYILBM.vp->ColorMap, 0L, (uint32) MYILBM.vp->ColorMap->Count, PALETTE + 1L);
			CheckMenu (MYILBM.win);
	    }
	    
		else
	    {
			DisplayError (MYILBM.win, TXT_ERR_QueryMandPic, 0);
			cmd->ac_RC = RC_ERROR;
			cmd->ac_RC2 = ERR10_012; /* error return from function */
	    }
	  
		PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
    }
    
	else
    {
		if (AddARexxEvent (LOADPICTURE_MSG, NULL))
	  	{
	    	cmd->ac_RC = RC_OK;
	    	cmd->ac_RC2 = 0;
	  	}
    }
  }
#endif
}

/* ARexxFunc_SavePicture(): Handle ARexx-Command 'SAVE_PICTURE'.   Template: "PATH,FORCE/S" */
void ARexxFunc_SavePicture (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR path = (STRPTR) cmd->ac_ArgList[0];	/* internal only */
  int16 force = (uint32) (cmd->ac_ArgList[1]);
  BPTR lock = NULL;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003; /* no memory available */
  	
	if (!(path))
    {
      	if (! (FileRequest (MYILBM.win, CATSTR (TXT_SavePictureTitle), "Pictures", PICTURES_DRAWER, TRUE)))
		{
	  		cmd->ac_RC = RC_WARN;
	  		cmd->ac_RC2 = 0;
	  		return;	/* request cancelled */
		}
    }
  	
	else Strlcpy (MYPATH, path, sizeof (MYPATH));

  	if (force == FALSE)
    {
      	if (lock = Lock (MYPATH, ACCESS_READ))
		{
	  		int16 overwrite = FALSE;
	  		UnLock (lock);
#ifdef FM_REACT_SUPPORT
	  		overwrite = Do_OverwriteRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
		{
			struct EasyStruct my_req = { sizeof (struct EasyStruct), 0, CATSTR (TITLE_OverwriteReq), CATSTR (OverWr_TXT_Question), CATSTR (TXT_YesNo) };
	    	overwrite = EasyRequest (MYILBM.win, &my_req, 0);
		}
#endif /* FM_REACT_SUPPORT */
			if (overwrite == FALSE)
			{
				DisplayBeep (NULL);
	    		cmd->ac_RC = RC_WARN;
	    		cmd->ac_RC2 = 0;
	    		return;
			} /* request cancelled */
		}
    }

  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  	
    LSFMChunk->LeftEdge = MANDChunk->LeftEdge;
    LSFMChunk->TopEdge = MANDChunk->TopEdge;
    LSFMChunk->Width = MANDChunk->Width;
    LSFMChunk->Height = MANDChunk->Height;
    LSFMChunk->Iterations = MANDChunk->Iterations;
    LSFMChunk->PixelFormat = MANDChunk->PixelFormat;
    LSFMChunk->Modulo = MANDChunk->Modulo;
    LSFMChunk->Flags = MANDChunk->Flags;
    LSFMChunk->Power = MANDChunk->Power;
    LSFMChunk->PrecisionDigits = MANDChunk->PrecisionDigits;
    LSFMChunk->Depth = MANDChunk->Depth;
    LSFMChunk->ModeID = MANDChunk->ModeID;
    LSFMChunk->PrecisionBits = MANDChunk->PrecisionBits;

    LSFMChunk->RMin = MANDChunk->RMin;
    LSFMChunk->IMin = MANDChunk->IMin;
    LSFMChunk->RMax = MANDChunk->RMax;
    LSFMChunk->IMax = MANDChunk->IMax;
    LSFMChunk->JKre = MANDChunk->JKre;
    LSFMChunk->JKim = MANDChunk->JKim;

    gmp_snprintf (LSFMChunk->GRMinSTR, MANDChunk->PrecisionDigits, "%.1235Ff", MANDChunk->GRMin);
    gmp_snprintf (LSFMChunk->GRMaxSTR, MANDChunk->PrecisionDigits, "%.1235Ff", MANDChunk->GRMax);
    gmp_snprintf (LSFMChunk->GIMinSTR, MANDChunk->PrecisionDigits, "%.1235Ff", MANDChunk->GIMin);
    gmp_snprintf (LSFMChunk->GIMaxSTR, MANDChunk->PrecisionDigits, "%.1235Ff", MANDChunk->GIMax);
    gmp_snprintf (LSFMChunk->GJKreSTR, MANDChunk->PrecisionDigits, "%.1235Ff", MANDChunk->GJKre);
    gmp_snprintf (LSFMChunk->GJKimSTR, MANDChunk->PrecisionDigits, "%.1235Ff", MANDChunk->GJKim);	

    if (ZMASK & MASK)
	{
  		DrawBorder (MYILBM.wrp, &MYBORDER, 0, 0);
  		MASK &= ~ZMASK;
	}
    	
	ShowTitle (MYILBM.scr, FALSE);
	GetRGB32 (MYILBM.vp->ColorMap, 0, MYILBM.vp->ColorMap->Count, (PALETTE + 1L));
   	if (SaveMandPic (&MYILBM, LSFMChunk, USERNAME_STRING, COPYRIGHT_STRING, MYPATH)) DisplayError (MYILBM.win, TXT_ERR_SaveMandPic, 5L);
   	if (TMASK & MASK) ShowTitle (MYILBM.scr, TRUE);      	  
  
  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_LoadPalette(): Handle ARexx-Command 'LOAD_PALETTE'.   Template: "PATH,FAST/S" */
void ARexxFunc_LoadPalette (REG (a0, struct ARexxCmd *cmd),
		       REG (a1, struct RexxMsg *rm))
{
  STRPTR path = (STRPTR) cmd->ac_ArgList[0];	/* internal only */
  int16 fast = (uint32) (cmd->ac_ArgList[1]);

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003; /* no memory available */
  	
	if (!(path))
    {
      	if (! (FileRequest (MYILBM.win, CATSTR (TXT_LoadPaletteTitle), "Palettes", PALETTES_DRAWER,FALSE)))
		{
	  		cmd->ac_RC = RC_WARN;
	  		cmd->ac_RC2 = 0;
	  		return;
		}
    }
  	
	else Strlcpy (MYPATH, path, sizeof (MYPATH));

  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  	if (MYILBM.ParseInfo.iff = AllocIFF ())
    {
      	if (LoadPalette (&MYILBM, MYPATH)) DisplayError (MYILBM.win, TXT_ERR_LoadMandPal, 5L);
      	GetRGB32 (MYILBM.vp->ColorMap, 0L, (uint32) MYILBM.vp->ColorMap->Count,	PALETTE + 1L);
      	FreeIFF (MYILBM.ParseInfo.iff);
    }
  	
	else DisplayError (MYILBM.win, TXT_ERR_NoMem, 5L);

  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_SavePalette(): Handle ARexx-Command 'SAVE_PALETTE'.   Template: "PATH,FORCE/S" */
void ARexxFunc_SavePalette (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR path = (STRPTR) cmd->ac_ArgList[0];	/* internal only */
  int16 force = (uint32) (cmd->ac_ArgList[1]);
  BPTR lock = NULL;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003; /* no memory available */
  
  	if (!(path))
    {
      	if (! (FileRequest (MYILBM.win, CATSTR (TXT_SavePaletteTitle), "Palettes", PALETTES_DRAWER, TRUE)))
		{
	  		cmd->ac_RC = RC_WARN;
	  		cmd->ac_RC2 = 0;
	  		return;	/* request cancelled */
		}
    }
  	
	else Strlcpy (MYPATH, path, sizeof (MYPATH));

  	if (force == FALSE)
    {
      	if (lock = Lock (MYPATH, ACCESS_READ))
		{
	  		int16 overwrite = FALSE;
	  		UnLock (lock);
#ifdef FM_REACT_SUPPORT
	  		overwrite = Do_OverwriteRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
	  		{
	    		struct EasyStruct my_req = { sizeof (struct EasyStruct), 0, CATSTR (TITLE_OverwriteReq),
												CATSTR (OverWr_TXT_Question), CATSTR (TXT_YesNo) };
	    		overwrite = EasyRequest (MYILBM.win, &my_req, 0);
	  		}
#endif /* FM_REACT_SUPPORT */
	  		if (overwrite == FALSE)
	    	{
	      		cmd->ac_RC = RC_WARN;
	      		cmd->ac_RC2 = 0;
	      		return;
	    	} /* request cancelled */
		}
    }

  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  
  	if (MYILBM.ParseInfo.iff = AllocIFF ())
    {
      if (SavePalette (&MYILBM, &USERNAME_CHUNK, &COPYRIGHT_CHUNK, MYPATH))
			DisplayError (MYILBM.win, TXT_ERR_SaveMandPal, 5L);
      FreeIFF (MYILBM.ParseInfo.iff);
    }
  
  	else DisplayError (MYILBM.win, TXT_ERR_NoMem, 5L);

  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_Print(): Handle ARexx-Command 'PRINT'.   Template: "FORCE/S" */
void ARexxFunc_Print (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int16 force = (uint32) (cmd->ac_ArgList[0]);
  struct FM_RxCmd_PRINT *args = NULL;

#if 0 /* external */
  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003; /* no memory available */
  	if (args = (struct FM_RxCmd_PRINT *) AllocVec (sizeof (struct FM_RxCmd_PRINT),
					 								MEMF_ANY | MEMF_CLEAR))
    {
      	args->FMRC_PRINT_Force = (cmd->ac_ArgList[0]) ? TRUE : FALSE;
      	if (AddARexxEvent (DUMP_MSG, (APTR) args))
		{
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
      	
		else FreeVec (args);
    }
#else /* internal */
  	ModifyIDCMP (MYILBM.win, NULL);
  	ClearMenuStrip (MYILBM.win);
  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  	
	if (!(force))
    {
#ifdef FM_REACT_SUPPORT
      	res = Do_PrintRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
      	res = Choice (MYILBM.win, CATSTR (TITLE_PrintReq), CATSTR (Prt_TXT_AreYouSure));
#endif /* FM_REACT_SUPPORT */
    }
  	
	else res = TRUE;
  	
	if (res) WinDump (MYILBM.win);
  	else
    {
      	cmd->ac_RC = RC_WARN;
      	cmd->ac_RC2 = 0;
    } /* user-abort */
  
  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
  	ResetMenuStrip (MYILBM.win, MAINMENU);
  	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
#endif
}

/* ARexxFunc_Quit(): Handle ARexx-Command 'QUIT'.    Template: "FORCE/S,FAST/S" */
void ARexxFunc_Quit (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int16 force = (uint32) (cmd->ac_ArgList[0]);
  int16 fast = (uint32) (cmd->ac_ArgList[1]);
  struct FM_RxCmd_QUIT *args = NULL;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003; /* no memory available */
  	if (args = (struct FM_RxCmd_QUIT *) AllocVec (sizeof (struct FM_RxCmd_QUIT),
					 								MEMF_ANY | MEMF_CLEAR))
    {
      	args->FMRC_QUIT_Force = force ? TRUE : FALSE;
      	args->FMRC_QUIT_Fast = fast ? TRUE : FALSE;
      	
		if (AddARexxEvent (EXIT_MSG, (APTR) args))
		{
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
      	
		else FreeVec (args);
    }
}

/****   OPTIONS   ****/
/* ARexxFunc_ShowTitle(): Handle ARexx-Command 'SHOW_TITLE'.  Template: "int16EAN/A/N" */
void ARexxFunc_ShowTitle (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int16 show = *(int32 *) (cmd->ac_ArgList[0]);

  	if (!(show)) MASK &= ~TMASK;
  	else MASK |= TMASK;
  	MYILBM.TBState = TMASK & MASK;
	//  ShowTitle(MYILBM.scr, (int32) MYILBM.TBState);
  	ShowTitle (MYILBM.scr, (int32) show);
  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_ShowElapsed(): Handle ARexx-Command 'SHOW_ELAPSED'.  Template: - */
void ARexxFunc_ShowElapsed (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  	if (!(TMASK & MASK))  ShowTitle (MYILBM.scr, TRUE);
  	ShowTime (MYILBM.win, CATSTR (TXT_LastCalcTime), ELAPSEDTIME, FALSE);
  	Delay (TWOSECS);
  	
	if (!(TMASK & MASK))
   	{
      	ShowTitle (MYILBM.scr, FALSE);
    }
  
  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_SetIterations(): Handle ARexx-Command 'SET_ITERATIONS'.  Template: "NUMBER/N" */
void ARexxFunc_SetIterations (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int32 iterations = *(int32 *) (cmd->ac_ArgList[0]);
  uint16 i;
  int16 def = FALSE;
  
  	CONST int16 iter_item[9][2] = { {256, 0}, {320, 1}, {512, 2}, {1024, 3}, {2048, 4}, {4096, 5},
  									{8192, 6}, {16384, 7}, {32768, 8} };

  	if (!(iterations))
    {
      	ModifyIDCMP (MYILBM.win, NULL);
      	ClearMenuStrip (MYILBM.win);

      	while (1L)
		{
#ifdef FM_REACT_SUPPORT
			MANDChunk->Iterations = Do_IterationsRequest (MYILBM.win, FMSCREENNAME, 0, 0, MANDChunk->Iterations);
#else /* FM_REACT_SUPPORT */
	  		MANDChunk->Iterations = IntegerGad (MYILBM.win, CATSTR (TITLE_IterationsReq),
												CATSTR (TXT_ScrTitle_It), CATSTR (It_TXT_Iterations), MANDChunk->Iterations);
#endif /* FM_REACT_SUPPORT */
	  		if ((MANDChunk->Iterations >= MIN_ITERATIONS) && (MANDChunk->Iterations <= MAX_ALLOWED_ITERATIONS)) break;  		
			DisplayBeep (MYILBM.scr);
		}
		
      	ResetMenuStrip (MYILBM.win, MAINMENU);
      	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
    }

  	else
    {
      	if ((iterations < (MIN_ITERATIONS)) || (iterations > (MAX_ALLOWED_ITERATIONS)))
		{
	  		cmd->ac_RC = RC_ERROR;
	  		cmd->ac_RC2 = ERR10_018;
	  		return;	/* invalid argument to function */
		}
      	
		MANDChunk->Iterations = iterations;
    }

  	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 10)))->Flags &= ~CHECKED;
  	for (i = 0; i < 9; i++)
    {
      	if (MANDChunk->Iterations == iter_item[i][0])
		{
	  		((struct MenuItem *) ItemAddress (MAINMENU,	FULLMENUNUM (1, 2, iter_item[i][1])))->Flags |= CHECKED;
	  		def = TRUE;
		}
      	
		else ((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 2, iter_item[i][1])))->Flags &= ~CHECKED;
    }
  
  	if (def == FALSE) ((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 10)))->Flags |= CHECKED;

  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_SetPower(): Handle ARexx-Command 'SET_POWER'.  Template: "INDEX/A/N" */
void ARexxFunc_SetPower (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
	/* power must be 2^n where 1<n<11 */

  int32 Power = *(int32 *) (cmd->ac_ArgList[0]);
  uint16 i, j;

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018; /* invalid argument to function */

  	for (i = 1; i < 12; i++)
    {
      	if (pow (2.0, (double) i) == (double) Power)
		{
	  		for (j = 0; j < 11; j++)
	    		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 3, j)))->Flags &= ~CHECKED;
	  		
			MANDChunk->Power = i;
	  		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 3, i - 1)))->Flags |= CHECKED;
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
	  		break;
		}
    }
}

/* ARexxFunc_SetPriority(): Handle ARexx-Command 'SET_PRIORITY'.  Template: "PRI/A" */
void ARexxFunc_SetPriority (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR pristr = (STRPTR) cmd->ac_ArgList[0];
  int32 pri, p;
  CONST int16 pri_item[11][2] = { {-5, 0}, {-4, 1}, {-3, 2}, {-2, 3}, {-1, 4}, {0, 6}, {1, 8}, {2, 9}, {3, 10}, {4, 11}, {5, 12}};

  cmd->ac_RC = RC_ERROR;
  cmd->ac_RC2 = ERR10_018; /* invalid argument to function */

  	if (!(IsNumber (pristr))) return;

  	pri = atol (pristr);
  	if (errno != ERANGE)
    {
      	if ((pri <= 5L) && (pri >= -5L))
		{
	  		PRIORITY = pri;

	  		for (p = 0; p < 11; p++)
	    	{
	      		if (pri == pri_item[p][0])
					((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 4, pri_item[p][1])))->Flags |= CHECKED;
	      		else
					((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 4, pri_item[p][1])))->Flags &= ~CHECKED;
	    	}
	  
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
    }
}

/* ARexxFunc_SetColorRemap(): Handle ARexx-Command 'SET_COLOR_REMAP'.  Template: "INDEX/A/N" */
void ARexxFunc_SetColorRemap (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int32 Index = *(int32 *) (cmd->ac_ArgList[0]);
  int s;
  CONST uint32 bit_item[MAX_COLOR_REMAP][2] = { {LINEAR_BIT, 0}, {LOG_BIT, 1}, {REPEATED_BIT, 2}, {SQUARE_BIT, 3}, {ONE_BIT, 4}, {TWO_BIT, 5}, {THREE_BIT, 6}, {FOUR_BIT, 7} };

  cmd->ac_RC = RC_ERROR;
  cmd->ac_RC2 = ERR10_018; /* invalid argument to function */

  	if ((Index >= MIN_COLOR_REMAP) && (Index <= MAX_COLOR_REMAP))
    {
      	for (s = MIN_COLOR_REMAP - 1; s < MAX_COLOR_REMAP; s++)
		{
	  		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, s)))->Flags &= ~CHECKED;
	  		MANDChunk->Flags &= ~bit_item[s][0];
		}

      	MANDChunk->Flags |= bit_item[Index - 1][0];
      	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, bit_item[Index - 1][1])))->Flags |= CHECKED;

      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_SetFractalType(): Handle ARexx-Command 'SET_FRACTAL_TYPE'.  Template: "INDEX/A/N" */
void ARexxFunc_SetFractalType (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int32 Index = *(int32 *) (cmd->ac_ArgList[0]);

  	switch (Index)
    {
    	case 1:
      		MANDChunk->Flags &= ~MANDEL_BIT;
      		MANDChunk->Flags |= JULIA_BIT;
      		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 0)))->Flags |= CHECKED;
      		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 1)))->Flags &= ~CHECKED;
      	break;
    	case 2:
      		MANDChunk->Flags &= ~JULIA_BIT;
      		MANDChunk->Flags |= MANDEL_BIT;
      		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 0)))->Flags &= ~CHECKED;
      		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 1)))->Flags |= CHECKED;
      	break;
    	default:
      		cmd->ac_RC = RC_ERROR;
      		cmd->ac_RC2 = ERR10_018; /* invalid argument to function */
      		return;
    }

  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_SetCPUType(): Handle ARexx-Command 'SET_CPU_TYPE'.  Template: "INDEX/A/N" */
void ARexxFunc_SetCPUType (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int32 Index = *(int32 *) (cmd->ac_ArgList[0]);

  	switch (Index)
    {
    	case 1:
      		MANDChunk->Flags &= ~PPC_BIT;
      		MANDChunk->Flags |= MC68K_BIT;
      		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 0)))->Flags |= CHECKED;
      		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 1)))->Flags &= ~CHECKED;
      	break;
   		case 2:
      		MANDChunk->Flags &= ~MC68K_BIT;
      		MANDChunk->Flags |= PPC_BIT;
	  		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 0)))->Flags &= ~CHECKED;
	  		((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 1)))->Flags |= CHECKED;
		break;
    	default:
      		cmd->ac_RC = RC_ERROR;
      		cmd->ac_RC2 = ERR10_018; /* invalid argument to function */
    }

  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_ShowPreview(): Handle ARexx-Command 'SHOW_PREVIEW'.  Template: - */
void ARexxFunc_ShowPreview (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  	if (NewCoords (MYILBM.win, ZOOMLINE[6], ZOOMLINE[3], ZOOMLINE[4], ZOOMLINE[5]))
    {
      	ModifyIDCMP (MYILBM.win, NULL);
      	ClearMenuStrip (MYILBM.win);
      	Preview (MYILBM.win, PIXELVECTOR, ARGBMEM, RGBMEM, RNDMEM, PIXMEM, GFXMEM, MYILBM.win->GZZWidth, MYILBM.win->GZZHeight);
      	ResetMenuStrip (MYILBM.win, MAINMENU);
      	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
      	RestoreCoords (MYILBM.win);
    }
}

/* ARexxFunc_Render(): Handle ARexx-Command 'RENDER'.   Template: "FORCE/S" */
void ARexxFunc_Render (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
int16 force = (uint32) (cmd->ac_ArgList[0]);
struct FM_RxCmd_RENDER *args = NULL;

	if (force == FALSE)
    {
#ifdef FM_REACT_SUPPORT
      	res = Do_RenderRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
      	res = Choice (MYILBM.win, CATSTR (TITLE_RenderReq),	CATSTR (NewScr_TXT_Question));
#endif /* FM_REACT_SUPPORT */
		if (res == FALSE)
		{
	  		cmd->ac_RC = RC_WARN;
	  		cmd->ac_RC2 = 0;
	  		return;
		}
    }

#if 0 /* external */
  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012; /* error return from function */
  	if (args = (struct FM_RxCmd_RENDER *) AllocVec (sizeof (struct FM_RxCmd_RENDER), MEMF_ANY | MEMF_CLEAR))
    {
      	args->FMRC_RENDER_Force = (cmd->ac_ArgList[0]) ? TRUE : FALSE;

      	if (AddARexxEvent (REDRAW_MSG, (APTR) args))
		{
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
      	
		else FreeVec (args);
    }
#else /* internal */
  	if (ZMASK & MASK)
    {
      	DrawBorder (MYILBM.wrp, &MYBORDER, 0, 0);
      	MASK &= ~ZMASK;
    }
	/* already present in set_coordinates
    mpf_set_d (MANDChunk->GRMin, MANDChunk->RMin);
    mpf_set_d (MANDChunk->GRMax, MANDChunk->RMax);
    mpf_set_d (MANDChunk->GIMin, MANDChunk->IMin);
	mpf_set_d (MANDChunk->GIMax, MANDChunk->IMax);													
	mpf_set_d (MANDChunk->GJKre, MANDChunk->JKre);
	mpf_set_d (MANDChunk->GJKim, MANDChunk->JKim);
	*/  
  	SetMenuStop (&MYILBM);
  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  	ELAPSEDTIME = DrawFractal (MANDChunk, MYILBM.win, ARGBMEM, RGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, FALSE);
  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
  	SetMenuStart (&MYILBM, UNDOCOUNTER);
  	ShowTime (MYILBM.win, CATSTR (TXT_RecalculateTime), ELAPSEDTIME, FALSE);

  	if (rm->rm_Action & RXFF_RESULT)
    {
      char string[50];
      STRPTR ResultString;

        SNPrintf ((STRPTR) &string, sizeof (string), "%ld", ELAPSEDTIME);
		if (ResultString = CreateArgstring ((STRPTR) &string, Strlen ((const STRPTR) &string)))
		{
	  		cmd->ac_Result = ResultString;
		}
    }
#endif
}

/* ARexxFunc_Undo(): Handle ARexx-Command 'UNDO'.  Template: - */
void ARexxFunc_Undo (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  	RestoreCoords (MYILBM.win);
}

/* ARexxFunc_Zoom(): Handle ARexx-Command 'ZOOM'.   Template: "LEFT/A/N,TOP/A/N,WIDTH/A/N,HEIGHT/A/N,REPEATS/N,FAST/S,STEPS/N" */
void ARexxFunc_Zoom (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_003; /* no memory available  */

	if (!(ZMASK & MASK)) return;

#if 0 /* external */
	{
    	struct FM_RxCmd_ZOOM *args = NULL;

    	if (args = (struct FM_RxCmd_ZOOM *) AllocVec (sizeof (struct FM_RxCmd_ZOOM), MEMF_ANY | MEMF_CLEAR))
      	{	/* Copy arguments */
			args->FMRC_ZOOM_Left = *(int32 *) (cmd->ac_ArgList[0]);
			args->FMRC_ZOOM_Top = *(int32 *) (cmd->ac_ArgList[1]);
			args->FMRC_ZOOM_Width = *(int32 *) (cmd->ac_ArgList[2]);
			args->FMRC_ZOOM_Height = *(int32 *) (cmd->ac_ArgList[3]);
			args->FMRC_ZOOM_Repeats =
			cmd->ac_ArgList[4] ? *(int32 *) (cmd->ac_ArgList[4]) : 1;
			args->FMRC_ZOOM_Fast = cmd->ac_ArgList[5] ? (uint32) (cmd->ac_ArgList[5]) : FALSE;
			args->FMRC_ZOOM_Steps = cmd->ac_ArgList[6] ? *(int32 *) (cmd->ac_ArgList[6]) : 1; /* not supported yet */
			if (AddARexxEvent (DRAW_MSG, (APTR) args))
	  		{
	    		cmd->ac_RC = RC_OK;
	    		cmd->ac_RC2 = 0;
	    		return;
	  		}
			else FreeVec (args);
      	}
  	}
#else /* internal */
  	{
    	int32 left = *(int32 *) (cmd->ac_ArgList[0]);
		int32 top =  *(int32 *) (cmd->ac_ArgList[1]);
    	int32 right = left + (*(int32 *) (cmd->ac_ArgList[2])) - 1;
    	int32 bottom = top + (*(int32 *) (cmd->ac_ArgList[3])) - 1;
    	int32 repeats = 1, steps = 1, elapsed = 0;
    	int16 fast = (uint32) (cmd->ac_ArgList[5]);
    	int i;
    	struct Screen *scr = NULL;
    	int16 scrwidth = 0, scrheight = 0;

    	if (cmd->ac_ArgList[4]) repeats = *(int32 *) (cmd->ac_ArgList[4]);
    	if (cmd->ac_ArgList[6]) steps = *(int32 *) (cmd->ac_ArgList[6]); /* not supported yet */

    	if (scr = LockPubScreen (FMSCREENNAME))
      	{
			scrwidth = scr->Width;
			scrheight = scr->Height;
			UnlockPubScreen (FMSCREENNAME, scr);
      	}
    	else
      	{
			cmd->ac_RC2 = ERR10_012;
			return;
      	}	/* error return from function */

    	/* Check parameters */
    	if ((left < 0) || (top < 0) || (right > (scrwidth - 1)) || (bottom > (scrheight - 1)) || (repeats < 0))
      	{
			cmd->ac_RC = RC_ERROR;
			cmd->ac_RC2 = ERR10_018;
			return;	/* invalid argument to function */
      	}

    	if (fast == FALSE)
      	{
			for (i = 0; i < repeats; i++)
	  		{
	    		ZOOMLINE[6] = left;
	    		ZOOMLINE[3] = top;
	    		ZOOMLINE[4] = right;
	    		ZOOMLINE[5] = bottom;
	    		
				if (ZMASK & MASK)
	      		{
					DrawBorder (MYILBM.wrp, &MYBORDER, 0, 0);
					MASK &= ~ZMASK;
	      		}
				
	    		if (NewCoords (MYILBM.win, ZOOMLINE[6], ZOOMLINE[3], ZOOMLINE[4], ZOOMLINE[5]))
	      		{ 
					MYBITMAP = CopyBitMap (MYILBM.win, (uint16) ZOOMLINE[6], (uint16) ZOOMLINE[3], (uint16) (ZOOMLINE[4] - ZOOMLINE[6] + 1), (uint16) (ZOOMLINE[5] - ZOOMLINE[3] + 1));
					PasteBitMap (MYBITMAP, MYILBM.win, (uint16) MYILBM.win->LeftEdge, (uint16) MYILBM.win->TopEdge, (uint16) (ZOOMLINE[4] - ZOOMLINE[6] + 1), (uint16) (ZOOMLINE[5] - ZOOMLINE[3] + 1));
					SetMenuStop (&MYILBM);
					PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
					elapsed += ELAPSEDTIME =  DrawFractal (MANDChunk, MYILBM.win, ARGBMEM, RGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, FALSE);
					PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
					SetMenuStart (&MYILBM, UNDOCOUNTER);
      			}
	  		}
      	}
    	else
      	{
			SetMenuStop (&MYILBM);
			for (i = 0; i < repeats; i++)
	  		{
	    		ZOOMLINE[6] = left;
	    		ZOOMLINE[3] = top;
	    		ZOOMLINE[4] = right;
	    		ZOOMLINE[5] = bottom;
	    		if (NewCoords (MYILBM.win, ZOOMLINE[6], ZOOMLINE[3], ZOOMLINE[4], ZOOMLINE[5]))
	      		{
					elapsed += DrawFractal (MANDChunk, MYILBM.win, ARGBMEM, RGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, FALSE);
	      		}
	  		}
			
			SetMenuStart (&MYILBM, UNDOCOUNTER);
      	}

    	ELAPSEDTIME = elapsed;
    	ShowTime (MYILBM.win, CATSTR (TXT_ZoomTime), ELAPSEDTIME, FALSE);
    	cmd->ac_RC = RC_OK;
    	cmd->ac_RC2 = 0;
    	if (rm->rm_Action & RXFF_RESULT)
      	{
			char string[50];
			STRPTR ResultString;
			
			SNPrintf ((STRPTR) &string, sizeof (string), "%ld", elapsed);			
			if (ResultString = CreateArgstring ((STRPTR) &string, Strlen ((const STRPTR) &string)))
	  		{
	    		cmd->ac_Result = ResultString;
	  		}
      	}
  	}
#endif
}

/****   DISPLAY   ****/
/* ARexxFunc_DoColorcycling(): Handle ARexx-Command 'DO_COLORCYCLING'.  Template: "FORWARD/S,BACKWARD/S" */
void ARexxFunc_DoColorcycling (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int16 forward = (uint32) (cmd->ac_ArgList[0]);
  int16 backward = (uint32) (cmd->ac_ArgList[1]);
  int16 flag = SHIFTLEFT;

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018; /* invalid argument to function */

  	if ((forward && backward) || (!forward && !backward)) return;
  	if (forward) flag = SHIFTRIGHT;

  	ModifyIDCMP (MYILBM.win, IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY | IDCMP_MENUPICK);
  	if (TMASK & MASK) ShowTitle (MYILBM.win->WScreen, FALSE);
  	ClearMenuStrip (MYILBM.win);
  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  	Cycle (MYILBM.win, DELAY, flag);
  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
  	ResetMenuStrip (MYILBM.win, MAINMENU);
  	if (TMASK & MASK) ShowTitle (MYILBM.win->WScreen, TRUE);	
  	LoadRGB32 (MYILBM.vp, PALETTE);	
  	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;
}

/* ARexxFunc_SetDelay(): Handle ARexx-Command 'SET_DELAY'.  Template: "SECONDS/N" */
void ARexxFunc_SetDelay (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  int32 tmp_DELAY = *(int32 *) cmd->ac_ArgList[0];

  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;

  	if (!(tmp_DELAY))
    {
      	while (1L)
		{
#ifdef FM_REACT_SUPPORT
	  		tmp_DELAY = Do_CycleDelayRequest (MYILBM.win, FMSCREENNAME, 0, 0, DELAY);
#else /* FM_REACT_SUPPORT */
	  		tmp_DELAY = IntegerGad (MYILBM.win, CATSTR (TITLE_CycleDelayReq),
									CATSTR (TXT_ScrTitle_Cyc), CATSTR (Cyc_TXT_DelayTime), DELAY);
#endif /* FM_REACT_SUPPORT */
	  		if ((tmp_DELAY >= 0L) && (tmp_DELAY <= 200))
	    	{
	      		DELAY = tmp_DELAY;
	      		return;
	    	}
		}
    }

  	else
    {
      	if ((tmp_DELAY >= 0L) && (tmp_DELAY <= 200))
		{
	  		DELAY = tmp_DELAY;
	  		return;
		}
    }

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012; /* error return from function */
  	DisplayBeep (MYILBM.scr);
}

/* ARexxFunc_StartTimer(): Handle ARexx-Command 'START_TIMER'.  Template: -  */
struct TimeVal timer1; /* global vars */
int16 timer_started = FALSE;

void ARexxFunc_StartTimer (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  	GetSysTime (&timer1);
  	timer_started = TRUE;
}

/* ARexxFunc_StopTimer(): Handle ARexx-Command 'STOP_TIMER'.  Template: -  */
void ARexxFunc_StopTimer (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  struct TimeVal timer2 = { 0 };

  	cmd->ac_RC = RC_OK;
  	cmd->ac_RC2 = 0;

  	if (timer_started == TRUE)
    {
      	GetSysTime (&timer2);
      	SubTime (&timer2, &timer1);
      	timer_started = FALSE;

      	if (rm->rm_Action & RXFF_RESULT)
		{
	  		char string[100];
	  		STRPTR ResultString;

			SNPrintf ((STRPTR) &string, sizeof (string), "%ld.%07ld", timer2.Seconds, timer2.Microseconds);
		  	if (ResultString = CreateArgstring ((STRPTR) &string, Strlen ((const STRPTR) &string)))
	    	{
	      		cmd->ac_Result = ResultString;
	    	}
		}
    }

  	else
    {
      cmd->ac_RC = RC_WARN;
      cmd->ac_RC2 = 0; /* timer not started */
      DisplayBeep (MYILBM.scr);
    }
}

/* ARexxFunc_PlaySound(): Handle ARexx-Command 'PLAY_SOUND'.  Template: "PATH/A,ASYNC/S,REPEAT/S" */
Object *SoundObject = NULL;

void ARexxFunc_PlaySound (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR filename = (STRPTR) cmd->ac_ArgList[0];
  int16 async = (uint32) (cmd->ac_ArgList[1]);
  int16 repeat = (uint32) (cmd->ac_ArgList[2]);

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012; /* error return from function */

  	if (async == FALSE) repeat = FALSE; 	/* repeat only in async-mode */
  	if ((async == FALSE) && (DataTypesBase->lib_Version < 40)) return; /* Need V40 for sync, because of SDTA_SignalTask ... */
  	if (SoundObject) DisposeDTObject (SoundObject);	/* if async */

  	PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
  	if (SoundObject = NewDTObject (filename, DTA_SourceType, DTST_FILE,
				 					DTA_GroupID, GID_SOUND, DTA_Repeat, repeat, TAG_DONE))
    {
      	if (async == FALSE)
		{
	  		uint32 readysig = 0L, readymask = 0L;
	  		struct Task *owntask;
	  		
			if ((readysig = AllocSignal (-1)) != -1)
	    	{
	      		readymask = 1 << readysig;
	      		owntask = FindTask (NULL);
	      		SetDTAttrs (SoundObject, MYILBM.win, NULL, SDTA_SignalTask, owntask,
			  				SDTA_SignalBitMask, readymask, TAG_DONE);
	      		DoDTMethod (SoundObject, NULL, NULL, DTM_TRIGGER, NULL, STM_PLAY, NULL);
	      		Wait (readymask | SIGBREAKF_CTRL_C);
	      		DisposeDTObject (SoundObject);
	      		SoundObject = NULL;
	      		FreeSignal (readysig);
	      		cmd->ac_RC = RC_OK;
	      		cmd->ac_RC2 = 0;
	    	}
		}
      	
		else
		{
	  		DoDTMethod (SoundObject, NULL, NULL, DTM_TRIGGER, NULL, STM_PLAY, NULL);
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
    }

  	else
    {
      	cmd->ac_RC = RC_ERROR;
      	cmd->ac_RC2 = ERR10_018;
    } /* invalid argument to function */
  
  	PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
}

/* ARexxFunc_StopSound(): Handle ARexx-Command 'STOP_SOUND'.  Template: - */
void ARexxFunc_StopSound (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  	if (SoundObject)
    {
      	DisposeDTObject (SoundObject);
      	SoundObject = NULL;
    }
}

/* HandleARexxEvents(): Handle external arexx-events.

        SYNOPSIS: uint32 retcode = HandleARexxEvents
                        (
                        struct ILBMInfo  *Ilbm;
                        int32                  *Error;
                        );

        INPUTS:     Ilbm:
                            Pointer to the ILBMInfo-structure.

        RETURNS:        retcode:
                            Messagecode or 0;
*/
uint32 HandleARexxEvents (struct ILBMInfo *Ilbm)
{
  uint32 retcode = 0;
  struct FM_ARexxEvent *act_event = NULL;

  	while (IsListEmpty (&AREXXEVENTLIST) == FALSE)
    {
      	act_event = (struct FM_ARexxEvent *) AREXXEVENTLIST.lh_Head;

      	switch (act_event->FMRXC_Command)
		{

			case EXIT_MSG:
	  		{	/* EXIT_MSG - "QUIT" */
	    		struct FM_RxCmd_QUIT *args = (struct FM_RxCmd_QUIT *) act_event->FMRXC_Args;
	    		retcode = EXIT_MSG;
	    		ForceAbort = args->FMRC_QUIT_Force;				
	    		break;
	  		}

			case LOADPICTURE_MSG:
	  		{	/* LOADPICTURE_MSG - "LOAD_PICTURE" */
	    		struct FM_RxCmd_LOADPIC *msg_loadpic = (struct FM_RxCmd_LOADPIC *) act_event->FMRXC_Args;

	    		if (!(msg_loadpic->FMRC_LOADPIC_Path))
	    		{
					if (! (FileRequest (Ilbm->win, CATSTR (TXT_LoadPictureTitle), "Pictures", PICTURES_DRAWER, FALSE)))
		  			{
		    			goto LOAD_PIC_END;
		  			}
	    		}
	    
				else 
				{
					// Strlcpy (MYPATH, msg_loadpic->FMRC_LOADPIC_Path, sizeof (MYPATH));
					strcpy (MYPATH, msg_loadpic->FMRC_LOADPIC_Path);
	    		}
				
				PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
				SaveCoords (Ilbm->win);
				if (!(QueryMandFile (Ilbm, LSFMChunk, &MYPATH)))
				{
				    MANDChunk->LeftEdge = LSFMChunk->LeftEdge;
           		    MANDChunk->TopEdge = LSFMChunk->TopEdge;
            	    MANDChunk->Width = LSFMChunk->Width;
            	    MANDChunk->Height = LSFMChunk->Height;
            	    MANDChunk->Iterations = LSFMChunk->Iterations;
            	    MANDChunk->PixelFormat = LSFMChunk->PixelFormat;
            	    MANDChunk->Modulo = LSFMChunk->Modulo;
            	    MANDChunk->Flags = LSFMChunk->Flags;
            	    MANDChunk->Power = LSFMChunk->Power;
            	    MANDChunk->PrecisionDigits = LSFMChunk->PrecisionDigits;
            	    MANDChunk->Depth = LSFMChunk->Depth;
            	    MANDChunk->ModeID = LSFMChunk->ModeID;
            	    MANDChunk->PrecisionBits = LSFMChunk->PrecisionBits;
    
            	    MANDChunk->RMin = LSFMChunk->RMin;
            	    MANDChunk->IMin = LSFMChunk->IMin;
            	    MANDChunk->RMax = LSFMChunk->RMax;
            	    MANDChunk->IMax = LSFMChunk->IMax;
            	    MANDChunk->JKre = LSFMChunk->JKre;
            	    MANDChunk->JKim = LSFMChunk->JKim;
			    
				    //  copy coords from float64 values
				    /* mpf_set_d (MANDChunk->GRMax,MANDChunk->RMax);
            	    mpf_set_d (MANDChunk->GRMin,MANDChunk->RMin);
            	    mpf_set_d (MANDChunk->GIMax,MANDChunk->IMax);
            	    mpf_set_d (MANDChunk->GIMin,MANDChunk->IMin);
            	    mpf_set_d (MANDChunk->GJKre,MANDChunk->JKre);
            	    mpf_set_d (MANDChunk->GJKim,MANDChunk->JKim); */
			    
				    //  copy coords from string base 10 GMP values
            	    mpf_set_str (MANDChunk->GRMin, &LSFMChunk->GRMinSTR, 10);
            	    mpf_set_str (MANDChunk->GIMin, &LSFMChunk->GIMinSTR, 10);
            	    mpf_set_str (MANDChunk->GRMax, &LSFMChunk->GRMaxSTR, 10);
            	    mpf_set_str (MANDChunk->GIMax, &LSFMChunk->GIMaxSTR, 10);
            	    mpf_set_str (MANDChunk->GJKre, &LSFMChunk->GJKreSTR, 10);
            	    mpf_set_str (MANDChunk->GJKim, &LSFMChunk->GJKimSTR, 10);
			    
            	    Ilbm->camg = BestModeID (BIDTAG_NominalWidth, MANDChunk->Width,
                                         BIDTAG_DesiredWidth, MANDChunk->Width,
                                         BIDTAG_NominalHeight, MANDChunk->Height,
                                         BIDTAG_DesiredHeight, MANDChunk->Height,
                                         BIDTAG_Depth, MANDChunk->Depth,
                                         BIDTAG_DIPFMustNotHave, (DIPF_IS_DUALPF|DIPF_IS_PF2PRI|DIPF_IS_HAM|DIPF_IS_EXTRAHALFBRITE|DIPF_IS_PAL), 
                                         TAG_DONE);
    
            	    if (Ilbm->camg == INVALID_ID)
            	    {
                	    DisplayError (Ilbm->win, TXT_ERR_ModeNotAvailable, 20L);
                	    break;
            	    }
    
            	    Ilbm->Bmhd.w = MANDChunk->Width;
            	    Ilbm->Bmhd.h = MANDChunk->Height;
            	    Ilbm->Bmhd.nPlanes = MANDChunk->Depth;
													  		    
		    	    if (msg_loadpic->FMRC_LOADPIC_Fast == FALSE)
		    	    {
					    Fade (Ilbm->win, ARGBMEM, RGBMEM, PALETTE, 25L, 1L, TOBLACK);
		    	    }
		        
				    CloseDisplay (Ilbm);
		        
		    	    if (!MakeDisplay (Ilbm))
		    	    {
					    DisplayError (Ilbm->win, TXT_ERR_MakeDisplay, 20L);
					    retcode = EXIT_MSG;
					    break;
		    	    }
		        
				    MASK &= ~ZMASK;
		        
		    	    ShowTitle (Ilbm->scr, FALSE);
		        
				    if (LoadMandPic (Ilbm, MYPATH)) DisplayError (Ilbm->win, TXT_ERR_LoadMandPic, 5L);
		    	    if (TMASK & MASK) ShowTitle (Ilbm->scr, TRUE);
		    	    GetRGB32 (Ilbm->vp->ColorMap, 0L, (uint32) Ilbm->vp->ColorMap->Count, PALETTE + 1L);
		    	    SetMenuStart (Ilbm,UNDOCOUNTER);
			    }
		    
			    else DisplayError (Ilbm->win, TXT_ERR_QueryMandPic, 0);
		
LOAD_PIC_END:
	  			PutPointer (Ilbm->win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
	    	break;
	  		}

			case NEWDISPLAY_MSG:
	  		{	/* NEWDISPLAY_MSG - "SET_SCREENATTRS" */
	    		struct FM_RxCmd_SET_SCREENATTRS *msg_setscr = (struct FM_RxCmd_SET_SCREENATTRS *) act_event->FMRXC_Args;
	    		if (msg_setscr->FMRC_SETSCR_ModeID == INVALID_ID)
	      		{
					retcode = NEWDISPLAY_MSG;
					break;
		      	}
	    	
				else
		      	{
					Ilbm->camg = msg_setscr->FMRC_SETSCR_ModeID;
					Ilbm->Bmhd.w = msg_setscr->FMRC_SETSCR_Width;
					Ilbm->Bmhd.h = msg_setscr->FMRC_SETSCR_Height;
					Ilbm->Bmhd.nPlanes = msg_setscr->FMRC_SETSCR_Depth;
					Ilbm->ucliptype = msg_setscr->FMRC_SETSCR_OScanType;

					if (ZMASK & MASK)
			  		{
			    		DrawBorder (Ilbm->wrp, &MYBORDER, 0, 0);
			    		MASK &= ~ZMASK;
			  		}
		
					if (Ilbm->win->RPort->BitMap->Depth <= Ilbm->Bmhd.nPlanes)
			  		{
			    		MYBITMAP = CopyBitMap (Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) Ilbm->win->Width, (uint16) Ilbm->win->Height);
				  	}
		
					Fade (Ilbm->win, ARGBMEM, RGBMEM, PALETTE, 25L, 1L, TOBLACK);
					CloseDisplay (Ilbm);
					Ilbm->Bmhd.pageWidth = 0;
					Ilbm->Bmhd.pageHeight = 0;
			
					if (MakeDisplay (Ilbm))
				  	{
		    			PasteBitMap (MYBITMAP, Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) GetBitMapAttr (MYBITMAP, BMA_WIDTH),
						 				(uint16) GetBitMapAttr (MYBITMAP, BMA_HEIGHT));
#ifdef FM_REACT_SUPPORT
		    			res = Do_RenderRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
		    			res = Choice (Ilbm->win, CATSTR (TITLE_RenderReq),
			      				CATSTR (NewScr_TXT_Question));
#endif /* FM_REACT_SUPPORT */
		    			if (res)
		      			{
							SetMenuStop (Ilbm);
							PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
							ELAPSEDTIME =DrawFractal (MANDChunk, MYILBM.win, ARGBMEM, RGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, FALSE);
							PutPointer (Ilbm->win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
							SetMenuStart (Ilbm, UNDOCOUNTER);
							ShowTime (Ilbm->win, CATSTR (TXT_RenderTime), ELAPSEDTIME, FALSE);
				      	}
				  	}
		
					else DisplayError (NULL, TXT_ERR_MakeDisplay, 20L);
				}
	    
				break;
				}

			case FONTREQ_MSG:
		 	{	/* FONTREQ_MSG - "SET_FONT" */
	    		struct FM_RxCmd_SET_FONT *msg_setfnt = (struct FM_RxCmd_SET_FONT *) act_event->FMRXC_Args;
		    	if (msg_setfnt->FMRC_SETFNT_FontName)
		      	{
					MYFONTSTRUCT.ta_Name = MYFONT;
					Strlcpy (MYFONTSTRUCT.ta_Name, msg_setfnt->FMRC_SETFNT_FontName, sizeof (MYFONT));
					MYFONTSTRUCT.ta_YSize = msg_setfnt->FMRC_SETFNT_Size;
					MYFONTSTRUCT.ta_Style = msg_setfnt->FMRC_SETFNT_Style;
					MYFONTSTRUCT.ta_Flags = FPF_DISKFONT;
		      	}
	    	
				else
		      	{	
					if (!(FontRequest (Ilbm->win)))	break;
		      	}
	    
				if (ZMASK & MASK)
		      	{
					DrawBorder (Ilbm->wrp, &MYBORDER, 0, 0);
					MASK &= ~ZMASK;
		      	}
	    
				PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
		    	MYBITMAP =   CopyBitMap (Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) Ilbm->win->Width, (uint16) Ilbm->win->Height);
		    	if (msg_setfnt->FMRC_SETFNT_Fast == FALSE) Fade (Ilbm->win, ARGBMEM, RGBMEM, PALETTE, 25L, 1L, TOBLACK);
		    	CloseDisplay (Ilbm);
			
				if (!MakeDisplay (Ilbm))
		      	{
					DisplayError (Ilbm->win, TXT_ERR_MakeDisplay, 20L);
					retcode = EXIT_MSG;
					break;
		      	}
	    
				PasteBitMap (MYBITMAP, Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) Ilbm->win->Width, (uint16) Ilbm->win->Height);
		    	PutPointer (Ilbm->win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
		    	break;
		  	}
		}
    
	RemoveARexxEvent (act_event);
   	}

  	return (retcode);
}

struct ARexxCmd rexxcommands[] = {
  {"TEST", 65535, ARexxFunc_Test,
   "SWITCH/S,STRING/K,NUMBER/N,NEEDSTR/A,FOLLOW/F", 0, NULL, 0, 0, NULL},
  {"ABOUT", 2, ARexxFunc_About, NULL, 0, NULL, 0, 0, NULL},
  {"QUIT", 1, ARexxFunc_Quit, "FORCE/S,FAST/S", 0, NULL, 0, 0, NULL},
  {"SYSINFO", 3, ARexxFunc_SysInfo, NULL, 0, NULL, 0, 0, NULL},
  {"GET_COORDINATES", 4, ARexxFunc_GetCoordinates, "STEM/A", 0, NULL, 0, 0,
   NULL},
  {"SET_COORDINATES", 5, ARexxFunc_SetCoordinates, "STEM", 0, NULL, 0, 0, NULL},
  {"RENDER", 6, ARexxFunc_Render, "FORCE/S", 0, NULL, 0, 0, NULL},
  {"GET_SCREENATTRS", 7, ARexxFunc_GetScreenAttrs, "STEM/A", 0, NULL, 0, 0,
   NULL},
  {"SET_SCREENATTRS", 8, ARexxFunc_SetScreenAttrs, "STEM,FAST/S", 0, NULL, 0, 0,
   NULL},
  {"ZOOM", 9, ARexxFunc_Zoom,
   "LEFT/A/N,TOP/A/N,WIDTH/A/N,HEIGHT/A/N,REPEATS/N,STEPS/N,FAST/S", 0, NULL, 0,
   0, NULL},
  {"GET_FRAME", 10, ARexxFunc_GetFrame, "STEM/A", 0, NULL, 0, 0, NULL},
  {"SET_FRAME", 11, ARexxFunc_SetFrame, "STEM/A", 0, NULL, 0, 0, NULL},
  {"HELP", 12, ARexxFunc_Help, NULL, 0, NULL, 0, 0, NULL},
  {"LOAD_PICTURE", 13, ARexxFunc_LoadPicture, "PATH,FAST/S", 0, NULL, 0, 0,
   NULL},
  {"SAVE_PICTURE", 14, ARexxFunc_SavePicture, "PATH,FORCE/S", 0, NULL, 0, 0,
   NULL},
  {"LOAD_PALETTE", 15, ARexxFunc_LoadPalette, "PATH,FAST/S", 0, NULL, 0, 0,
   NULL},
  {"SAVE_PALETTE", 16, ARexxFunc_SavePalette, "PATH,FORCE/S", 0, NULL, 0, 0,
   NULL},
  {"PRINT", 17, ARexxFunc_Print, "FORCE/S", 0, NULL, 0, 0, NULL},
  {"SHOW_TITLE", 18, ARexxFunc_ShowTitle, "int16EAN/A/N", 0, NULL, 0, 0, NULL},
  {"SHOW_ELAPSED", 19, ARexxFunc_ShowElapsed, NULL, 0, NULL, 0, 0, NULL},
  {"SET_ITERATIONS", 20, ARexxFunc_SetIterations, "NUMBER/N", 0, NULL, 0, 0,
   NULL},
  {"GET_ITERATIONS", 21, ARexxFunc_GetIterations, "VAR/A", 0, NULL, 0, 0, NULL},
  {"SET_PRIORITY", 22, ARexxFunc_SetPriority, "PRI/A", 0, NULL, 0, 0, NULL},
  {"GET_PRIORITY", 23, ARexxFunc_GetPriority, "VAR/A", 0, NULL, 0, 0, NULL},
  {"SET_COLOR_REMAP", 24, ARexxFunc_SetColorRemap, "INDEX/A/N", 0, NULL, 0, 0,
   NULL},
  {"GET_COLOR_REMAP", 25, ARexxFunc_GetColorRemap, "VAR/A", 0, NULL, 0, 0,
   NULL},
  {"SET_FRACTAL_TYPE", 26, ARexxFunc_SetFractalType, "INDEX/A/N", 0, NULL, 0, 0,
   NULL},
  {"GET_FRACTAL_TYPE", 27, ARexxFunc_GetFractalType, "VAR/A", 0, NULL, 0, 0,
   NULL},
  {"SET_CPU_TYPE", 28, ARexxFunc_SetCPUType, "INDEX/A/N", 0, NULL, 0, 0, NULL},
  {"GET_CPU_TYPE", 29, ARexxFunc_GetCPUType, "VAR/A", 0, NULL, 0, 0, NULL},
  {"SHOW_PREVIEW", 30, ARexxFunc_ShowPreview, NULL, 0, NULL, 0, 0, NULL},
  {"UNDO", 31, ARexxFunc_Undo, NULL, 0, NULL, 0, 0, NULL},
  {"DO_COLORCYCLING", 32, ARexxFunc_DoColorcycling, "FORWARD/S,BACKWARD/S", 0,
   NULL, 0, 0, NULL},
  {"SET_DELAY", 33, ARexxFunc_SetDelay, "SECONDS/N", 0, NULL, 0, 0, NULL},
  {"GET_DELAY", 34, ARexxFunc_GetDelay, "VAR/A", 0, NULL, 0, 0, NULL},
  {"SET_FONT", 35, ARexxFunc_SetFont, "STEM,FAST/S", 0, NULL, 0, 0, NULL},
  {"GET_FONT", 36, ARexxFunc_GetFont, "STEM/A", 0, NULL, 0, 0, NULL},
  {"START_TIMER", 37, ARexxFunc_StartTimer, NULL, 0, NULL, 0, 0, NULL},
  {"STOP_TIMER", 38, ARexxFunc_StopTimer, NULL, 0, NULL, 0, 0, NULL},
  {"PLAY_SOUND", 39, ARexxFunc_PlaySound, "PATH/A,ASYNC/S,REPEAT/S", 0, NULL, 0,
   0, NULL},
  {"SET_PALETTE", 40, ARexxFunc_SetPalette, "STEM,FAST/S", 0, NULL, 0, 0, NULL},
  {"GET_PALETTE", 41, ARexxFunc_GetPalette, "STEM/A", 0, NULL, 0, 0, NULL},
  {"SET_POWER", 42, ARexxFunc_SetPower, "NUMBER/A/N", 0, NULL, 0, 0, NULL},
  {"GET_POWER", 43, ARexxFunc_GetPower, "VAR/A", 0, NULL, 0, 0, NULL},
  {"STOP_SOUND", 44, ARexxFunc_StopSound, NULL, 0, NULL, 0, 0, NULL},
  {NULL, 0, NULL, NULL, 0, NULL, 0, 0, NULL}
};

/* InitARexx(): Initialize ARexx-Support

        SYNOPSIS: int16 success = InitARexx
                        (
                        RESOURCEFILE        ResPtr;
                        );

        INPUTS:     ResPtr (reactor only):
                            Pointer to the reaction-resource.

        RETURNS:        success:
                            Is TRUE, if successfull.
*/

int16 InitARexx (void)
{
	// NewList(&AREXXEVENTLIST);
	// AREXXEVENTLIST.lh_Type = NT_USER;

  	ARexxBase = (OpenLibrary ("arexx.class", 0));
  	if (ARexxBase)
    {
#ifdef __amigaos4__
      	IAREXX = ((struct ARexxIFace *) GetInterface ((struct Library *) ARexxBase, "main", 1, NULL));
      	if (IAREXX)
		{
	  		TimerMP =  (struct MsgPort *) AllocSysObject (ASOT_PORT, (struct TagItem *) NULL);
	  		if (TimerMP)
	    	{
	      		TimerIO = (struct TimeRequest *) AllocSysObjectTags (ASOT_IOREQUEST, ASOIOR_Size, sizeof (struct TimeRequest), ASOIOR_ReplyPort, TimerMP, TAG_DONE);
	      		if (TimerIO)
				{
		  			TimerError = OpenDevice (TIMERNAME, UNIT_MICROHZ, (struct IORequest *) TimerIO, 0L);
		  			if (!TimerError)
		    		{
		      			struct Library *TimerBase = (struct Library *) TimerIO->Request.io_Device;
		      			ITimer = (struct TimerIFace *) GetInterface (TimerBase, "main", 1, NULL);
		      			if (ITimer)
						{
			  				TimerIO->Request.io_Command = TR_ADDREQUEST;	/* dummy-request */
			  				TimerIO->Time.Seconds = 1;
			  				TimerIO->Time.Microseconds = 1;
			  				DoIO ((struct IORequest *) TimerIO);

			  				AREXXOBJ = NewObject ((IAREXX->AREXX_GetClass) (), NULL,
#else /* __amigaos4__ */
      						AREXXOBJ = NewObject (AREXX_GetClass (), NULL,
#endif /* __amigaos4__ */
			    						AREXX_HostName, "FLASHMANDEL",
			    						AREXX_DefExtension, "rexx",
			    						AREXX_NoSlot, FALSE,
			    						AREXX_ErrorCode, &ARexxError,
			    						AREXX_Commands, &rexxcommands, TAG_DONE);
    					}
#ifdef __amigaos4__
  						else PutStr ("Error: no timer.device\n");
					}

					else PutStr ("Error: no timer-iorequest\n");
				}
				
				else PutStr ("Error: no timer-port\n");
			}

			else PutStr ("Error: no arexx.class-interface\n");
		}

		else PutStr ("Error: no arexx.class\n");
	}
#endif /* __amigaos4__ */

	if (AREXXOBJ)
  	{
    	GetAttr (AREXX_SigMask, AREXXOBJ, (uint32 *) & AREXXSIGNAL);
    	return (TRUE);
  	}

	switch (ARexxError)
  	{
		case RXERR_NO_COMMAND_LIST:
    	{
		      PutStr ("Can't init ARexx-Host - No commands defined.\n");
		      break;
		}
  		
		case RXERR_NO_PORT_NAME:
    	{
      		PutStr ("Can't init ARexx-Host - No portname defined.\n");
      		break;
    	}
  
  		case RXERR_PORT_ALREADY_EXISTS:
    	{
      		PutStr ("Can't init ARexx-Host - portname already exists.\n");
      		break;
    	}
  
  		case RXERR_OUT_OF_MEMORY:
    	{
      		PutStr ("Can't init ARexx-Host - Out of memory.\n");
      		break;
    	}
  
  		default:
    	{
      		PutStr ("Can't init ARexx-Host.\n");
      		break;
    	}
  	}

	FreeARexx ();
	return (FALSE);
}

/* FreeARexx(): Free ARexx-Support.

        SYNOPSIS: void = FreeARexx
                        (
                        RESOURCEFILE        ResPtr;
                        );

        INPUTS:     ResPtr (reactor only):
                            Pointer to the reaction-resource.

      
	    RETURNS:        -
*/

void FreeARexx (void)
{
  	if (AREXXOBJ)
    {
      	DisposeObject (AREXXOBJ);
      	AREXXOBJ = NULL;
    }
#ifdef __amigaos4__
  	
	if (ITimer)
    {
      	DropInterface ((struct Interface *) ITimer);
      	ITimer = NULL;
    }
	
  	if (!TimerError)
    {
      	if (!CheckIO ((struct IORequest *) TimerIO))
		{
	  		AbortIO ((struct IORequest *) TimerIO);
	  		WaitIO ((struct IORequest *) TimerIO);
		}
      	
		CloseDevice (TimerIO);
      	TimerError = -1;
    }
  	
	if (TimerIO)
    {
      	FreeSysObject (ASOT_IOREQUEST, TimerIO);
      	TimerIO = NULL;
    }
	
  	if (TimerMP)
    {
      	FreeSysObject (ASOT_PORT, TimerMP);
      	TimerMP = NULL;
    }
	
  	if (IAREXX)
    {
      	DropInterface ((struct Interface *) IAREXX);
      	IAREXX = NULL;
    }
#endif /* __amigaos4__ */
  	if (ARexxBase)
    {
      	CloseLibrary (ARexxBase);
      	ARexxBase = NULL;
    }
  
  	AREXXSIGNAL = 0;
}

#else /* FM_AREXX_SUPPORT */
	extern struct Library *ResourceBase;	/* dummy */
#endif /* FM_AREXX_SUPPORT */
