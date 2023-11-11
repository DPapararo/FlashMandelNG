/*
 *  FlashMandel - FM_SysInfoReq_React.c
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
 *    $Id: FM_SysInfoReq_React.c,v 1.3 19/02/2018 01:23:39 eschwan Exp $
 *
 *    Code for palette-prefs with reaction-support
 *
 *    $Log: FM_SysInfoReq_React.c,v $
 *    Revision 1.4  2020/12/18 00:00:00  eschwan
 *    Modified for "no reactor" code
 *    Made internal functions static
 *
 *    Revision 1.3  2018/02/19 20:56:20  dpapararo
 *    Cleanup code referred to 68k cpus and pre Amiga OS4
 *
 *    Revision 1.2  2004/07/01 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:24:39  eschwan
 *    First tracked version
 *
 *    Revision 2.1  2002/10/17 00:00:00  eschwan
 *    Last no-cvs-release.
 *
 *    Revision 2.2  2018/05/30 23:31:00  dpapararo
 *    changed AvailMem return values from bytes to mbytes and used MEMF_SHARED instead obsolete MEMF_FAST.
 */

#define NDEBUG

#include <stdlib.h>
#include <stdio.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>

#include <classes/window.h>
#include <gadgets/string.h>
#include <gadgets/layout.h>
#include <images/label.h>

#include "FM_SysInfoReq_React.h"
#include "FM_ReactionBasics.h"

#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

extern struct Catalog *CatalogPtr;

extern uint32 *PALETTE;

extern CONST_STRPTR CPUPPC_STR[];
extern CONST_STRPTR VERPPC_STR[];
extern CONST_STRPTR VECPPC_STR[];

struct ReactWinData *sysinfo_rwd = NULL;

extern struct MsgPort *IDCMPortPtr;
extern struct MsgPort *AppPortPtr;
#ifndef __amigaos4__
static Object *sysinfo_68kcpu = NULL;
static Object *sysinfo_68kfpu = NULL;
#endif
static Object *sysinfo_ppccpu = NULL;
static Object *sysinfo_ppcfpu = NULL;
static Object *sysinfo_avail_chip = NULL;
static Object *sysinfo_avail_fast = NULL;
static Object *sysinfo_vector_unit = NULL;
static Object *sysinfo_largest_chip = NULL;
static Object *sysinfo_largest_fast = NULL;

static int16 OpenSysInfoReqWindow (char *PubScreenName);
static void CloseSysInfoReqWindow (void);
static int32 HandleSysInfoReqWindow (struct Window *Win);

/*  Do_SysInfoRequest(): sysinfo-requester (full handling).

        SYNOPSIS: void = Do_SysInfoRequest
                        (
                        struct Window     *Win;
                        char                  *PubScreenName;
                        int16                    Left;
                        int16                    Top;
                        );

        INPUTS:     Win:
                            Pointer to the parent-window.

                        PubScreenName:
                            name of the public-screen, where the window should appear.

                        Left:
                            Left edge of palette-window.

                        Top:
                            Top edge of palette-window.

        RETURNS:        -
*/

void Do_SysInfoRequest (struct Window *Win, char *PubScreenName, int16 Left, int16 Top)
{
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  	if (OpenSysInfoReqWindow (PubScreenName) == TRUE)
    {
      	waitsigs = sysinfo_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      	while (result == RESULT_NOTHING)
		{
	  		mask = Wait (waitsigs);
#ifndef NDEBUG
	  		Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  		if (mask & sysinfo_rwd->RWD_WSignals)
	    	{
	      		result = HandleSysInfoReqWindow (Win);
	    	}
	  
	  		if (mask & SIGBREAKF_CTRL_C)
	    	{
	      		result = RESULT_CANCEL;
	    	}
		}

      	CloseSysInfoReqWindow ();
    }
  
  	else DisplayError (Win, TXT_ERR_Window, 5L);

  	LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
}

#ifdef FM_REACT_SUPPORT

