/*
 *  FlashMandel - FM_InfoReq_React.c
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
 *    $Id: FM_InfoReq_React.c,v 1.3 2004/06/30 17:23:55 eschwan Exp $
 *
 *    Code for info-request with reaction-support
 *
 *    $Log: FM_InfoReq_React.c,v $
 *    Revision 1.3  2020/12/18 00:00:00  eschwan
 *    Modified for "no reactor" code
 *    Made internal functions static
 *
 *    $Log: FM_InfoReq_React.c,v $
 *    Revision 1.2  2004/06/30 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:23:55  eschwan
 *    First tracked version
 *
 *    Revision 2.1  2002/10/17 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#define NDEBUG

#ifdef __amigaos4__
#define __USE_INLINE__
#define __USE_BASETYPE__
#endif /* __amigaos4__ */

#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <intuition/screens.h>
#include <classes/window.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <images/label.h>
#include <dos/dos.h>

#ifdef __GNUC__
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/locale.h>
#include <clib/alib_protos.h>
#else /* __GNUC__ */
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/locale_protos.h>
#include <clib/alib_protos.h>

#include <pragmas/intuition_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/locale_pragmas.h>
#endif /* __GNUC__ */

#include <stdlib.h>
#include <stdio.h>

#include "FM_InfoReq_React.h"
#include "FM_ReactionBasics.h"

#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

extern struct Catalog *CatalogPtr;

extern uint32 *PALETTE;

extern struct MsgPort *IDCMPortPtr;
extern struct MsgPort *AppPortPtr;

static struct ReactWinData *info_rwd = NULL;
static struct ReactWinData *workgroup_rwd = NULL;

static int16 OpenInfoReqWindow (uint8 * PubScreenName);
static void CloseInfoReqWindow (void);
static int32 HandleInfoReqWindow (struct Window *Win, uint8 * PubScreenName);

static int16 OpenWorkgroupReqWindow (uint8 * PubScreenName);
static void CloseWorkgroupReqWindow (void);
static int32 HandleWorkgroupReqWindow (struct Window *Win);

/*  Do_InfoRequest(): info-requester (full handling).

        SYNOPSIS: void = Do_InfoRequest
                        (
                        struct Window     *Win;
                        uint8                  *PubScreenName;
                        int16                    Left;
                        int16                    Top;
                        );

        INPUTS:     Win:
                            Pointer to the parent-window.

                        PubScreenName:
                            name of the public-screen, where the window should appear.

                        Left:
                            Left edge of palette-window (not supported yet).

                        Top:
                            Top edge of palette-window (not supported yet).

        RETURNS:        -
*/

void
Do_InfoRequest (struct Window *Win, uint8 * PubScreenName, int16 Left,
		int16 Top)
{
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  	if (OpenInfoReqWindow (PubScreenName) == TRUE)
    {
      	waitsigs = info_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      	while (result == RESULT_NOTHING)
		{
	  		mask = Wait (waitsigs);
#ifndef NDEBUG
	  		Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  		if (mask & info_rwd->RWD_WSignals)
	    	{
	      		result = HandleInfoReqWindow (Win, PubScreenName);
	    	}
	  
	  		if (mask & SIGBREAKF_CTRL_C)
	    	{
	      		result = RESULT_CANCEL;
	    	}
		}

      	CloseInfoReqWindow ();
    }
  	
	else DisplayError (Win, TXT_ERR_Window, 5L);

  	LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
}

