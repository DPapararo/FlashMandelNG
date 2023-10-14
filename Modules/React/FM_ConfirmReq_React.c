/*
 *  FlashMandel - FM_ConfirmReq_React.c
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
 *    $Id: FM_ConfirmReq_React.c,v 1.2 2004/06/29 17:23:28 eschwan Exp $
 *
 *    Requester for:
 *     print-request,
 *     exit-request,
 *     render-request,
 *     overwrite-request,
 *     newscr-request with reaction-support
 *
 *    $Log: FM_ConfirmReq_React.c,v $
 *    Revision 1.3  2020/12/18 00:00:00  eschwan
 *    Modified for "no reactor" code
 *    Made internal functions static
 *
 *    Revision 1.2  2004/06/29 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:23:28  eschwan
 *    First tracked version
 *
 *    Revision 2.1  2002/10/17 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#define NDEBUG


#include <stdlib.h>
#include <stdio.h>

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/locale.h>

#include <classes/window.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <images/label.h>

#include "FM_ConfirmReq_React.h"
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

static struct ReactWinData *print_rwd = NULL;
static struct ReactWinData *exit_rwd = NULL;
static struct ReactWinData *newscr_rwd = NULL;
static struct ReactWinData *render_rwd = NULL;
static struct ReactWinData *overwr_rwd = NULL;

static int16 OpenPrintReqWindow (char *PubScreenName);
static void ClosePrintReqWindow (void);
static int32 HandlePrintReqWindow (void);

static int16 OpenExitReqWindow (char *PubScreenName);
static void CloseExitReqWindow (void);
static int32 HandleExitReqWindow (void);

static int16 OpenNewScrReqWindow (char *PubScreenName);
static void CloseNewScrReqWindow (void);
static int32 HandleNewScrReqWindow (void);

static int16 OpenRenderReqWindow (char *PubScreenName);
static void CloseRenderReqWindow (void);
static int32 HandleRenderReqWindow (void);

static int16 OpenOverwriteReqWindow (char *PubScreenName);
static void CloseOverwriteReqWindow (void);
static int32 HandleOverwriteReqWindow (void);

#define REQ_MIN_WIDTH 250
#define REQ_MIN_HEIGHT 60

/*  Do_PrintRequest(): print-requester (full handling).

        SYNOPSIS: int16 res = Do_PrintRequest
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
                            Left edge of print-window (not supported yet).

                        Top:
                            Top edge of print-window (not supported yet).

        RETURNS:        res:
                            Is TRUE, if user confirms to print.
*/

int16
Do_PrintRequest (struct Window *Win, char *PubScreenName, int16 Left, int16 Top)
{
  int16 rc = FALSE;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  if (OpenPrintReqWindow (PubScreenName) == TRUE)
    {
      waitsigs = print_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      while (result == RESULT_NOTHING)
	{
	  mask = Wait (waitsigs);
	  if (mask & print_rwd->RWD_WSignals)
	    {
	      result = HandlePrintReqWindow ();
	    }
	  if (mask & SIGBREAKF_CTRL_C)
	    {
	      result = RESULT_CANCEL;
	    }
	  if (result == RESULT_OK)
	    rc = TRUE;
	}

      ClosePrintReqWindow ();
    }
  else
    DisplayError (Win, TXT_ERR_Window, 5L);

  LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);

#endif /* FM_REACT_SUPPORT */
  return (rc);
}

#ifdef FM_REACT_SUPPORT

