/*
 *  FlashMandel - FM_IntegerReq_React.c
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
 *    $Id: FM_IntegerReq_React.c,v 1.5 2004/07/01 17:24:08 eschwan Exp $
 *
 *    Code for cycledelay-request, iterations-request and precision-
 *    request with reaction-support.
 *
 *    $Log: FM_IntegerReq_React.c,v $
 *    Revison 1.5   2021/01/09 15:31 dpapararo
 *    Fixed integer requesters lower/upper limits for cycle, iterations, precision bits
 *
 *    Revision 1.4  2020/12/18 00:00:00  eschwan
 *    Modified for "no reactor" code
 *    Made internal functions static
 *
 *    Revision 1.3  2018/02/019 10:56:20
 *    cleanup code from all 68k and pre os4 stuff dpapararo
 *
 *    Revision 1.2  2004/07/01 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC, added precision-request
 *
 *    Revision 1.1  2004/03/01 17:24:08  eschwan
 *    First tracked version
 *
 *    Revision 2.1  2002/10/17 00:00:00  eschwan
 *    Last no-cvs-release.
 *
 *    Revision 2.2  2019/02/09 15:54:00  dpapararo
 *    Changed Do_IterationsRequest() from uint32 to int32 return value
 *
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
#include <gadgets/integer.h>

#ifdef __GNUC__
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

#include "FM_IntegerReq_React.h"
#include "FM_ReactionBasics.h"

#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include "Headers/FlashMandel.h"

extern struct Catalog *CatalogPtr;

extern uint32 *PALETTE;

static struct ReactWinData *cycle_rwd = NULL;
static struct ReactWinData *iterat_rwd = NULL;
static struct ReactWinData *prec_rwd = NULL;

extern struct MsgPort *IDCMPortPtr;
extern struct MsgPort *AppPortPtr;

static Object *cycle_int = NULL;
static Object *iterat_int = NULL;
static Object *prec_int = NULL;

static int16 OpenCycleReqWindow (char *PubScreenName, uint32 Value);
static void CloseCycleReqWindow (void);
static int32 HandleCycleReqWindow (struct Window *Win, uint32 * WorkValue);

static int16 OpenIteratReqWindow (char *PubScreenName, uint32 Value);
static void CloseIteratReqWindow (void);
static int32 HandleIteratReqWindow (struct Window *Win, uint32 * WorkValue);

static int16 OpenPrecReqWindow (char *PubScreenName, uint32 Value);
static void ClosePrecReqWindow (void);
static int32 HandlePrecReqWindow (struct Window *Win, uint32 * NewValue);

#define REQ_MIN_WIDTH 250
#define REQ_MIN_HEIGHT 60

/*  Do_CycleDelayRequest(): cycledelay-requester (full handling).

        SYNOPSIS: uint32 NewValue = Do_CycleDelayRequest
                        (
                        struct Window     *Win;
                        char                  *PubScreenName;
                        int16                    Left;
                        int16                    Top;
                        uint32                   Value;
                        );

        INPUTS:     Win:
                            Pointer to the parent-window.

                        PubScreenName:
                            name of the public-screen, where the window should appear.

                        Left:
                            Left edge of palette-window.

                        Top:
                            Top edge of palette-window.

                        Value:
                            Actual value for delay-time.

        RETURNS:        NewValue:
                            New value for delay-time.
*/

uint32
Do_CycleDelayRequest (struct Window *Win, char *PubScreenName, int16 Left,
		      int16 Top, uint32 Value)
{
  uint32 NewValue = Value;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  	if (OpenCycleReqWindow (PubScreenName, Value) == TRUE)
    {

      	waitsigs = cycle_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      	while (result == RESULT_NOTHING)
		{
	  		mask = Wait (waitsigs);
#ifndef NDEBUG
	  		Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  		if (mask & cycle_rwd->RWD_WSignals)
	    	{
	      		result = HandleCycleReqWindow (Win, &NewValue);
	      		if (result == RESULT_CANCEL) NewValue = Value;
	    	}
	  
	  		if (mask & SIGBREAKF_CTRL_C)
	    	{
	      		NewValue = Value;
	      		result = RESULT_CANCEL;
	    	}
		}

      	CloseCycleReqWindow ();
    }
  
  	else DisplayError (Win, TXT_ERR_Window, 5L);

  	LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
  	return (NewValue);
}

#ifdef FM_REACT_SUPPORT

