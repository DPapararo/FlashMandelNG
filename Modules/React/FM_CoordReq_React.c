/*
 *  FlashMandel - FM_CoordReq_React.c
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
 *    $Id: FM_CoordReq_React.c,v 1.2 2004/06/30 17:23:41 eschwan Exp $
 *
 *    Code for coordinates-request with reaction-support
 *
 *    $Log: FM_CoordReq_React.c,v $
 *    Revision 1.4  2021/01/09 13:40:00  dpapararo
 *    Fixed KeepReal ratio management - >was passed pointer address and not value
 *    Added GMP math to "reactor" code
 *
 *    Revision 1.3  2020/12/18 00:00:00  eschwan
 *    Modified for "no reactor" code
 *    Made internal functions static
 *
 *    Revision 1.2  2004/06/30 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:23:41  eschwan
 *    First tracked version
 *
 *    Revision 2.2  2002/10/17 00:00:00  eschwan
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
#include <gadgets/string.h>
#include <gadgets/checkbox.h>

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

#include "FM_CoordReq_React.h"
#include "FM_ReactionBasics.h"

#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include <exec/types.h>
#include <GMP/gmp.h>

#include "flashmandel.h"

extern struct Catalog *CatalogPtr;

extern uint32 *PALETTE;
extern struct MandelChunk *MANDChunk;

extern struct MsgPort *IDCMPortPtr;
extern struct MsgPort *AppPortPtr;

static Object *string_top_gad = NULL;
static Object *string_left_gad = NULL;
static Object *string_right_gad = NULL;
static Object *string_bottom_gad = NULL;
static Object *string_juliareal_gad = NULL;
static Object *string_juliaimag_gad = NULL;
static Object *checkbox_axis_gad = NULL;

/* prototypes */
//void SaveCoords(struct Window *);
//void AdjustRatio(float64 *, float64 *, float64 *, float64 *, int16, int16, int16);

static struct ReactWinData *coord_rwd = NULL;

/* Data */
static mpf_t Tmp_RMIN, Tmp_IMAX, Tmp_RMAX, Tmp_IMIN, Tmp_JKRE, Tmp_JKIM;

extern float64 DEF_RMIN, DEF_RMAX, DEF_IMIN, DEF_IMAX, DEF_JKRE, DEF_JKIM;

static int16 OpenCoordReqWindow (uint8 * PubScreenName, int16 *);
static void CloseCoordReqWindow (void);
static int32 HandleCoordReqWindow (struct Window *Win, int16 * KeepReal);

/*  Do_CoordinatesRequest(): coordinates-requester (full handling).

		SYNOPSIS: int16 res = Do_CoordinatesRequest
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

		RETURNS:        res:
							Is TRUE, if changes are accepted.
*/

int16
Do_CoordinatesRequest (struct Window *Win, uint8 * PubScreenName, int16 Left,
			   int16 Top)
{
  int16 rc = FALSE;
#ifdef FM_REACT_SUPPORT
  int32 result = RESULT_NOTHING;
  int16 KeepReal = TRUE;
  uint32 waitsigs, mask;

  if (OpenCoordReqWindow (PubScreenName, &KeepReal) == TRUE)
	{
	  waitsigs = coord_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;

	  while (result == RESULT_NOTHING)
	{
	  mask = Wait (waitsigs);
#ifndef NDEBUG
	  Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
	  if (mask & coord_rwd->RWD_WSignals)
		{
		  result = HandleCoordReqWindow (Win, &KeepReal);
		}
	  if (mask & SIGBREAKF_CTRL_C)
		{
		  result = RESULT_CANCEL;
		}
	  if (result == RESULT_OK)
		rc = TRUE;
	}

	  CloseCoordReqWindow ();
	}
  else
	DisplayError (Win, TXT_ERR_Window, 5L);

  LoadRGB32 (ViewPortAddress (Win), (APTR) PALETTE);
#endif /* FM_REACT_SUPPORT */
  return (rc);
}

#ifdef FM_REACT_SUPPORT

/*  OpenCoordReqWindow(): open window of coordinates-requester.

		SYNOPSIS: int16 res = OpenCoordReqWindow
						(
						uint8  *PubScreenName;
						int16    *KeepReal;
						);

		INPUTS:     PubScreenName:
							name of the public-screen, where the window should appear.

						KeepReal:
							Is TRUE, if to keep the x-axis.

		RETURNS:        res:
							TRUE, if successfull.
*/