/*  OpenSysInfoReqWindow(): open window of sysinfo-requester.

        SYNOPSIS: int16 res = OpenSysInfoReqWindow
                        (
                        char    *PubScreenName;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16 OpenSysInfoReqWindow (char *PubScreenName)
{
  uint32 chip = AvailMem (MEMF_CHIP) / 1024 / 1024;
  uint32 fast = AvailMem (MEMF_SHARED) / 1024 / 1024;
  uint32 largestchip = AvailMem (MEMF_CHIP | MEMF_LARGEST) / 1024 / 1024;
  uint32 largestfast = AvailMem (MEMF_SHARED | MEMF_LARGEST) / 1024 / 1024;

  char buffer_avail_chip[50];
  char buffer_avail_fast[50];
  char buffer_largest_chip[50];
  char buffer_largest_fast[50];

  	SNPrintf ((char *) &buffer_avail_chip, sizeof (buffer_avail_chip), "%ld MByte", chip);
  	SNPrintf ((char *) &buffer_avail_fast, sizeof (buffer_avail_fast), "%ld MByte", fast);
  	SNPrintf ((char *) &buffer_largest_chip, sizeof (buffer_largest_chip), "%ld MByte", largestchip);
  	SNPrintf ((char *) &buffer_largest_fast, sizeof (buffer_largest_fast), "%ld MByte", largestfast);

  	sysinfo_rwd =  AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  	if (sysinfo_rwd != NULL)
    {
		sysinfo_rwd->RWD_WindowObject = NewObject (NULL, "window.class", WA_Title, CATSTR (TITLE_SysInfoReq), 
WA_IDCMP, IDCMP_RAWKEY, WA_NoCareRefresh, TRUE, WA_CloseGadget, TRUE, WA_DepthGadget, TRUE, WA_SizeGadget, FALSE, 
WA_DragBar, TRUE, WA_Activate, TRUE, WA_PubScreenName, PubScreenName, WA_PubScreenFallBack, TRUE, 
WINDOW_Position, WPOS_CENTERSCREEN, WINDOW_GadgetHelp, FALSE, WINDOW_IconifyGadget, FALSE, 
WINDOW_SharedPort, IDCMPortPtr, WINDOW_AppPort, AppPortPtr, WINDOW_Layout, 
NewObject (NULL, "layout.gadget",	/* SysI_VERT_ROOT */
LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_SpaceInner,TRUE,LAYOUT_SpaceOuter,TRUE,
//LAYOUT_BevelStyle, BVS_THIN,
//LAYOUT_DeferLayout, TRUE,
LAYOUT_AddChild, NewObject (NULL, "layout.gadget", /* SysI_VERT_1 */ LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
//LAYOUT_BevelState, IDS_SELECTED,
//LAYOUT_BevelStyle, BVS_FIELD,
LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_VERT_1_1 */
LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_BevelState,IDS_NORMAL,LAYOUT_BevelStyle,BVS_GROUP,LAYOUT_Label,
CATSTR(SysI_TXT_CpuInfo),LAYOUT_LabelPlace,   BVJ_TOP_CENTER,
#ifndef __amigaos4__
LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_1 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_BevelState, IDS_NORMAL, LAYOUT_BevelStyle, BVS_GROUP, LAYOUT_Label, "Mc68k",	//CATSTR(SysI_TXT_mc68k),
LAYOUT_LabelPlace, BVJ_TOP_LEFT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_1_1 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_68kcpu
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_68kCPU,
GA_RelVerify, FALSE, GA_ReadOnly, TRUE, STRINGA_TextVal, "68000",	//CATSTR(SysI_TXT_68k_CPU),
STRINGA_MaxChars,
15,
STRINGA_MinVisible,
10,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, "CPU:",	//CATSTR(SysI_TXT_CPU),
LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_1_2 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_68kfpu
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_68kFPU,
GA_RelVerify, FALSE, GA_ReadOnly, TRUE, STRINGA_TextVal, "Builtin",	//CATSTR(SysI_TXT_68k_FPU),
STRINGA_MaxChars,
15,
STRINGA_MinVisible,
10,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, "FPU:",	//CATSTR(SysI_TXT_FPU),
LABEL_Justification,
LJ_LEFT,
TAG_DONE),
TAG_DONE),
TAG_DONE),
#endif /* !__amigaos4__ */
LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_2 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_BevelState, IDS_NORMAL, LAYOUT_BevelStyle, BVS_GROUP, LAYOUT_Label, "PowerPC",	//CATSTR(SysI_TXT_PowerPC),
LAYOUT_LabelPlace, BVJ_TOP_LEFT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_2_1 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_ppccpu
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_PPCCPU,
//GA_RelVerify, FALSE,
GA_ReadOnly, TRUE, STRINGA_TextVal, &CPUPPC_STR,	//CATSTR(SysI_TXT_PPCCPU),
STRINGA_MaxChars,
30,
STRINGA_MinVisible,
30,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (SysI_TXT_CPU_PPC), LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_2_2 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_ppcfpu
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_PPCFPU,
//GA_RelVerify, FALSE,
GA_ReadOnly, TRUE, STRINGA_TextVal, &VERPPC_STR,	//CATSTR(SysI_TXT_PPCFPU),
STRINGA_MaxChars,
5,
STRINGA_MinVisible,
5,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, "Version:",	//CATSTR(SysI_TXT_FPU_PPC),
LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_1_2_3 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_vector_unit
=
NewObject
(NULL,
"string.gadget",
GA_ReadOnly,
TRUE,
STRINGA_TextVal,
&VECPPC_STR,
STRINGA_MaxChars,
15,
STRINGA_MinVisible,
15,
//GA_TextAttr, "topaz.font/11",
TAG_DONE),
CHILD_Label,
NewObject
(NULL,
"label.image",
LABEL_Text,
"Vector Unit:",
LABEL_Justification,
LJ_LEFT,
TAG_DONE),
TAG_DONE),
TAG_DONE),
TAG_DONE),
//CHILD_MinWidth, 200,
//CHILD_MinHeight, 200,
LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_VERT_1_2 */
LAYOUT_Orientation, LAYOUT_ORIENT_VERT, LAYOUT_BevelState, IDS_NORMAL, LAYOUT_BevelStyle, BVS_GROUP, LAYOUT_Label, CATSTR (SysI_TXT_MemInfo), LAYOUT_LabelPlace, BVJ_TOP_CENTER, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_2_1 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_BevelState, IDS_NORMAL, LAYOUT_BevelStyle, BVS_GROUP, LAYOUT_Label, CATSTR (SysI_TXT_Available), LAYOUT_LabelPlace, BVJ_TOP_LEFT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_2_1_1 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_avail_chip
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_AvailChip,
GA_ReadOnly,
TRUE,
STRINGA_TextVal,
&buffer_avail_chip,
STRINGA_MaxChars,
12,
STRINGA_MinVisible,
12,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (SysI_TXT_AvailChip), LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_2_1_2 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_avail_fast
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_AvailFast,
GA_ReadOnly,
TRUE,
STRINGA_TextVal,
&buffer_avail_fast,
STRINGA_MaxChars,
12,
STRINGA_MinVisible,
12,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (SysI_TXT_AvailFast), LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_2_2 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_BevelState, IDS_NORMAL, LAYOUT_BevelStyle, BVS_GROUP, LAYOUT_Label, CATSTR (SysI_TXT_LargestFree), LAYOUT_LabelPlace, BVJ_TOP_LEFT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_2_2_1 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_largest_chip
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_LargestChip,
GA_ReadOnly,
TRUE,
STRINGA_TextVal,
&buffer_largest_chip,
STRINGA_MaxChars,
12,
STRINGA_MinVisible,
12,
//GA_TextAttr, "topaz.font/11",
TAG_DONE), CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (SysI_TXT_LargestChip), LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* SysI_HORIZ_1_2_2_2 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
sysinfo_largest_fast
=
NewObject
(NULL,
"string.gadget",
//GA_ID, SysI_STRING_LargestFast,
GA_ReadOnly,
TRUE,
STRINGA_TextVal,
&buffer_largest_fast,
STRINGA_MaxChars,
12,
STRINGA_MinVisible,
12,
//GA_TextAttr, "topaz.font/11",
TAG_DONE),
CHILD_Label,
NewObject
(NULL,
"label.image",
LABEL_Text,
CATSTR
(SysI_TXT_LargestFast),
LABEL_Justification,
LJ_LEFT,
TAG_DONE),
TAG_DONE),
TAG_DONE),
TAG_DONE),
TAG_DONE),
LAYOUT_AddChild,
NewObject
(NULL,
"layout.gadget",
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_HorizAlignment,
LALIGN_CENTER,
LAYOUT_EvenSize,
TRUE,
LAYOUT_SpaceInner,
TRUE,
LAYOUT_SpaceOuter,
TRUE,
LAYOUT_AddChild,
NewObject
(NULL,
"button.gadget",
GA_ID,
SysI_BUTTON_OK,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Work_TXT_OK),
TAG_DONE),
CHILD_WeightedHeight,
0,
CHILD_WeightedWidth,
0,
CHILD_MinWidth,
80,
TAG_DONE),
TAG_DONE),
   TAG_DONE);

		if (sysinfo_rwd->RWD_WindowObject != NULL)
		{
	  		if (DoMethod (sysinfo_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    	{
	    	  	GetAttr (WINDOW_Window, sysinfo_rwd->RWD_WindowObject, (uint32 *) & sysinfo_rwd->RWD_IWindow);
	    	  	GetAttr (WINDOW_SigMask, sysinfo_rwd->RWD_WindowObject, &sysinfo_rwd->RWD_WSignals);
	    	  	return (TRUE);
	    	}
		}
    }
  
  	return (FALSE);
}