/*  OpenInfoReqWindow(): open window of info-requester.

        SYNOPSIS: int16 res = OpenInfoReqWindow
                        (
                        uint8                  *PubScreenName;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenInfoReqWindow (uint8 * PubScreenName)
{
#ifdef FM_REACT_SUPPORT
  info_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  
  	if (info_rwd != NULL)
    {
      	info_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
					      						WA_Title, CATSTR (TITLE_InfoReq),
					      						WA_IDCMP, IDCMP_RAWKEY,
					      						WA_NoCareRefresh, TRUE,
					      						WA_CloseGadget, TRUE,
					      						WA_DepthGadget, TRUE,
					      						WA_SizeGadget, FALSE,
					                            WA_DragBar, TRUE,
					                            WA_Activate, TRUE,
					                            WA_PubScreenName, PubScreenName,
					                            WA_PubScreenFallBack, TRUE,
					                            WINDOW_Position,
					                            WPOS_CENTERSCREEN,
					                            WINDOW_GadgetHelp, FALSE,
					                            WINDOW_IconifyGadget, FALSE,
					                            WINDOW_SharedPort, IDCMPortPtr,
					                            WINDOW_AppPort, AppPortPtr,
					                            WINDOW_Layout, NewObject (NULL,
						                        "layout.gadget",
									            LAYOUT_Orientation,
									            LAYOUT_ORIENT_VERT,
									            LAYOUT_SpaceInner,
									            TRUE,
									            LAYOUT_SpaceOuter,
									            TRUE,
									            LAYOUT_BevelState,
									            IDS_NORMAL,
									            LAYOUT_BevelStyle,
									            BVS_GROUP,
									            LAYOUT_AddChild,
									            NewObject
									            (NULL,
									            "layout.gadget",
									            LAYOUT_Orientation,
									            LAYOUT_ORIENT_VERT,
									            LAYOUT_BevelState,
									            IDS_SELECTED,
									            LAYOUT_BevelStyle,
									            BVS_FIELD,
									            LAYOUT_SpaceInner,
									            TRUE,
									            LAYOUT_SpaceOuter,
									            TRUE,
									            LAYOUT_VertAlignment,
									            LALIGN_CENTER,
									            LAYOUT_AddImage,
									            NewObject
									            (NULL,
									            "label.image",
									            LABEL_Text,
									            CATSTR
									            (Info_TXT_Info),
									            LABEL_Justification,
									            LJ_LEFT,
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
									            Info_BUTTON_OK,
									            GA_RelVerify,
									            TRUE,
									            GA_Text,
									            CATSTR
									            (Info_TXT_OK),
									            TAG_DONE),
									            CHILD_WeightedHeight,
									            0,
									            CHILD_WeightedWidth,
									            0,
									            LAYOUT_AddChild,
									            NewObject
									            (NULL,
									            "button.gadget",
									            GA_ID,
									            Info_BUTTON_More,
									            GA_RelVerify,
									            TRUE,
									            GA_Text,
									            CATSTR
									            (Info_TXT_More),
									            TAG_DONE),
									            CHILD_WeightedHeight,
									            0,
									            CHILD_WeightedWidth,
									            0,
									            TAG_DONE),
												TAG_DONE),
					      						TAG_DONE);

		if (info_rwd->RWD_WindowObject != NULL)
		{
	  		if (DoMethod (info_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    	{
	      		GetAttr (WINDOW_Window, info_rwd->RWD_WindowObject, (uint32 *) & info_rwd->RWD_IWindow);
	      		GetAttr (WINDOW_SigMask, info_rwd->RWD_WindowObject, &info_rwd->RWD_WSignals);
	      		return (TRUE);
	    	}
		}
    }
#endif /* FM_REACT_SUPPORT */
  	return (FALSE);
}

/*  CloseInfoReqWindow(): close window of info-requester.

        SYNOPSIS: void = CloseInfoReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void
CloseInfoReqWindow (void)
{
#ifdef FM_REACT_SUPPORT
  	if (info_rwd != NULL)
    {
      	if (info_rwd->RWD_WindowObject)
		{
	  		DisposeObject (info_rwd->RWD_WindowObject);
		}
      	
		FreeVec (info_rwd);
      	info_rwd = NULL;
    }
#endif	/* FM_REACT_SUPPORT */
}

/*  HandleInfoReqWindow(): Handle the messages of the info-requester.

        SYNOPSIS: int32 res = HandlePalettePrefWindow
                        (
                        struct Window   *Win;
                        uint8                *PubScreenName;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        PubScreenName:
                            name of the public-screen, where windows should appear.

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user wants to quit without a change.
                                                        RESULT_OK      -> accept changes and quit.
*/