static int16
OpenCoordReqWindow (uint8 * PubScreenName, int16 * KeepReal)
{
  uint8 String_top[MAXCHARS];
  uint8 String_left[MAXCHARS];
  uint8 String_right[MAXCHARS];
  uint8 String_bottom[MAXCHARS];
  uint8 String_jreal[MAXCHARS];
  uint8 String_jimag[MAXCHARS];

  mpf_inits (Tmp_RMIN, Tmp_IMAX, Tmp_RMAX, Tmp_IMIN, Tmp_JKRE, Tmp_JKIM, 0);

  mpf_set (Tmp_IMAX, MANDChunk->GIMax);
  mpf_set (Tmp_RMIN, MANDChunk->GRMin);
  mpf_set (Tmp_RMAX, MANDChunk->GRMax);
  mpf_set (Tmp_IMIN, MANDChunk->GIMin);
  mpf_set (Tmp_JKRE, MANDChunk->GJKre);
  mpf_set (Tmp_JKIM, MANDChunk->GJKim);

  gmp_snprintf (String_top, sizeof (String_top), "%+2.1235Ff", Tmp_IMAX);
  gmp_snprintf (String_left, sizeof (String_left), "%+2.1235Ff", Tmp_RMIN);
  gmp_snprintf (String_right, sizeof (String_right), "%+2.1235Ff", Tmp_RMAX);
  gmp_snprintf (String_bottom, sizeof (String_bottom), "%+2.1235Ff", Tmp_IMIN);
  gmp_snprintf (String_jreal, sizeof (String_jreal), "%+2.1235Ff", Tmp_JKRE);
  gmp_snprintf (String_jimag, sizeof (String_jimag), "%+2.1235Ff", Tmp_JKIM);

  coord_rwd = AllocVec (sizeof (struct ReactWinData), MEMF_PUBLIC | MEMF_CLEAR);
  if (coord_rwd != NULL)
	{
	  coord_rwd->RWD_WindowObject = NewObject (NULL, "window.class",WA_Title, CATSTR (TITLE_CoordReq), 
	  WA_IDCMP, IDCMP_RAWKEY, WA_NoCareRefresh, TRUE, WA_CloseGadget, TRUE, WA_DepthGadget, TRUE, 
	  WA_SizeGadget, FALSE, WA_DragBar, TRUE, WA_Activate, TRUE, WA_PubScreenName, PubScreenName, 
	  WA_PubScreenFallBack, TRUE, WINDOW_Position, WPOS_CENTERSCREEN, WINDOW_GadgetHelp, FALSE, 
	  WINDOW_IconifyGadget, FALSE, WINDOW_SharedPort, IDCMPortPtr, WINDOW_AppPort, AppPortPtr, 
	  WINDOW_Layout,NewObject (NULL, "layout.gadget",   /* Coord_VERT_ROOT */
	  LAYOUT_Orientation, LAYOUT_ORIENT_VERT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_VERT_1 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_VERT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_BevelState, IDS_SELECTED, LAYOUT_BevelStyle, BVS_FIELD, LAYOUT_Label, CATSTR (Coord_TXT_Coord), 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_HORIZ_1_1 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_HorizAlignment, LALIGN_CENTER, 
	  LAYOUT_AddChild, string_top_gad = NewObject (NULL, "string.gadget",   /* Coord_STRING_Top */
	  GA_ID, Coord_STRING_Top, GA_RelVerify, TRUE, GA_TabCycle, TRUE, STRINGA_MaxChars, 21, 
	  STRINGA_TextVal, String_top, STRINGA_MinVisible, 20, STRINGA_HookType, SHK_FLOAT, 
	  STRINGA_ReplaceMode, TRUE, TAG_DONE), CHILD_WeightedWidth, 0, 
	  CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Coord_TXT_Top), 
	  LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "space.gadget", /* Coord_SPACE_1 */
	  CHILD_WeightedHeight, 2, TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget", /* Coord_HORIZ_1_2 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_HORIZ_1_2_1 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_AddChild, string_left_gad = NewObject (NULL, "string.gadget",  /* Coord_STRING_Left */
	  GA_ID, Coord_STRING_Left, GA_RelVerify, TRUE, GA_TabCycle, TRUE, STRINGA_MaxChars, 21, 
	  STRINGA_TextVal, String_left, STRINGA_MinVisible, 20, STRINGA_HookType, SHK_FLOAT, 
	  STRINGA_ReplaceMode, TRUE, TAG_DONE), CHILD_WeightedWidth, 0, 
	  CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Coord_TXT_Left), 
	  LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_HORIZ_1_2_2 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_AddChild, string_right_gad = NewObject (NULL, "string.gadget", /* Coord_STRING_Right */
	  GA_ID, Coord_STRING_Right, GA_RelVerify, TRUE, GA_TabCycle, TRUE, STRINGA_MaxChars, 21, 
	  STRINGA_TextVal, String_right, STRINGA_MinVisible, 20, STRINGA_HookType, SHK_FLOAT, 
	  STRINGA_ReplaceMode, TRUE, TAG_DONE), CHILD_WeightedWidth, 0, 
	  CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Coord_TXT_Right), 
	  LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "space.gadget", /* Coord_SPACE_2 */
	  CHILD_WeightedHeight, 2, TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_HORIZ_1_3 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_HorizAlignment, LALIGN_CENTER, LAYOUT_AddChild, 
	  string_bottom_gad = NewObject (NULL, "string.gadget", /* Coord_STRING_Bottom */
	  GA_ID, Coord_STRING_Bottom, GA_RelVerify, TRUE, GA_TabCycle, TRUE, STRINGA_MaxChars, 21, 
	  STRINGA_TextVal, String_bottom, STRINGA_MinVisible, 20, STRINGA_HookType, SHK_FLOAT, 
	  STRINGA_ReplaceMode, TRUE, TAG_DONE), CHILD_WeightedWidth, 0, 
	  CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Coord_TXT_Bottom), 
	  LABEL_Justification, LJ_LEFT, TAG_DONE), TAG_DONE), TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_VERT_2 */
	  LAYOUT_Orientation, LAYOUT_ORIENT_VERT, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_HorizAlignment, LALIGN_CENTER, LAYOUT_BevelState, IDS_SELECTED, 
	  LAYOUT_BevelStyle, BVS_FIELD, LAYOUT_Label, CATSTR (Coord_TXT_Julia), 
	  LAYOUT_LabelPlace, BVJ_TOP_CENTER, LAYOUT_AddChild, 
	  string_juliareal_gad = NewObject (NULL, "string.gadget",  /* Coord_STRING_JuliaReal */
	  GA_ID, Coord_STRING_JuliaReal, GA_RelVerify, TRUE, GA_TabCycle, TRUE, STRINGA_MaxChars, 21, 
	  STRINGA_TextVal, String_jreal, STRINGA_Justification, LJ_LEFT, STRINGA_MinVisible, 20, 
	  STRINGA_HookType, SHK_FLOAT, STRINGA_ReplaceMode, TRUE, TAG_DONE), CHILD_WeightedWidth, 0, 
	  CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Coord_TXT_JuliaReal), 
	  LABEL_Justification, LJ_LEFT, TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "space.gadget", /* Coord_SPACE_3 */
	  CHILD_WeightedHeight, 2, TAG_DONE), LAYOUT_AddChild, 
	  string_juliaimag_gad = NewObject (NULL, "string.gadget",  /* Coord_STRING_JuliaImag */
	  GA_ID, Coord_STRING_JuliaImag, GA_RelVerify, TRUE, GA_TabCycle, TRUE, STRINGA_MaxChars, 21, 
	  STRINGA_TextVal, String_jimag, STRINGA_Justification, LJ_LEFT, STRINGA_MinVisible, 20, 
	  STRINGA_HookType, SHK_FLOAT, STRINGA_ReplaceMode, TRUE, TAG_DONE), CHILD_WeightedWidth, 0, 
	  CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Coord_TXT_JuliaImag), 
	  LABEL_Justification, LJ_LEFT, TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "space.gadget", /* Coord_SPACE_4 */
	  CHILD_WeightedHeight, 2, TAG_DONE), TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "space.gadget", /* Coord_SPACE_6 */
	  CHILD_WeightedHeight, 2, TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "layout.gadget",    /* Coord_HORIZ_3 */
      LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, 
	  LAYOUT_HorizAlignment, LALIGN_CENTER, LAYOUT_AddChild, 
	  checkbox_axis_gad = NewObject (NULL, "checkbox.gadget",   /* Coord_CHECKBOX_KeepAxis */
	  GA_ID, Coord_CHECKBOX_KeepAxis, GA_RelVerify, TRUE, GA_Selected, *KeepReal, 
	  GA_Text, CATSTR (Coord_TXT_KeepAxis), CHECKBOX_TextPlace, PLACETEXT_RIGHT, TAG_DONE), 
	  CHILD_WeightedWidth, 0, TAG_DONE), 
	  LAYOUT_AddChild, NewObject (NULL, "space.gadget",   /* Coord_SPACE_7 */																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																						   
	  TAG_DONE),LAYOUT_AddChild,NewObject(NULL,"layout.gadget",LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,
	  LAYOUT_HorizAlignment,LALIGN_CENTER,LAYOUT_EvenSize,TRUE,LAYOUT_SpaceInner,TRUE,
		LAYOUT_SpaceOuter,TRUE,LAYOUT_AddChild,NewObject(NULL,"button.gadget",GA_ID,Coord_BUTTON_Accept,
		GA_RelVerify,TRUE,GA_Text,CATSTR(Coord_TXT_Accept),BUTTON_Justification,BCJ_CENTER,TAG_DONE),
		CHILD_WeightedHeight,0,CHILD_WeightedWidth,0,LAYOUT_AddChild,NewObject(NULL,"button.gadget",GA_ID,
		Coord_BUTTON_Ratio,GA_RelVerify,TRUE,GA_Text,CATSTR(Coord_TXT_Ratio),BUTTON_Justification,BCJ_CENTER,
		TAG_DONE),CHILD_WeightedHeight,0,CHILD_WeightedWidth,0,LAYOUT_AddChild,NewObject(NULL,"button.gadget",GA_ID,
		Coord_BUTTON_Default,GA_RelVerify,TRUE,GA_Text,CATSTR(Coord_TXT_Default),BUTTON_Justification,BCJ_CENTER,
		TAG_DONE),CHILD_WeightedHeight,0,CHILD_WeightedWidth,0,LAYOUT_AddChild,NewObject(NULL,"button.gadget",GA_ID,
		Coord_BUTTON_Cancel,GA_RelVerify,TRUE,GA_Text,CATSTR(Coord_TXT_Cancel),BUTTON_Justification,BCJ_CENTER,
		TAG_DONE),CHILD_WeightedHeight,0,CHILD_WeightedWidth,0,TAG_DONE),TAG_DONE),TAG_DONE);

	  if (coord_rwd->RWD_WindowObject != NULL)
	{
	  if (DoMethod (coord_rwd->RWD_WindowObject, WM_OPEN) != NULL)
		{
		  GetAttr (WINDOW_Window, coord_rwd->RWD_WindowObject,
			   (uint32 *) & coord_rwd->RWD_IWindow);
		  GetAttr (WINDOW_SigMask, coord_rwd->RWD_WindowObject,
			   &coord_rwd->RWD_WSignals);
		  return (TRUE);
		}
	}
	}
  return (FALSE);
}