/*  OpenCycleReqWindow(): open window of cycle-requester.

        SYNOPSIS: int16 res = OpenCycleReqWindow
                        (
                        char      *PubScreenName;
                        uint32       Value;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

                        Value:
                            Value for Integer-Gadget.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16 OpenCycleReqWindow (char *PubScreenName, uint32 Value)
{
  	cycle_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  	if (cycle_rwd != NULL)
    {
      cycle_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
					       						WA_Title,
					                            CATSTR (TITLE_CycleDelayReq),
					                            WA_IDCMP, IDCMP_RAWKEY,
					                            WA_NoCareRefresh, TRUE,
					                            WA_CloseGadget, TRUE,
					                            WA_DepthGadget, TRUE,
					                            WA_SizeGadget, FALSE, WA_DragBar,
					                            TRUE, WA_Activate, TRUE,
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
									            LAYOUT_BevelStyle,
									            BVS_THIN,
									            LAYOUT_DeferLayout,
									            TRUE,
									            LAYOUT_AddChild,
									            NewObject
									            (NULL,
									            "layout.gadget",
									            LAYOUT_Orientation,
									            LAYOUT_ORIENT_HORIZ,
									            LAYOUT_BevelState,
									            IDS_SELECTED,
									            LAYOUT_BevelStyle,
									            BVS_FIELD,
									            LAYOUT_SpaceInner,
									            TRUE,
									            LAYOUT_SpaceOuter,
									            TRUE,
									            LAYOUT_HorizAlignment,
									            LALIGN_CENTER,
									            LAYOUT_VertAlignment,
									            LALIGN_CENTER,
									            LAYOUT_AddChild,
									            cycle_int
									            =
									            NewObject
									            (NULL,
									            "integer.gadget",
									            GA_ID,
									            Cyc_INTEGER_DelayTime,
									            GA_RelVerify,
									            TRUE,
									            GA_ReadOnly,
									            FALSE,
									            INTEGER_Number,
									            Value,
									            INTEGER_MaxChars,
									            4,
									            INTEGER_Minimum,
									            MIN_DELAY,
									            INTEGER_Maximum,
									            MAX_DELAY,
									            INTEGER_MinVisible,
									            5,
									            INTEGER_Arrows,
									            TRUE,
									            STRINGA_ReplaceMode,
									            TRUE,
									            LAYOUT_AddImage,
									            NewObject
									            (NULL,
									            "label.image",
									            LABEL_Text,
									            CATSTR
									            (Cyc_TXT_DelayTime),
									            LABEL_Justification,
									            LJ_LEFT,
									            TAG_DONE),
									            TAG_DONE),
									            CHILD_WeightedWidth,
									            0,
									            CHILD_WeightedHeight,
									            0,
									            TAG_DONE),
									            CHILD_MinWidth,
									            REQ_MIN_WIDTH,
									            CHILD_MinHeight,
									            REQ_MIN_HEIGHT,
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
									            Cyc_BUTTON_Accept,
									            GA_RelVerify,
									            TRUE,
									            GA_Text,
									            CATSTR
									            (Cyc_TXT_Accept),
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
									            Cyc_BUTTON_Cancel,
									            GA_RelVerify,
									            TRUE,
									            GA_Text,
									            CATSTR
									            (Cyc_TXT_Cancel),
									            TAG_DONE),
									            CHILD_WeightedHeight,
									            0,
									            CHILD_WeightedWidth,
									            0,
									            TAG_DONE),
									            TAG_DONE),
					                            TAG_DONE);

		if (cycle_rwd->RWD_WindowObject != NULL)
		{
		  	if (DoMethod (cycle_rwd->RWD_WindowObject, WM_OPEN) != NULL)
		    {
		      	GetAttr (WINDOW_Window, cycle_rwd->RWD_WindowObject, (uint32 *) & cycle_rwd->RWD_IWindow);
		      	GetAttr (WINDOW_SigMask, cycle_rwd->RWD_WindowObject, &cycle_rwd->RWD_WSignals);
		      	return (TRUE);
		    }
		}
    }
  return (FALSE);
}

/*  CloseCycleReqWindow(): close window of cycle-requester.

        SYNOPSIS: void = CloseCycleReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void CloseCycleReqWindow (void)
{
  	if (cycle_rwd != NULL)
    {
      	if (cycle_rwd->RWD_WindowObject)
		{
	  		DisposeObject (cycle_rwd->RWD_WindowObject);
		}
    
	  	FreeVec (cycle_rwd);
      	cycle_rwd = NULL;
      	cycle_int = NULL;
    }
}

/*  HandleCycleReqWindow(): Handle the messages of the cycle-requester.

        SYNOPSIS: int32 = HandleCycleReqWindow
                        (
                        struct Window   *Win;
                        uint32               *NewValue
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        NewValue:
                            Pointer to a var to store the new value.

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user wants to quit without a change.
                                                        RESULT_OK      -> accept changes and quit.
*/