/*  CloseSysInfoReqWindow(): close window of sysinfo-requester.

        SYNOPSIS: void = CloseSysInfoReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void CloseSysInfoReqWindow (void)
{
  	if (sysinfo_rwd != NULL)
    {
      	if (sysinfo_rwd->RWD_WindowObject)
		{
	  		DisposeObject (sysinfo_rwd->RWD_WindowObject);
		}
      	
		FreeVec (sysinfo_rwd);
      	sysinfo_rwd = NULL;
    }
}

/*  HandleSysInfoReqWindow(): Handle the messages of the sysinfo-requester.

        SYNOPSIS: int32 res = HandlePalettePrefWindow
                        (
                        struct Window   *Win;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user wants to quit without a change.
                                                        RESULT_OK      -> accept changes and quit.
*/

static int32 HandleSysInfoReqWindow (struct Window *Win)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  	while ((result = DoMethod (sysinfo_rwd->RWD_WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
    {
#ifndef NDEBUG
      	Printf ("result: %ld\n", result);
#endif /*NDEBUG*/
		switch (result & WMHI_CLASSMASK)
		{
			case WMHI_CLOSEWINDOW:
	  			rc = RESULT_CANCEL;
	  		break;

			case WMHI_GADGETUP:
	  			switch (result & RL_GADGETMASK)
	    		{
	    			case SysI_BUTTON_OK:
	      				rc = RESULT_OK;
	      			break;
#ifndef NDEBUG
	    			default:
	      				Printf ("unknown result: %lX\nunknown gadget: %lX\n", result, result & WMHI_GADGETMASK);
						// DisplayBeep (NULL);
	      			break;
#endif /* !NDEBUG */
	    		}
	  		break;

			case WMHI_ICONIFY:
	  			DoMethod (sysinfo_rwd->RWD_WindowObject, WM_ICONIFY);
	  			GetAttr (WINDOW_Window, sysinfo_rwd->RWD_WindowObject, (uint32 *) & sysinfo_rwd->RWD_IWindow);
	  		break;

			case WMHI_UNICONIFY:
	  			DoMethod (sysinfo_rwd->RWD_WindowObject, WM_OPEN);
	  			GetAttr (WINDOW_Window, sysinfo_rwd->RWD_WindowObject, (uint32 *) & sysinfo_rwd->RWD_IWindow);
	  		break;
	
			case WMHI_RAWKEY:
	  			switch (result & WMHI_KEYMASK)
	    		{
	    			case 0x45:		/* Escape */
	      				rc = RESULT_CANCEL;
	      			break;
	    			case 0x44:		/* Return */
	    			case 0x43:		/* Enter */
	      				rc = RESULT_OK;
	      			break;
	    		}
	  		break;
#ifndef NDEBUG
			default:
	  			Printf ("unknown result: %lX\nunknown class: %lX\n", result, result & WMHI_CLASSMASK);
				// DisplayBeep (NULL);
	  		break;
#endif /* !NDEBUG */
		}
    }
  
  	return (rc);
}
#endif /* FM_REACT_SUPPORT */