/*  CloseCoordReqWindow(): close window of coordinates-requester.

		SYNOPSIS: void = CloseCoordReqWindow
						(
						void
						);

		INPUTS:     -

		RETURNS:        -
*/

static void
CloseCoordReqWindow (void)
{
  if (coord_rwd != NULL)
	{
	  if (coord_rwd->RWD_WindowObject)
	{
	  DisposeObject (coord_rwd->RWD_WindowObject);
	}
	  FreeVec (coord_rwd);
	  coord_rwd = NULL;
	}
}

/*  HandleCoordReqWindow(): Handle the messages of the coordinates-requester.

		SYNOPSIS: int32 res = HandlePalettePrefWindow
						(
						struct Window   *Win;
						);

		INPUTS:     Win:
							Pointer to a window-structure.

						KeepReal:
							.

		RETURNS:        res:
							result of messages: RESULT_NOTHING -> nothing happens.
														RESULT_CANCEL  -> user wants to quit without a change.
														RESULT_OK      -> accept changes and quit.
*/

static int32
HandleCoordReqWindow (struct Window *Win, int16 * KeepReal)
{
  int32 rc = RESULT_NOTHING;
  uint32 result, code;
  uint8 String[MAXCHARS];
  const uint8 *str = NULL;

  while ((result =
	  DoMethod (coord_rwd->RWD_WindowObject, WM_HANDLEINPUT,
			&code)) != WMHI_LASTMSG)
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
		case Coord_STRING_Top:
		  ;
		  break;
		case Coord_STRING_Left:
		  ;
		  break;
		case Coord_STRING_Right:
		  ;
		  break;
		case Coord_STRING_Bottom:
		  ;
		  break;
		case Coord_STRING_JuliaReal:
		  ;
		  break;
		case Coord_STRING_JuliaImag:
		  ;
		  break;
		case Coord_CHECKBOX_KeepAxis:
		  *KeepReal = !(*KeepReal);
		  break;
		case Coord_BUTTON_Accept:
		  {

		GetAttr (STRINGA_TextVal, string_top_gad, (uint32 *) & str);
		mpf_set_str (Tmp_IMAX, str, 10);
		GetAttr (STRINGA_TextVal, string_left_gad, (uint32 *) & str);
		mpf_set_str (Tmp_RMIN, str, 10);
		GetAttr (STRINGA_TextVal, string_right_gad, (uint32 *) & str);
		mpf_set_str (Tmp_RMAX, str, 10);
		GetAttr (STRINGA_TextVal, string_bottom_gad, (uint32 *) & str);
		mpf_set_str (Tmp_IMIN, str, 10);
		GetAttr (STRINGA_TextVal, string_juliareal_gad,
			 (uint32 *) & str);
		mpf_set_str (Tmp_JKRE, str, 10);
		GetAttr (STRINGA_TextVal, string_juliaimag_gad,
			 (uint32 *) & str);
		mpf_set_str (Tmp_JKIM, str, 10);

		if ((mpf_cmp (Tmp_RMIN, Tmp_RMAX) >= 0)
			|| (mpf_cmp (Tmp_IMIN, Tmp_IMAX)) >= 0)
		  DisplayBeep (Win->WScreen);
		else
		  {
			SaveCoords (Win);

			mpf_set (MANDChunk->GIMax, Tmp_IMAX);
			mpf_set (MANDChunk->GRMin, Tmp_RMIN);
			mpf_set (MANDChunk->GRMax, Tmp_RMAX);
			mpf_set (MANDChunk->GIMin, Tmp_IMIN);
			mpf_set (MANDChunk->GJKre, Tmp_JKRE);
			mpf_set (MANDChunk->GJKim, Tmp_JKIM);

			/* save coords even in float64 numbers for compatibility */
			MANDChunk->IMax = mpf_get_d (Tmp_IMAX);
			MANDChunk->RMin = mpf_get_d (Tmp_RMIN);
			MANDChunk->RMax = mpf_get_d (Tmp_RMAX);
			MANDChunk->IMin = mpf_get_d (Tmp_IMIN);
			MANDChunk->JKre = mpf_get_d (Tmp_JKRE);
			MANDChunk->JKim = mpf_get_d (Tmp_JKIM);

			rc = RESULT_OK;
		  }

		  }         /* case accept */
		  break;

		case Coord_BUTTON_Ratio:
		  {
		GetAttr (STRINGA_TextVal, string_top_gad, (uint32 *) & str);
		mpf_set_str (Tmp_IMAX, str, 10);
		GetAttr (STRINGA_TextVal, string_left_gad, (uint32 *) & str);
		mpf_set_str (Tmp_RMIN, str, 10);
		GetAttr (STRINGA_TextVal, string_right_gad, (uint32 *) & str);
		mpf_set_str (Tmp_RMAX, str, 10);
		GetAttr (STRINGA_TextVal, string_bottom_gad, (uint32 *) & str);
		mpf_set_str (Tmp_IMIN, str, 10);
		GetAttr (STRINGA_TextVal, string_juliareal_gad,
			 (uint32 *) & str);
		mpf_set_str (Tmp_JKRE, str, 10);
		GetAttr (STRINGA_TextVal, string_juliaimag_gad,
			 (uint32 *) & str);
		mpf_set_str (Tmp_JKIM, str, 10);

		if ((mpf_cmp (Tmp_RMIN, Tmp_RMAX) >= 0)
			|| (mpf_cmp (Tmp_IMIN, Tmp_IMAX)) >= 0)
		  DisplayBeep (Win->WScreen);
		else
		  AdjustRatio (&Tmp_RMIN, &Tmp_IMAX, &Tmp_RMAX, &Tmp_IMIN,
				   Win->Width, Win->Height, *KeepReal);

		gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMAX);
		SetGadgetAttrs (string_top_gad, coord_rwd->RWD_IWindow, NULL,
				STRINGA_TextVal, &String, TAG_DONE);
		gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMIN);
		SetGadgetAttrs (string_left_gad, coord_rwd->RWD_IWindow, NULL,
				STRINGA_TextVal, &String, TAG_DONE);
		gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMAX);
		SetGadgetAttrs (string_right_gad, coord_rwd->RWD_IWindow, NULL,
				STRINGA_TextVal, &String, TAG_DONE);
		gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMIN);
		SetGadgetAttrs (string_bottom_gad, coord_rwd->RWD_IWindow, NULL,
				STRINGA_TextVal, &String, TAG_DONE);
		  }
		  break;

		case Coord_BUTTON_Default:
		  mpf_set_d (Tmp_IMAX, DEF_IMAX);
		  mpf_set_d (Tmp_RMIN, DEF_RMIN);
		  mpf_set_d (Tmp_RMAX, DEF_RMAX);
		  mpf_set_d (Tmp_IMIN, DEF_IMIN);
		  mpf_set_d (Tmp_JKRE, DEF_JKRE);
		  mpf_set_d (Tmp_JKIM, DEF_JKIM);

		  gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMAX);
		  SetGadgetAttrs (string_top_gad, coord_rwd->RWD_IWindow, NULL,
				  STRINGA_TextVal, &String, TAG_DONE);
		  gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMIN);
		  SetGadgetAttrs (string_left_gad, coord_rwd->RWD_IWindow, NULL,
				  STRINGA_TextVal, &String, TAG_DONE);
		  gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMAX);
		  SetGadgetAttrs (string_right_gad, coord_rwd->RWD_IWindow, NULL,
				  STRINGA_TextVal, &String, TAG_DONE);
		  gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMIN);
		  SetGadgetAttrs (string_bottom_gad, coord_rwd->RWD_IWindow, NULL,
				  STRINGA_TextVal, &String, TAG_DONE);
		  gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_JKRE);
		  SetGadgetAttrs (string_juliareal_gad, coord_rwd->RWD_IWindow,
				  NULL, STRINGA_TextVal, &String, TAG_DONE);
		  gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_JKIM);
		  SetGadgetAttrs (string_juliaimag_gad, coord_rwd->RWD_IWindow,
				  NULL, STRINGA_TextVal, &String, TAG_DONE);
		  break;
		case Coord_BUTTON_Cancel:
		  rc = RESULT_CANCEL;
		  break;