static int32 HandleInfoReqWindow (struct Window *Win, uint8 * PubScreenName)
{
  int32 rc = RESULT_NOTHING;
#ifdef FM_REACT_SUPPORT
  uint32 result, code;

  	while ((result = DoMethod (info_rwd->RWD_WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
    {
#ifndef NDEBUG
    	Printf ("result: %ld\n", result);
#endif /* !NDEBUG */

      	switch (result & WMHI_CLASSMASK)
		{
			case WMHI_CLOSEWINDOW:
	  			rc = RESULT_CANCEL;
	  		break;
			case WMHI_GADGETUP:
	  			switch (result & RL_GADGETMASK)
	    		{
	    			case Info_BUTTON_OK:
	      				rc = RESULT_OK;
	      			break;
	    			case Info_BUTTON_More:
	      				SetAttrs (info_rwd->RWD_WindowObject, WA_BusyPointer, TRUE,	TAG_DONE);
	      				Do_WorkgroupRequest (info_rwd->RWD_IWindow, PubScreenName, 50, 50);
	      				SetAttrs (info_rwd->RWD_WindowObject, WA_BusyPointer, FALSE, TAG_DONE);
	      				rc = RESULT_NOTHING;
	      			break;
#ifndef NDEBUG
	    			default:
	      				Printf ("unknown result: %lX\nunknown gadget: %lX\n", result, result & WMHI_GADGETMASK);
						// DisplayBeep(NULL);
	      			break;
#endif /* !NDEBUG */
	    		}
	  		break;	
		case WMHI_ICONIFY:
	  		DoMethod (info_rwd->RWD_WindowObject, WM_ICONIFY);
	  		GetAttr (WINDOW_Window, info_rwd->RWD_WindowObject, (uint32 *) & info_rwd->RWD_IWindow);
	  	break;
		case WMHI_UNICONIFY:
	  		DoMethod (info_rwd->RWD_WindowObject, WM_OPEN);
	  		GetAttr (WINDOW_Window, info_rwd->RWD_WindowObject, (uint32 *) & info_rwd->RWD_IWindow);
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
			// DisplayBeep(NULL);
	  	break;
#endif /* !NDEBUG */
		}
    }
#endif /* FM_REACT_SUPPORT */
  	return (rc);
}

/*  Do_WorkgroupRequest(): workgroup-requester (full handling).

        SYNOPSIS: void = Do_WorkgroupRequest
                        (
                        struct Window     *Win;
                        uint8                  *PubScreenName;
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

void Do_WorkgroupRequest (struct Window *Win, uint8 * PubScreenName, int16 Left, int16 Top)
{
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  	if (OpenWorkgroupReqWindow (PubScreenName) == TRUE)
    {
      	waitsigs = workgroup_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      	while (result == RESULT_NOTHING)
		{
	  		mask = Wait (waitsigs);
#ifndef NDEBUG
	  		Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  		if (mask & workgroup_rwd->RWD_WSignals)
	    	{
	      		result = HandleWorkgroupReqWindow (Win);
	    	}
	  
	  		if (mask & SIGBREAKF_CTRL_C)
	    	{
	      		result = RESULT_CANCEL;
	    	}
		}

      	CloseWorkgroupReqWindow ();
    }
  
  	else DisplayError (Win, TXT_ERR_Window, 5L);
#endif /* FM_REACT_SUPPORT */
}

#ifdef FM_REACT_SUPPORT

/*  OpenWorkgroupReqWindow(): open window of workgroup-requester.

        SYNOPSIS: int16 res = OpenWorkgroupReqWindow
                        (
                        uint8                  *PubScreenName;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16 OpenWorkgroupReqWindow (uint8 * PubScreenName)
{
  	workgroup_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  	
	if (workgroup_rwd != NULL)
    {
      	workgroup_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
						   								WA_Title, CATSTR (TITLE_WorkgroupReq),
						   								WA_IDCMP, IDCMP_RAWKEY,
						                                WA_NoCareRefresh, TRUE,
						                                WA_CloseGadget, TRUE,
						                                WA_DepthGadget, TRUE,
						                                WA_SizeGadget, FALSE,
						                                WA_DragBar, TRUE,
						                                WA_Activate, TRUE,
						                                WA_PubScreenName,
						                                PubScreenName,
						                                WA_PubScreenFallBack, TRUE,
						                                WINDOW_Position,
						                                WPOS_CENTERSCREEN,
						                                WINDOW_GadgetHelp, FALSE,
						                                WINDOW_IconifyGadget, FALSE,
						                                WINDOW_SharedPort,
						                                IDCMPortPtr, WINDOW_AppPort,
						                                AppPortPtr, WINDOW_Layout,
						                                NewObject (NULL,
							                            "layout.gadget",
							                            LAYOUT_Orientation,
							                            LAYOUT_ORIENT_VERT,
							                            LAYOUT_SpaceInner,
							                            TRUE,
							                            LAYOUT_SpaceOuter,
							                            TRUE,
							                            LAYOUT_BevelState,
							                            IDS_NORMAL,
							                            LAYOUT_BevelStyle,
							                            BVS_GROUP,
							                            LAYOUT_AddChild,
							                            NewObject (NULL,
									                    "layout.gadget",
									                    LAYOUT_Orientation,
									                    LAYOUT_ORIENT_VERT,
									                    LAYOUT_BevelState,
									                    IDS_SELECTED,
									                    LAYOUT_BevelStyle,
									                    BVS_FIELD,
									                    LAYOUT_SpaceInner,
									                    TRUE,
									                    LAYOUT_SpaceOuter,
									                    TRUE,
									                    LAYOUT_VertAlignment,
									                    LALIGN_CENTER,
									                    LAYOUT_AddImage,
									                    NewObject
									                    (NULL,
									                    "label.image",
									                    LABEL_Text,
									                    CATSTR
									                    (Work_TXT_Workgroup),
									                    LABEL_Justification,
									                    LJ_LEFT,
									                    TAG_DONE),
									 					TAG_DONE),
							      						LAYOUT_AddChild,
							      						NewObject (NULL,
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
									                    Work_BUTTON_OK,
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

		if (workgroup_rwd->RWD_WindowObject != NULL)
		{
	  		if (DoMethod (workgroup_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    	{
	      		GetAttr (WINDOW_Window, workgroup_rwd->RWD_WindowObject, (uint32 *) & workgroup_rwd->RWD_IWindow);
	      		GetAttr (WINDOW_SigMask, workgroup_rwd->RWD_WindowObject, &workgroup_rwd->RWD_WSignals);
	      		return (TRUE);
	    	}
		}
    }

  	return (FALSE);
}

/*  CloseWorkgroupReqWindow(): close window of workgroup-requester.

        SYNOPSIS: void = CloseWorkgroupReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void CloseWorkgroupReqWindow (void)
{
  	if (workgroup_rwd != NULL)
    {
      	if (workgroup_rwd->RWD_WindowObject)
		{
	  		DisposeObject (workgroup_rwd->RWD_WindowObject);
		}
      
	  	FreeVec (workgroup_rwd);
      	workgroup_rwd = NULL;
    }
}

/*  HandleWorkgroupReqWindow(): Handle the messages of the workgroup-requester.

        SYNOPSIS: int16 = HandleWorkgroupReqWindow
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

static int32
HandleWorkgroupReqWindow (struct Window *Win)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  	while ((result = DoMethod (workgroup_rwd->RWD_WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
    {

#ifndef NDEBUG
      	Printf ("result: %ld\n", result);
#endif /* !NDEBUG */

      	switch (result & WMHI_CLASSMASK)
		{
			case WMHI_CLOSEWINDOW:
	  			rc = RESULT_CANCEL;
	  		break;
			case WMHI_GADGETUP:
	  			switch (result & RL_GADGETMASK)
	    		{
	    			case Work_BUTTON_OK:
	      				rc = RESULT_OK;
	      			break;
#ifndef NDEBUG
	    			default:
	      				Printf ("unknown result: %lX\nunknown gadget: %lX\n", result, result & WMHI_GADGETMASK);
						// DisplayBeep(NULL);
	      			break;
#endif /* !NDEBUG */
	    		}
	  		break;
			case WMHI_ICONIFY:
	  			DoMethod (workgroup_rwd->RWD_WindowObject, WM_ICONIFY);
	  			GetAttr (WINDOW_Window, workgroup_rwd->RWD_WindowObject, (uint32 *) & workgroup_rwd->RWD_IWindow);
	  		break;
			case WMHI_UNICONIFY:
	  			DoMethod (workgroup_rwd->RWD_WindowObject, WM_OPEN);
	  			GetAttr (WINDOW_Window, workgroup_rwd->RWD_WindowObject, (uint32 *) & workgroup_rwd->RWD_IWindow);
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
				// DisplayBeep(NULL);
	  		break;
#endif /* !NDEBUG */
		}
    }
  
  	return (rc);
}
#endif /* FM_REACT_SUPPORT */