static int32 HandleCycleReqWindow (struct Window *Win, uint32 * NewValue)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  	while ((result = DoMethod (cycle_rwd->RWD_WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
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
	             	case Cyc_BUTTON_Accept:
	               		rc = RESULT_OK;
	               	break;
	             	case Cyc_BUTTON_Cancel:
	               		rc = RESULT_CANCEL;
	               	break;
	             	case Cyc_INTEGER_DelayTime:
	               		GetAttr (INTEGER_Number, cycle_int, NewValue);
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
	           		DoMethod (cycle_rwd->RWD_WindowObject, WM_ICONIFY);
	           		GetAttr (WINDOW_Window, cycle_rwd->RWD_WindowObject, (uint32 *) & cycle_rwd->RWD_IWindow);
	           	break;
         
	         	case WMHI_UNICONIFY:
	           		DoMethod (cycle_rwd->RWD_WindowObject, WM_OPEN);
	           		GetAttr (WINDOW_Window, cycle_rwd->RWD_WindowObject, (uint32 *) & cycle_rwd->RWD_IWindow);
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

/*  Do_IterationsRequest(): iteration-requester (full handling).

        SYNOPSIS: int32 NewValue = Do_IterationsRequest
                        (
                        struct Window     *Win;
                        char                  *PubScreenName;
                        int16                    Left;
                        int16                    Top;
                        int32                   Value;
                        );

        INPUTS:     Win:
                            Pointer to the parent-window.

                        PubScreenName:
                            name of the public-screen, where the window should appear.

                        Left:
                            Left edge of palette-window.

                        Top:
                            Top edge of palette-window.

                        Value:
                            Actual value for delay-time.

        RETURNS:        NewValue:
                            New value for delay-time.
*/

int32 Do_IterationsRequest (struct Window * Win, char *PubScreenName, int16 Left,
		      int16 Top, int32 Value)
{
  int32 NewValue = Value;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  	if (OpenIteratReqWindow (PubScreenName, Value) == TRUE)
    {
      	waitsigs = iterat_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      	while (result == RESULT_NOTHING)
		{
	  		mask = Wait (waitsigs);
#ifndef NDEBUG
	  		Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  		if (mask & iterat_rwd->RWD_WSignals)
	    	{
	      		result = HandleIteratReqWindow (Win, &NewValue);
	      		if (result == RESULT_CANCEL) NewValue = Value;
	    	}
	  
	  		if (mask & SIGBREAKF_CTRL_C)
	    	{
	      		NewValue = Value;
	      		result = RESULT_CANCEL;
	    	}
		}

      	CloseIteratReqWindow ();
    }
  
  	else DisplayError (Win, TXT_ERR_Window, 5L);

  	LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
  	return (NewValue);
}

#ifdef FM_REACT_SUPPORT

/*  OpenIteratReqWindow(): open window of iteration-requester.

        SYNOPSIS: int16 res = OpenIteratReqWindow
                        (
                        char      *PubScreenName;
                        uint32       Value;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

                        Value:
                            Value for integer-gadget.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16 OpenIteratReqWindow (char *PubScreenName, uint32 Value)
{
  	iterat_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  	if (iterat_rwd != NULL)
    {
      	iterat_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
													WA_Title,
													CATSTR (TITLE_IterationsReq),
													WA_IDCMP, IDCMP_RAWKEY,
													WA_NoCareRefresh, TRUE,
													WA_CloseGadget, TRUE,
													WA_DepthGadget, TRUE,
													WA_SizeGadget, FALSE,
													WA_DragBar, TRUE, WA_Activate,
													TRUE, WA_PubScreenName,
													PubScreenName,
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
									                LAYOUT_BevelStyle,
									                BVS_THIN,
									                LAYOUT_DeferLayout,
									                TRUE,
									                LAYOUT_AddChild,
									                NewObject
									                (NULL,
									                "layout.gadget",
									                LAYOUT_Orientation,
									                LAYOUT_ORIENT_HORIZ,
									                LAYOUT_BevelState,
									                IDS_SELECTED,
									                LAYOUT_BevelStyle,
									                BVS_FIELD,
									                LAYOUT_SpaceInner,
									                TRUE,
									                LAYOUT_SpaceOuter,
									                TRUE,
									                LAYOUT_HorizAlignment,
									                LALIGN_CENTER,
									                LAYOUT_VertAlignment,
									                LALIGN_CENTER,
									                LAYOUT_AddChild,
									                iterat_int
									                =
									                NewObject
									                (NULL,
									                "integer.gadget",
									                GA_ID,
									                It_INTEGER_Iterations,
									                GA_RelVerify,
									                TRUE,
									                GA_ReadOnly,
									                FALSE,
									                INTEGER_Number,
									                Value,
									                INTEGER_MaxChars,
									                10,
									                INTEGER_Minimum,
									                MIN_ITERATIONS,
									                INTEGER_Maximum,
									                MAX_ALLOWED_ITERATIONS,
									                INTEGER_MinVisible,
									                11,
									                INTEGER_Arrows,
									                TRUE,
									                STRINGA_ReplaceMode,
									                TRUE,
									                LAYOUT_AddImage,
									                NewObject
									                (NULL,
									                "label.image",
									                LABEL_Text,
									                CATSTR
									                (It_TXT_Iterations),
									                LABEL_Justification,
									                LJ_LEFT,
									     			TAG_DONE),
									    			TAG_DONE),
									                CHILD_WeightedWidth,
									                0,
									                CHILD_WeightedHeight,
									                0,
									                TAG_DONE),
									               	CHILD_MinWidth,
									               	REQ_MIN_WIDTH,
									               	CHILD_MinHeight,
									               	REQ_MIN_HEIGHT,
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
									                It_BUTTON_Accept,
									                GA_RelVerify,
									                TRUE,
									                GA_Text,
									                CATSTR
									                (It_TXT_Accept),
									                TAG_DONE),
									                CHILD_WeightedWidth,
									                0,
									                CHILD_WeightedHeight,
									                0,
									                LAYOUT_AddChild,
									                NewObject
									                (NULL,
									                "button.gadget",
									                GA_ID,
									                It_BUTTON_Cancel,
									                GA_RelVerify,
									                TRUE,
									                GA_Text,
									                CATSTR
									                (It_TXT_Cancel),
									                TAG_DONE),
									                CHILD_WeightedWidth,
									                0,
									                CHILD_WeightedHeight,
									                0,
									                TAG_DONE),
									               	TAG_DONE),
					                 				TAG_DONE);

      	if (iterat_rwd->RWD_WindowObject != NULL)
		{
	  		if (DoMethod (iterat_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    	{
	      		GetAttr (WINDOW_Window, iterat_rwd->RWD_WindowObject, (uint32 *) & iterat_rwd->RWD_IWindow);
	      		GetAttr (WINDOW_SigMask, iterat_rwd->RWD_WindowObject, &iterat_rwd->RWD_WSignals);
	      		return (TRUE);
	    	}
		}
    }
  
  	return (FALSE);
}

/*  CloseIteratReqWindow(): close window of iteration-requester.

        SYNOPSIS: void = CloseIteratReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void CloseIteratReqWindow (void)
{
  	if (iterat_rwd != NULL)
    {
      	if (iterat_rwd->RWD_WindowObject)
		{
	  		DisposeObject (iterat_rwd->RWD_WindowObject);
		}
      
	  	FreeVec (iterat_rwd);
      	iterat_rwd = NULL;
      	iterat_int = NULL;
    }
}

/*  HandleIteratReqWindow(): Handle the messages of the iteration-requester.

        SYNOPSIS: int32 = HandleIteratReqWindow
                        (
                        struct Window   *Win;
                        uint32               *NewValue
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        NewValue:
                            Pointer to a var to store the new value.

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user wants to quit without a change.
                                                        RESULT_OK      -> accept changes and quit.
*/

static int32 HandleIteratReqWindow (struct Window *Win, uint32 * NewValue)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  	while ((result = DoMethod (iterat_rwd->RWD_WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
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
	    			case It_BUTTON_Accept:
	      				rc = RESULT_OK;
	      			break;
	    			case It_BUTTON_Cancel:
	      				rc = RESULT_CANCEL;
	      			break;
	    			case It_INTEGER_Iterations:
	      				GetAttr (INTEGER_Number, iterat_int, NewValue);
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
	              	DoMethod (iterat_rwd->RWD_WindowObject, WM_ICONIFY);
	              	GetAttr (WINDOW_Window, iterat_rwd->RWD_WindowObject, (uint32 *) & iterat_rwd->RWD_IWindow);
	            break;
            
	            case WMHI_UNICONIFY:
	              	DoMethod (iterat_rwd->RWD_WindowObject, WM_OPEN);
	              	GetAttr (WINDOW_Window, iterat_rwd->RWD_WindowObject, (uint32 *) & iterat_rwd->RWD_IWindow);
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

/*  Do_PrecisionRequest(): precision-requester (full handling).

        SYNOPSIS: uint32 NewValue = Do_PrecisionRequest
                        (
                        struct Window     *Win;
                        char              *PubScreenName;
                        int16               Left;
                        int16               Top;
                        uint32              Value;
                        );

        INPUTS:     Win:
                        Pointer to the parent-window.

                    PubScreenName:
                        name of the public-screen, where the window should appear.

                    Left:
                        Left edge of palette-window.

                    Top:
                        Top edge of palette-window.

                    Value:
                        Actual value for number of digits.

        RETURNS:    NewValue:
                        New value for number of digits.
*/

uint32 Do_PrecisionRequest (struct Window * Win, char *PubScreenName, int16 Left, int16 Top, uint32 Value)
{
  uint32 NewValue = Value;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  	if (OpenPrecReqWindow (PubScreenName, Value) == TRUE)
    {
      	waitsigs = prec_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      	while (result == RESULT_NOTHING)
		{
	  		mask = Wait (waitsigs);
#ifndef NDEBUG
	  		Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  		if (mask & prec_rwd->RWD_WSignals)
	    	{
	      		result = HandlePrecReqWindow (Win, &NewValue);
	      		if (result == RESULT_CANCEL) NewValue = Value;
	    	}
	  
	  		if (mask & SIGBREAKF_CTRL_C)
	    	{
	      		NewValue = Value;
	      		result = RESULT_CANCEL;
	    	}
		}

      	ClosePrecReqWindow ();
    }
  
  	else DisplayError (Win, TXT_ERR_Window, 5L);

  	LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
  	return (NewValue);
}

#ifdef FM_REACT_SUPPORT

/*  OpenPrecReqWindow(): open window of precision-requester.

        SYNOPSIS: int16 res = OpenPrecReqWindow
                        (
                        char      *PubScreenName;
                        uint32      Value;
                        );

        INPUTS:     PubScreenName:
                        name of the public-screen, where the window should appear.

                    Value:
                        Value for integer-gadget.

        RETURNS:    res:
                        TRUE, if successfull.
*/

static int16 OpenPrecReqWindow (char *PubScreenName, uint32 Value)
{
  	prec_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  	
	if (prec_rwd != NULL)
    {
      	prec_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
					      WA_Title,
					      CATSTR (TITLE_PrecisionReq),
					      WA_IDCMP, IDCMP_RAWKEY,
					      WA_NoCareRefresh, TRUE,
					      WA_CloseGadget, TRUE,
					      WA_DepthGadget, TRUE,
					      WA_SizeGadget, FALSE, WA_DragBar,
					      TRUE, WA_Activate, TRUE,
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
									LAYOUT_BevelStyle,
									BVS_THIN,
									LAYOUT_DeferLayout,
									TRUE,
									LAYOUT_AddChild,
									NewObject
									(NULL,
									 "layout.gadget",
									 LAYOUT_Orientation,
									 LAYOUT_ORIENT_HORIZ,
									 LAYOUT_BevelState,
									 IDS_SELECTED,
									 LAYOUT_BevelStyle,
									 BVS_FIELD,
									 LAYOUT_SpaceInner,
									 TRUE,
									 LAYOUT_SpaceOuter,
									 TRUE,
									 LAYOUT_HorizAlignment,
									 LALIGN_CENTER,
									 LAYOUT_VertAlignment,
									 LALIGN_CENTER,
									 LAYOUT_AddChild,
									 prec_int
									 =
									 NewObject
									 (NULL,
									  "integer.gadget",
									  GA_ID,
									  Prec_INTEGER_Precision,
									  GA_RelVerify,
									  TRUE,
									  GA_ReadOnly,
									  FALSE,
									  INTEGER_Number,
									  Value,
									  INTEGER_MaxChars,
									  4,
									  INTEGER_Minimum,
									  MIN_PRECISION_BITS,
									  INTEGER_Maximum,
									  MAX_PRECISION_BITS,
									  INTEGER_MinVisible,
									  5,
									  INTEGER_Arrows,
									  TRUE,
									  STRINGA_ReplaceMode,
									  TRUE,
									  LAYOUT_AddImage,
									  NewObject
									  (NULL,
									   "label.image",
									   LABEL_Text,
									   CATSTR
									   (Prec_TXT_NumOfDigits),
									   LABEL_Justification,
									   LJ_LEFT,
									   TAG_DONE),
									  TAG_DONE),
									 CHILD_WeightedWidth,
									 0,
									 CHILD_WeightedHeight,
									 0,
									 TAG_DONE),
									CHILD_MinWidth,
									REQ_MIN_WIDTH,
									CHILD_MinHeight,
									REQ_MIN_HEIGHT,
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
									  Prec_BUTTON_Accept,
									  GA_RelVerify,
									  TRUE,
									  GA_Text,
									  CATSTR
									  (Prec_TXT_Accept),
									  TAG_DONE),
									 CHILD_WeightedWidth,
									 0,
									 CHILD_WeightedHeight,
									 0,
									 LAYOUT_AddChild,
									 NewObject
									 (NULL,
									  "button.gadget",
									  GA_ID,
									  Prec_BUTTON_Cancel,
									  GA_RelVerify,
									  TRUE,
									  GA_Text,
									  CATSTR
									  (Prec_TXT_Cancel),
									  TAG_DONE),
									 CHILD_WeightedWidth,
									 0,
									 CHILD_WeightedHeight,
									 0,
									 TAG_DONE),
									TAG_DONE),
					      			TAG_DONE);

      	if (prec_rwd->RWD_WindowObject != NULL)
		{
	  		if (DoMethod (prec_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    	{
	      		GetAttr (WINDOW_Window, prec_rwd->RWD_WindowObject, (uint32 *) & prec_rwd->RWD_IWindow);
	      		GetAttr (WINDOW_SigMask, prec_rwd->RWD_WindowObject, &prec_rwd->RWD_WSignals);
	      		return (TRUE);
	    	}
		}
    }
  
  	return (FALSE);
}

/*  ClosePrecReqWindow(): close window of precision-requester.

        SYNOPSIS: void = ClosePrecReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void ClosePrecReqWindow (void)
{
  	if (prec_rwd != NULL)
    {
      	if (prec_rwd->RWD_WindowObject)
		{
	  		DisposeObject (prec_rwd->RWD_WindowObject);
		}
      
	  	FreeVec (prec_rwd);
      	prec_rwd = NULL;
      	prec_int = NULL;
    }
}

/*  HandlePrecReqWindow(): Handle the messages of the precision-requester.

        SYNOPSIS: int32 = HandlePrecReqWindow
                        (
                        struct Window   *Win;
                        uint32           *NewValue
                        );

        INPUTS:     Win:
                        Pointer to a window-structure.

                    NewValue:
                        Pointer to a var to store the new value.

        RETURNS:    res:
                        result of messages: RESULT_NOTHING -> nothing happens.
                        RESULT_CANCEL  -> user wants to quit without a change.
                        RESULT_OK      -> accept changes and quit.
*/

static int32 HandlePrecReqWindow (struct Window *Win, uint32 * NewValue)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  	while ((result = DoMethod (prec_rwd->RWD_WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
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
	            	case Prec_BUTTON_Accept:
	              		rc = RESULT_OK;
	              	break;
	            	case Prec_BUTTON_Cancel:
	              		rc = RESULT_CANCEL;
	              	break;
	            	case Prec_INTEGER_Precision:
	              		GetAttr (INTEGER_Number, prec_int, NewValue);
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
	          	DoMethod (prec_rwd->RWD_WindowObject, WM_ICONIFY);
	          	GetAttr (WINDOW_Window, prec_rwd->RWD_WindowObject, (uint32 *) & prec_rwd->RWD_IWindow);
	        break;
        
	        case WMHI_UNICONIFY:
	          	DoMethod (prec_rwd->RWD_WindowObject, WM_OPEN);
	          	GetAttr (WINDOW_Window, prec_rwd->RWD_WindowObject, (uint32 *) & prec_rwd->RWD_IWindow);
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