#ifndef NDEBUG
		default:
		  Printf ("unknown result: %lX\nunknown gadget: %lX\n", result,
			  result & WMHI_GADGETMASK);
//                  DisplayBeep(NULL);
		  break;
#endif /* !NDEBUG */
		}
	  break;

	case WMHI_ICONIFY:
	  DoMethod (coord_rwd->RWD_WindowObject, WM_ICONIFY);
	  GetAttr (WINDOW_Window, coord_rwd->RWD_WindowObject,
		   (uint32 *) & coord_rwd->RWD_IWindow);
	  break;

	case WMHI_UNICONIFY:
	  DoMethod (coord_rwd->RWD_WindowObject, WM_OPEN);
	  GetAttr (WINDOW_Window, coord_rwd->RWD_WindowObject,
		   (uint32 *) & coord_rwd->RWD_IWindow);
	  break;
	case WMHI_RAWKEY:
	  switch (result & WMHI_KEYMASK)
		{
		case 0x45:      /* Escape */
		  rc = RESULT_CANCEL;
		  break;
		case 0x44:      /* Return */
		case 0x43:      /* Enter */
		  rc = RESULT_OK;
		  break;
		}
	  break;
#ifndef NDEBUG
	default:
	  Printf ("unknown result: %lX\nunknown class: %lX\n", result,
		  result & WMHI_CLASSMASK);
//          DisplayBeep(NULL);
	  break;
#endif /* !NDEBUG */
	}
	}
  return (rc);
}
#endif /* FM_REACT_SUPPORT */