/*  OpenPrintReqWindow(): open window of print-requester.

        SYNOPSIS: int16 res = OpenPrintReqWindow
                        (
                        char      *PubScreenName;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenPrintReqWindow (char *PubScreenName)
{
  print_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  if (print_rwd != NULL)
    {
      print_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
					       WA_Title,
					       CATSTR (TITLE_PrintReq),
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
									 //LAYOUT_FillPattern, 0xF0F0,
									 //LAYOUT_FillPen, 2,
									 LAYOUT_BevelStyle,
									 BVS_THIN,
									 LAYOUT_DeferLayout,
									 TRUE,
									 LAYOUT_AddChild,
									 NewObject
									 (NULL,
									  "button.gadget",
									  GA_ReadOnly,
									  TRUE,
									  GA_Text,
									  CATSTR
									  (Prt_TXT_AreYouSure),
									  BUTTON_Justification,
									  BCJ_CENTER,
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
									   Prt_BUTTON_Yes,
									   GA_RelVerify,
									   TRUE,
									   GA_Text,
									   CATSTR
									   (Prt_TXT_Yes),
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
									   Prt_BUTTON_No,
									   GA_RelVerify,
									   TRUE,
									   GA_Text,
									   CATSTR
									   (Prt_TXT_No),
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

      if (print_rwd->RWD_WindowObject != NULL)
	{
	  if (DoMethod (print_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    {
	      GetAttr (WINDOW_Window, print_rwd->RWD_WindowObject,
		       (uint32 *) & print_rwd->RWD_IWindow);
	      GetAttr (WINDOW_SigMask, print_rwd->RWD_WindowObject,
		       &print_rwd->RWD_WSignals);
	      return (TRUE);
	    }
	}
    }

  return (FALSE);
}

/*  ClosePrintReqWindow(): close window of print-requester.

        SYNOPSIS: void = ClosePrintReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void
ClosePrintReqWindow (void)
{
  if (print_rwd != NULL)
    {
      if (print_rwd->RWD_WindowObject)
	{
	  DisposeObject (print_rwd->RWD_WindowObject);
	}
      FreeVec (print_rwd);
      print_rwd = NULL;
    }
}

/*  HandlePrintReqWindow(): Handle the messages of the print-requester.

        SYNOPSIS: int32 res = HandlePrintReqWindow
                        (
                        void;
                        );

        INPUTS:     -

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user canceled.
                                                        RESULT_OK      -> user wants to print.
*/

static int32
HandlePrintReqWindow (void)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  while ((result =
	  DoMethod (print_rwd->RWD_WindowObject, WM_HANDLEINPUT,
		    &code)) != WMHI_LASTMSG)
    {

      switch (result & WMHI_CLASSMASK)
	{
	case WMHI_CLOSEWINDOW:
	  rc = RESULT_CANCEL;
	  break;

	case WMHI_GADGETUP:
	  switch (result & RL_GADGETMASK)
	    {
	    case Prt_BUTTON_Yes:
	      rc = RESULT_OK;
	      break;
	    case Prt_BUTTON_No:
	      rc = RESULT_CANCEL;
	      break;
	    }
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
/*              default:
                    {
                    char test = result & WMHI_KEYMASK;
                    ;
                    }*/
	    }
	  break;
	}
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */

/*  Do_ExitRequest(): exit-requester (full handling).

        SYNOPSIS: int16 res = Do_ExitRequest
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
                            Left edge of exit-window (not supported yet).

                        Top:
                            Top edge of exit-window (not supported yet).

        RETURNS:        res:
                            Is TRUE, if user confirms to exit.
*/

int16
Do_ExitRequest (struct Window * Win, char *PubScreenName, int16 Left, int16 Top)
{
  int16 rc = FALSE;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  if (OpenExitReqWindow (PubScreenName) == TRUE)
    {
      waitsigs = exit_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      while (result == RESULT_NOTHING)
	{
	  mask = Wait (waitsigs);
	  if (mask & exit_rwd->RWD_WSignals)
	    {
	      result = HandleExitReqWindow ();
	    }
	  if (mask & SIGBREAKF_CTRL_C)
	    {
	      result = RESULT_CANCEL;
	    }
	  if (result == RESULT_OK)
	    rc = TRUE;
	}

      CloseExitReqWindow ();
    }
  else
    DisplayError (Win, TXT_ERR_Window, 5L);

  LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);

#endif /* FM_REACT_SUPPORT */
  return (rc);
}

#ifdef FM_REACT_SUPPORT

/*  OpenExitReqWindow(): open window of exit-requester.

        SYNOPSIS: int16 res = OpenExitReqWindow
                        (
                        char                  *PubScreenName;
                        );

        INPUTS:     -

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenExitReqWindow (char *PubScreenName)
{
  exit_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  if (exit_rwd != NULL)
    {
      exit_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
					      WA_Title, CATSTR (TITLE_ExitReq),
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
									LAYOUT_BevelStyle,
									BVS_THIN,
									LAYOUT_DeferLayout,
									TRUE,
									LAYOUT_AddChild,
									NewObject
									(NULL,
									 "button.gadget",
									 GA_ReadOnly,
									 TRUE,
									 GA_Text,
									 CATSTR
									 (Prt_TXT_AreYouSure),
									 BUTTON_Justification,
									 BCJ_CENTER,
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
									  Exit_BUTTON_Yes,
									  GA_RelVerify,
									  TRUE,
									  GA_Text,
									  CATSTR
									  (Prt_TXT_Yes),
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
									  Exit_BUTTON_No,
									  GA_RelVerify,
									  TRUE,
									  GA_Text,
									  CATSTR
									  (Prt_TXT_No),
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

      if (exit_rwd->RWD_WindowObject != NULL)
	{
	  if (DoMethod (exit_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    {
	      GetAttr (WINDOW_Window, exit_rwd->RWD_WindowObject,
		       (uint32 *) & exit_rwd->RWD_IWindow);
	      GetAttr (WINDOW_SigMask, exit_rwd->RWD_WindowObject,
		       &exit_rwd->RWD_WSignals);
	      return (TRUE);
	    }
	}
    }
	
  return (FALSE);
}

/*  CloseExitReqWindow(): close window of exit-requester.

        SYNOPSIS: void = CloseExitReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void
CloseExitReqWindow (void)
{
  if (exit_rwd != NULL)
    {
      if (exit_rwd->RWD_WindowObject)
	{
	  DisposeObject (exit_rwd->RWD_WindowObject);
	}
      FreeVec (exit_rwd);
      exit_rwd = NULL;
    }
}

/*  HandleExitReqWindow(): Handle the messages of the exit-requester.

        SYNOPSIS: int32 res = HandleExitReqWindow
                        (
                        void;
                        );

        INPUTS:     -

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user canceled.
                                                        RESULT_OK      -> user wants to exit.
*/

static int32
HandleExitReqWindow (void)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  while ((result =
	  DoMethod (exit_rwd->RWD_WindowObject, WM_HANDLEINPUT,
		    &code)) != WMHI_LASTMSG)
    {

      switch (result & WMHI_CLASSMASK)
	{
	case WMHI_CLOSEWINDOW:
	  rc = RESULT_CANCEL;
	  break;

	case WMHI_GADGETUP:
	  switch (result & RL_GADGETMASK)
	    {
	    case Exit_BUTTON_Yes:
	      rc = RESULT_OK;
	      break;
	    case Exit_BUTTON_No:
	      rc = RESULT_CANCEL;
	      break;
	    }
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
	}
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */

/*  Do_NewScrRequest(): newscr-requester (full handling).

        SYNOPSIS: int16 res = Do_NewScrRequest
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
                            Left edge of newscr-window (not supported yet).

                        Top:
                            Top edge of newscr-window (not supported yet).

        RETURNS:        res:
                            Is TRUE, if user confirms to update rendering.
*/

int16
Do_NewScrRequest (struct Window * Win, char *PubScreenName, int16 Left, int16 Top)
{
  int16 rc = FALSE;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  if (OpenNewScrReqWindow (PubScreenName) == TRUE)
    {
      waitsigs = newscr_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      while (result == RESULT_NOTHING)
	{
	  mask = Wait (waitsigs);
	  if (mask & newscr_rwd->RWD_WSignals)
	    {
	      result = HandleNewScrReqWindow ();
	    }
	  if (mask & SIGBREAKF_CTRL_C)
	    {
	      result = RESULT_CANCEL;
	    }
	  if (result == RESULT_OK)
	    rc = TRUE;
	}

      CloseNewScrReqWindow ();
    }

  LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
  return (rc);
}

#ifdef FM_REACT_SUPPORT

/*  OpenNewScrReqWindow(): open window of newscr-requester.

        SYNOPSIS: int16 res = OpenNewScrReqWindow
                        (
                        char                  *PubScreenName;
                        );

        INPUTS:     -

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenNewScrReqWindow (char *PubScreenName)
{
  newscr_rwd =
    AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  if (newscr_rwd != NULL)
    {
      newscr_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
						WA_Title,
						CATSTR (TITLE_RendReq),
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
									    (NewScr_TXT_Question),
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
									    NewScr_BUTTON_Yes,
									    GA_RelVerify,
									    TRUE,
									    GA_Text,
									    CATSTR
									    (Prt_TXT_Yes),
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
									    NewScr_BUTTON_No,
									    GA_RelVerify,
									    TRUE,
									    GA_Text,
									    CATSTR
									    (Prt_TXT_No),
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

      if (newscr_rwd->RWD_WindowObject != NULL)
	{
	  if (DoMethod (newscr_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    {
	      GetAttr (WINDOW_Window, newscr_rwd->RWD_WindowObject,
		       (uint32 *) & newscr_rwd->RWD_IWindow);
	      GetAttr (WINDOW_SigMask, newscr_rwd->RWD_WindowObject,
		       &newscr_rwd->RWD_WSignals);
	      return (TRUE);
	    }
	}
    }
  return (FALSE);
}

/*  CloseNewScrReqWindow(): close window of newscr-requester.

        SYNOPSIS: void = CloseNewScrReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void
CloseNewScrReqWindow (void)
{
  if (newscr_rwd != NULL)
    {
      if (newscr_rwd->RWD_WindowObject)
	{
	  DisposeObject (newscr_rwd->RWD_WindowObject);
	}
      FreeVec (newscr_rwd);
      newscr_rwd = NULL;
    }
}

/*  HandleNewScrReqWindow(): Handle the messages of the newscr-requester.

        SYNOPSIS: int32 res = HandleNewScrReqWindow
                        (
                        void;
                        );

        INPUTS:     -

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user canceled.
                                                        RESULT_OK      -> user wants to render.
*/

static int32
HandleNewScrReqWindow (void)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  while ((result =
	  DoMethod (newscr_rwd->RWD_WindowObject, WM_HANDLEINPUT,
		    &code)) != WMHI_LASTMSG)
    {

      switch (result & WMHI_CLASSMASK)
	{
	case WMHI_CLOSEWINDOW:
	  rc = RESULT_CANCEL;
	  break;

	case WMHI_GADGETUP:
	  switch (result & RL_GADGETMASK)
	    {
	    case NewScr_BUTTON_Yes:
	      rc = RESULT_OK;
	      break;
	    case NewScr_BUTTON_No:
	      rc = RESULT_CANCEL;
	      break;
	    }
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
	}
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */

/*  Do_RenderRequest(): render-requester (full handling).

        SYNOPSIS: int16 res = Do_RenderRequest
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
                            Left edge of render-window (not supported yet).

                        Top:
                            Top edge of render-window (not supported yet).

        RETURNS:        res:
                            Is TRUE, if user confirms to update rendering.
*/

int16
Do_RenderRequest (struct Window * Win, char *PubScreenName, int16 Left, int16 Top)
{
  int16 rc = FALSE;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  if (OpenRenderReqWindow (PubScreenName) == TRUE)
    {
      waitsigs = render_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      while (result == RESULT_NOTHING)
	{
	  mask = Wait (waitsigs);
	  if (mask & render_rwd->RWD_WSignals)
	    {
	      result = HandleRenderReqWindow ();
	    }
	  if (mask & SIGBREAKF_CTRL_C)
	    {
	      result = RESULT_CANCEL;
	    }
	  if (result == RESULT_OK)
	    rc = TRUE;
	}

      CloseRenderReqWindow ();
    }
  else
    DisplayError (Win, TXT_ERR_Window, 5L);

  LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);

#endif /* FM_REACT_SUPPORT */
  return (rc);
}

#ifdef FM_REACT_SUPPORT

/*  OpenRenderReqWindow(): open window of render-requester.

        SYNOPSIS: int16 res = OpenRenderReqWindow
                        (
                        char                  *PubScreenName;
                        );

        INPUTS:     -

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenRenderReqWindow (char *PubScreenName)
{
  render_rwd =
    AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  if (render_rwd != NULL)
    {
      render_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
						WA_Title,
						CATSTR (TITLE_RendReq),
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
									   "button.gadget",
									   GA_ReadOnly,
									   TRUE,
									   GA_Text,
									   CATSTR
									   (Rend_TXT_Question),
									   BUTTON_Justification,
									   BCJ_CENTER,
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
									    Rend_BUTTON_Yes,
									    GA_RelVerify,
									    TRUE,
									    GA_Text,
									    CATSTR
									    (Prt_TXT_Yes),
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
									    Rend_BUTTON_No,
									    GA_RelVerify,
									    TRUE,
									    GA_Text,
									    CATSTR
									    (Prt_TXT_No),
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

      if (render_rwd->RWD_WindowObject != NULL)
	{
	  if (DoMethod (render_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    {
	      GetAttr (WINDOW_Window, render_rwd->RWD_WindowObject,
		       (uint32 *) & render_rwd->RWD_IWindow);
	      GetAttr (WINDOW_SigMask, render_rwd->RWD_WindowObject,
		       &render_rwd->RWD_WSignals);
	      return (TRUE);
	    }
	}
    }
  return (FALSE);
}

/*  CloseRenderReqWindow(): close window of render-requester.

        SYNOPSIS: void = CloseRenderReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void
CloseRenderReqWindow (void)
{
  if (render_rwd != NULL)
    {
      if (render_rwd->RWD_WindowObject)
	{
	  DisposeObject (render_rwd->RWD_WindowObject);
	}
      FreeVec (render_rwd);
      render_rwd = NULL;
    }
}

/*  HandleRenderReqWindow(): Handle the messages of the render-requester.

        SYNOPSIS: int32 res = HandleRenderReqWindow
                        (
                        void;
                        );

        INPUTS:     -

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user canceled.
                                                        RESULT_OK      -> user wants to render.
*/

static int32
HandleRenderReqWindow (void)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  while ((result =
	  DoMethod (render_rwd->RWD_WindowObject, WM_HANDLEINPUT,
		    &code)) != WMHI_LASTMSG)
    {

      switch (result & WMHI_CLASSMASK)
	{
	case WMHI_CLOSEWINDOW:
	  rc = RESULT_CANCEL;
	  break;

	case WMHI_GADGETUP:
	  switch (result & RL_GADGETMASK)
	    {
	    case Rend_BUTTON_Yes:
	      rc = RESULT_OK;
	      break;
	    case Rend_BUTTON_No:
	      rc = RESULT_CANCEL;
	      break;
	    }
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
	}
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */

/*  Do_OverwriteRequest(): overwrite-requester (full handling).

        SYNOPSIS: int16 res = Do_OverwriteRequest
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
                            Left edge of overwr-window (not supported yet).

                        Top:
                            Top edge of overwr-window (not supported yet).

        RETURNS:        res:
                            Is TRUE, if user confirms to overwrite file.
*/

int16
Do_OverwriteRequest (struct Window * Win, char *PubScreenName, int16 Left,
		     int16 Top)
{
  int16 rc = FALSE;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  uint32 waitsigs, mask;

  if (OpenOverwriteReqWindow (PubScreenName) == TRUE)
    {
      waitsigs = overwr_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

      while (result == RESULT_NOTHING)
	{
	  mask = Wait (waitsigs);
	  if (mask & overwr_rwd->RWD_WSignals)
	    {
	      result = HandleOverwriteReqWindow ();
	    }
	  if (mask & SIGBREAKF_CTRL_C)
	    {
	      result = RESULT_CANCEL;
	    }
	  if (result == RESULT_OK)
	    rc = TRUE;
	}

      CloseOverwriteReqWindow ();
    }

  LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
  return (rc);
}

#ifdef FM_REACT_SUPPORT

/*  OpenOverwriteReqWindow(): open window of overwrite-requester.

        SYNOPSIS: int16 res = OpenOverwriteReqWindow
                        (
                        char                  *PubScreenName;
                        );

        INPUTS:     -

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenOverwriteReqWindow (char *PubScreenName)
{
  overwr_rwd =
    AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  if (overwr_rwd != NULL)
    {
      overwr_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
						WA_Title,
						CATSTR (TITLE_OverwriteReq),
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
									    (OverWr_TXT_Question),
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
									    OverWr_BUTTON_Yes,
									    GA_RelVerify,
									    TRUE,
									    GA_Text,
									    CATSTR
									    (Prt_TXT_Yes),
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
									    OverWr_BUTTON_No,
									    GA_RelVerify,
									    TRUE,
									    GA_Text,
									    CATSTR
									    (Prt_TXT_No),
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

      if (overwr_rwd->RWD_WindowObject != NULL)
	{
	  if (DoMethod (overwr_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    {
	      GetAttr (WINDOW_Window, overwr_rwd->RWD_WindowObject,
		       (uint32 *) & overwr_rwd->RWD_IWindow);
	      GetAttr (WINDOW_SigMask, overwr_rwd->RWD_WindowObject,
		       &overwr_rwd->RWD_WSignals);
	      return (TRUE);
	    }
	}
    }
  return (FALSE);
}

/*  CloseOverwriteReqWindow(): close window of overwrite-requester.

        SYNOPSIS: void = CloseOverwriteReqWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        -
*/

static void
CloseOverwriteReqWindow (void)
{
  if (overwr_rwd != NULL)
    {
      if (overwr_rwd->RWD_WindowObject)
	{
	  DisposeObject (overwr_rwd->RWD_WindowObject);
	}
      FreeVec (overwr_rwd);
      overwr_rwd = NULL;
    }
}

/*  HandleOverwriteReqWindow(): Handle the messages of the overwrite-requester.

        SYNOPSIS: int32 res = HandleOverwriteReqWindow
                        (
                        void;
                        );

        INPUTS:     -

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user canceled.
                                                        RESULT_OK      -> user wants to overwrite.
*/

static int32
HandleOverwriteReqWindow (void)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;

  while ((result =
	  DoMethod (overwr_rwd->RWD_WindowObject, WM_HANDLEINPUT,
		    &code)) != WMHI_LASTMSG)
    {

      switch (result & WMHI_CLASSMASK)
	{
	case WMHI_CLOSEWINDOW:
	  rc = RESULT_CANCEL;
	  break;

	case WMHI_GADGETUP:
	  switch (result & RL_GADGETMASK)
	    {
	    case OverWr_BUTTON_Yes:
	      rc = RESULT_OK;
	      break;
	    case OverWr_BUTTON_No:
	      rc = RESULT_CANCEL;
	      break;
	    }
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
	}
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */
