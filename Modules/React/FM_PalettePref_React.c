/*
 *  FlashMandel - FM_PaletteReq_React.c
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
 *    $Id: FM_PalettePref_React.c,v 1.2 2004/06/30 17:24:19 eschwan Exp $
 *
 *    Code for palette-prefs with reaction-support
 *
 *    $Log: FM_PalettePref_React.c,v $
 *
 *    Revision 1.4  2020/12/26 00:00:00  eschwan
 *    Fixed Spread() function
 *
 *    Revision 1.3  2020/12/18 00:00:00  eschwan
 *    Modified for "no reactor" code
 *    Made internal functions static
 *
 *    Revision 1.2  2004/06/30 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:24:19  eschwan
 *    First tracked version
 *
 *    Revision 2.2  2002/06/17 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#define NDEBUG

#ifdef __amigaos4__
#define __USE_INLINE__
#define __USE_BASETYPE__
#endif /* __amigaos4__ */

//#define INTERCONNECTION 1

#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <intuition/screens.h>
#ifdef INTERCONNECTION
#include <intuition/icclass.h>
#endif
#include <classes/window.h>
#include <classes/arexx.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <images/label.h>
#include <gadgets/palette.h>
#include <gadgets/slider.h>
#include <gadgets/integer.h>

#ifdef __GNUC__
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/resource.h>
#include <proto/locale.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>
#include <clib/macros.h>
#else /* __GNUC__ */
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/resource_protos.h>
#include <clib/locale_protos.h>
#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/macros.h>

#include <pragmas/intuition_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/resource_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/dos_pragmas.h>
#endif /* __GNUC__ */

#include <stdlib.h>
#include <stdio.h>

#include "FM_PalettePref_React.h"
#include "FM_ReactionBasics.h"
#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

extern struct Catalog *CatalogPtr;
extern uint32 *PALETTE;

struct Library *PaletteBase = NULL;

static struct ReactWinData *pal_rwd = NULL;

extern struct MsgPort *IDCMPortPtr;
extern struct MsgPort *AppPortPtr;

static Object *palette_gad = NULL;
static Object *slider_red_gad = NULL;
static Object *slider_green_gad = NULL;
static Object *slider_blue_gad = NULL;
static Object *integer_red_gad = NULL;
static Object *integer_green_gad = NULL;
static Object *integer_blue_gad = NULL;

/* defaults */
#define DEF_PEN     3
#define STARTPEN   (4L)
#define ENDPEN     (255L)
#define MINVALUE   (0L)
#define MAXVALUE   (255L)
#define DELTA      (10L)
#define SHIFT_QUAL (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)

#ifdef FM_REACT_SUPPORT
/* local prototypes */
static int16 OpenPalettePrefWindow (char *PubScreenName);
static void ClosePalettePrefWindow (void);
static int32 HandlePalettePrefWindow (struct Window *Win,
				     struct loadrgb *Palette32,
				     struct loadrgb *WorkPal,
				     struct loadrgb *UndoPal);

static void Copy (struct Window *Win, uint32 PenNumber, struct RGB *RGBBuffer);
static void Paste (struct Window *Win, const uint32 PenNumber,
		   struct RGB *RGBBuffer, struct loadrgb *Palette);
static void Swap (struct Window *Win, const uint32 Pen_1, const uint32 Pen_2,
		  struct loadrgb *Palette);
static int16 Spread (struct Window *Win, const uint32 Pen_1, const uint32 Pen_2,
		    struct loadrgb *Palette);
static void InvertPalette (struct Window *Win, uint32 StartPen, uint32 EndPen,
			   struct loadrgb *Palette);
static void Shl (struct Window *Win, uint32 StartPen, uint32 EndPen,
		 struct loadrgb *Palette);
static void Shr (struct Window *Win, uint32 StartPen, uint32 EndPen,
		 struct loadrgb *Palette);
static void UpdateSliders (uint32 Pen, struct loadrgb *Palette);

/* vars */
static uint32 RedLevel, GreenLevel, BlueLevel;
static uint32 ActPen, OldPen, NewPen, NumColors;
static struct RGB UndoRGB = {0}, ClipRGB = {0}, ActRGB = {0};

static int16 Do_Copy, Do_Swap, Do_Spread;
#endif

#ifdef FM_REACT_SUPPORT
/*  Do_PalettePrefRequest(): palette-requester (full handling).

        SYNOPSIS: int16 res = Do_PalettePrefWd
                        (
                        struct Window     *Win;
                        char                  *PubScreenName;
                        int16                    Left;
                        int16                    Top;
                        struct loadrgb    *Palette32;
                        );

        INPUTS:     Win:
                            Pointer to the parent-window.

                        PubScreenName:
                            name of the public-screen, where the window should appear.

                        Left:
                            Left edge of palette-window (not supported yet).

                        Top:
                            Top edge of palette-window (not supported yet).

                        Palette32:
                            pointer to a loadrgb-structure with palette to edit.

        RETURNS:        res:
                            Is TRUE, if something changed.
*/

int16
Do_PalettePrefRequest (struct Window *Win, char *PubScreenName, int16 Left,
		       int16 Top, struct loadrgb *Palette32)
{
  int16 rc = FALSE;
  uint32 result = RESULT_NOTHING, waitsigs, mask;
  uint32 PalLength;
  struct loadrgb *WorkPal = NULL;
  struct loadrgb *UndoPal = NULL;
  struct RGB (*WorkPalColors)[1] = NULL;
  struct RGB (*UndoPalColors)[1] = NULL;
  struct loadrgb *WorkPalRGB = NULL;
  struct DrawInfo *dri = NULL;

  Do_Copy = FALSE;
  Do_Swap = FALSE;
  Do_Spread = FALSE;

  if (dri = GetScreenDrawInfo (Win->WScreen))
    {
      NumColors = 1L << dri->dri_Depth;
	  
//	  Printf ("colors = %d\n",NumColors);
	  
      if (OpenPalettePrefWindow (PubScreenName) == TRUE)
	{
	  waitsigs = pal_rwd->RWD_WSignals | SIGBREAKF_CTRL_C;
	  PalLength =
	    sizeof (struct loadrgb_head) +
	    ((NumColors + 1) * sizeof (struct RGB));
	  if (WorkPal =
	      (struct loadrgb *) AllocVec (PalLength, MEMF_PUBLIC | MEMF_CLEAR))
	    {
	      WorkPal->loadrgb_lrgb.lrgb_count = NumColors;
	      WorkPal->loadrgb_lrgb.lrgb_first = 0;
	      WorkPalColors =
		(struct RGB (*)[1]) (((uint32) * (&WorkPal)) +
				     sizeof (struct loadrgb_head));
	      if (UndoPal =
		  (struct loadrgb *) AllocVec (PalLength,
					       MEMF_PUBLIC | MEMF_CLEAR))
		{
		  UndoPal->loadrgb_lrgb.lrgb_count = NumColors;
		  UndoPal->loadrgb_lrgb.lrgb_first = 0;
		  UndoPalColors =
		    (struct RGB (*)[1]) (((uint32) * (&UndoPal)) +
					 sizeof (struct loadrgb_head));
		  GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
			    ColorMap, 0L, NumColors, (uint32 *) WorkPalColors);
		  GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
			    ColorMap, 0L, NumColors, (uint32 *) UndoPalColors);

		  UpdateSliders (DEF_PEN, WorkPal);
		  ActPen = DEF_PEN;
		  rc = TRUE;

		  while (result == RESULT_NOTHING)
		    {
		      mask = Wait (waitsigs);
#ifndef NDEBUG
		      Printf ("mask: %lX\n", mask);
#endif /* !NDEBUG */
		      if (mask & pal_rwd->RWD_WSignals)
			{
			  result =
			    HandlePalettePrefWindow (Win, Palette32, WorkPal,
						     UndoPal);
			}
		      if (mask & SIGBREAKF_CTRL_C)
			{
			  LoadRGB32 (ViewPortAddress (Win), (APTR) Palette32);
			  result = RESULT_CANCEL;
			}
		    }
		  FreeVec (UndoPal);
		}
	      else
		DisplayError (Win, TXT_ERR_NoMem, 5L);
	      FreeVec (WorkPal);
	    }
	  else
	    DisplayError (Win, TXT_ERR_NoMem, 5L);
	  ClosePalettePrefWindow ();
	}
      else
	DisplayError (Win, TXT_ERR_Window, 5L);
      FreeScreenDrawInfo (Win->WScreen, dri);
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  OpenPalettePrefWindow(): open window of palette-requester.

        SYNOPSIS: int16 res = OpenPalettePrefWindow
                        (
                        char    *PubScreenName;
                        );

        INPUTS:     PubScreenName:
                            name of the public-screen, where the window should appear.

        RETURNS:        res:
                            TRUE, if successfull.
*/

static int16
OpenPalettePrefWindow (char *PubScreenName)
{
#ifdef INTERCONNECTION
  struct TagItem MapSlider2Integer[] = {
    SLIDER_Level, INTEGER_Number,
    TAG_DONE
  };
  struct TagItem MapInteger2Slider[] = {
    INTEGER_Number, SLIDER_Level,
    TAG_DONE
  };
#endif /* INTERCONNECTION */

  pal_rwd =
    (struct ReactWinData *) AllocVec (sizeof (struct ReactWinData),
				      MEMF_PUBLIC | MEMF_CLEAR);

  if (pal_rwd)
    {
	if (PaletteBase =  (struct Library *) OpenLibrary ("gadgets/palette.gadget", 51L))
	{
	
      if (!(palette_gad = NewObject (NULL, "palette.gadget", 
	  								 GA_ID, Pal_PALETTE_1, 
	                                 GA_RelVerify, TRUE, 
									 PALETTE_Color, DEF_PEN, 
									 PALETTE_ColorOffset, 0, 
									 PALETTE_NumColors, NumColors, 
									 TAG_DONE)))
		Printf ("palette nok! Colors = %d\n",NumColors);
	}
      if (!(integer_red_gad = NewObject (NULL, "integer.gadget",
					 GA_ID, Pal_INTEGER_RedValue,
					 GA_RelVerify, TRUE,
					 GA_TabCycle, TRUE,
					 INTEGER_Number, 0,
					 INTEGER_Minimum, 0,
					 INTEGER_Maximum, 255,
					 INTEGER_MinVisible, 3,
					 INTEGER_Arrows, TRUE,
					 INTEGER_MaxChars,4,
#ifdef INTERCONNECTION
					 ICA_TARGET, slider_red_gad,
					 ICA_MAP, MapInteger2Slider,
#endif /* INTERCONNECTION */
					 TAG_DONE)))
	PutStr ("integer red gad nok!\n");

      if (!(integer_green_gad = NewObject (NULL, "integer.gadget",
					   GA_ID, Pal_INTEGER_GreenValue,
					   GA_RelVerify, TRUE,
					   GA_TabCycle, TRUE,
					   INTEGER_Number, 0,
					   INTEGER_Minimum, 0,
					   INTEGER_Maximum, 255,
					   INTEGER_MinVisible, 3,
					   INTEGER_Arrows, TRUE,
                       INTEGER_MaxChars,4,
#ifdef INTERCONNECTION
					   ICA_TARGET, slider_green_gad,
					   ICA_MAP, MapInteger2Slider,
#endif /* INTERCONNECTION */
					   TAG_DONE)))
	PutStr ("integer green gad nok!\n");

      if (!(integer_blue_gad = NewObject (NULL, "integer.gadget",
					  GA_ID, Pal_INTEGER_BlueValue,
					  GA_RelVerify, TRUE,
					  GA_TabCycle, TRUE,
					  INTEGER_Number, 0,
					  INTEGER_Minimum, 0,
					  INTEGER_Maximum, 255,
					  INTEGER_MinVisible, 3,
					  INTEGER_Arrows, TRUE,
                      INTEGER_MaxChars,4,
#ifdef INTERCONNECTION
					  ICA_TARGET, slider_blue_gad,
					  ICA_MAP, MapInteger2Slider,
#endif /* INTERCONNECTION */
					  TAG_DONE)))
	PutStr ("integer blue gad nok!\n");

      if (!(slider_red_gad = NewObject (NULL, "slider.gadget",	/* Pal_SLIDER_Red */
					GA_ID, Pal_SLIDER_Red,
					GA_RelVerify, TRUE,
					SLIDER_Min, 0,
					SLIDER_Max, 255,
					SLIDER_Level, 0, 
					SLIDER_Orientation, SORIENT_HORIZ,
					SLIDER_Ticks,255/4,
					SLIDER_ShortTicks,TRUE,
#ifdef INTERCONNECTION
					ICA_TARGET, integer_red_gad,
					ICA_MAP, MapSlider2Integer,
#endif /* INTERCONNECTION */
					TAG_DONE)))
	PutStr ("slider red gad nok!\n");

      if (!(slider_green_gad = NewObject (NULL, "slider.gadget",	/* Pal_SLIDER_Green */
					  GA_ID, Pal_SLIDER_Green,
					  GA_RelVerify, TRUE,
					  SLIDER_Min, 0,
					  SLIDER_Max, 255,
					  SLIDER_Level, 0, 
					  SLIDER_Orientation, SORIENT_HORIZ,
					  SLIDER_Ticks,255/4,
					  SLIDER_ShortTicks,TRUE,

#ifdef INTERCONNECTION
					  ICA_TARGET, integer_green_gad,
					  ICA_MAP, MapSlider2Integer,
#endif /* INTERCONNECTION */
					  TAG_DONE)))
	PutStr ("slider green gad nok!\n");

      if (!(slider_blue_gad = NewObject (NULL, "slider.gadget",	/* Pal_SLIDER_Blue */
					 GA_ID, Pal_SLIDER_Blue,
					 GA_RelVerify, TRUE,
					 SLIDER_Min, 0,
					 SLIDER_Max, 255,
					 SLIDER_Level, 0, 
					 SLIDER_Orientation, SORIENT_HORIZ,
					 SLIDER_Ticks,255/4,
   					 SLIDER_ShortTicks,TRUE,					 
#ifdef INTERCONNECTION
					 ICA_TARGET, integer_blue_gad,
					 ICA_MAP, MapSlider2Integer,
#endif /* INTERCONNECTION */
					 TAG_DONE)))
	PutStr ("slider blue gad nok!\n");

      pal_rwd->RWD_WindowObject = NewObject (NULL, "window.class",
					     WA_Title,
					     CATSTR (TITLE_PaletteReq),
					     WA_IDCMP, IDCMP_RAWKEY,
					     //WA_NoCareRefresh, TRUE,
					     WA_CloseGadget, TRUE, WA_DepthGadget, TRUE, WA_SizeGadget, TRUE, WA_DragBar, TRUE, WA_Activate, TRUE, WA_PubScreenName, PubScreenName, WA_PubScreenFallBack, TRUE, WA_SmartRefresh, TRUE, WINDOW_Position, WPOS_CENTERSCREEN, WINDOW_GadgetHelp, FALSE, WINDOW_IconifyGadget, FALSE, WINDOW_SharedPort, IDCMPortPtr, WINDOW_AppPort, AppPortPtr, WINDOW_Layout, NewObject (NULL, "layout.gadget",	/* Pal_VERT_ROOT */
	LAYOUT_Orientation,
	LAYOUT_ORIENT_VERT,
	LAYOUT_SpaceInner,
	TRUE,
	LAYOUT_SpaceOuter,
	TRUE,
	//LAYOUT_BevelStyle, BVS_THIN,
	//LAYOUT_DeferLayout, TRUE,
	LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_HORIZ_1 */
	LAYOUT_Orientation,
	LAYOUT_ORIENT_HORIZ,
	//LAYOUT_BevelState, IDS_SELECTED,
	//LAYOUT_BevelStyle, BVS_FIELD,
	//LAYOUT_SpaceInner, TRUE,
	//LAYOUT_SpaceOuter, TRUE,
	//LAYOUT_HorizAlignment, LALIGN_LEFT,
	//LAYOUT_VertAlignment, LALIGN_CENTER,
	LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_VERT_1_1 */
	LAYOUT_Orientation,
	LAYOUT_ORIENT_VERT,
	//LAYOUT_BevelState, IDS_NORMAL,
	LAYOUT_BevelStyle, BVS_FIELD,	//BVS_GROUP,
LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, palette_gad, CHILD_MinWidth, 200, CHILD_MinHeight, 200, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_HORIZ_1_1_1 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_VERT_1_1_1_2 */
LAYOUT_Orientation, LAYOUT_ORIENT_VERT, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_HORIZ_1_1_1_2_1 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_VertAlignment, LAYOUT_ALIGN_CENTER, LAYOUT_AddChild, slider_red_gad, CHILD_WeightedHeight, 0, CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Pal_TXT_R), LABEL_Justification, LJ_LEFT, TAG_DONE), LAYOUT_AddChild, integer_red_gad, TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_HORIZ_1_1_1_2_2 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_VertAlignment, LAYOUT_ALIGN_CENTER, LAYOUT_AddChild, slider_green_gad, CHILD_WeightedHeight, 0, CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Pal_TXT_G), LABEL_Justification, LJ_LEFT, TAG_DONE), LAYOUT_AddChild, integer_green_gad, TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_HORIZ_1_1_1_2_3 */
LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ, LAYOUT_VertAlignment, LAYOUT_ALIGN_CENTER, LAYOUT_AddChild, slider_blue_gad, CHILD_WeightedHeight, 0, CHILD_Label, NewObject (NULL, "label.image", LABEL_Text, CATSTR (Pal_TXT_B), LABEL_Justification, LJ_LEFT, TAG_DONE), LAYOUT_AddChild, integer_blue_gad, TAG_DONE), TAG_DONE), TAG_DONE), CHILD_WeightedHeight, 0, TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_VERT_1_2 */
LAYOUT_Orientation,
LAYOUT_ORIENT_VERT,
//LAYOUT_BevelState, IDS_NORMAL,
LAYOUT_BevelStyle, BVS_FIELD,	//BVS_GROUP,
LAYOUT_SpaceInner, TRUE, LAYOUT_SpaceOuter, TRUE, LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_Copy */
GA_ID,
Pal_BUTTON_Copy,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Copy),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_Swap */
GA_ID,
Pal_BUTTON_Swap,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Swap),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_Spread */
GA_ID,
Pal_BUTTON_Spread,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Spread),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_Invert */
GA_ID,
Pal_BUTTON_Invert,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Invert),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_ShiftLeft */
GA_ID,
Pal_BUTTON_ShiftLeft,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_ShiftLeft),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_Shift_Right */
GA_ID,
Pal_BUTTON_ShiftRight,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_ShiftRight),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "button.gadget",	/* Pal_BUTTON_Undo */
GA_ID,
Pal_BUTTON_Undo,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Undo),
BUTTON_Justification,
BCJ_CENTER,
CHILD_WeightedHeight,
0,
//CHILD_WeightedWidth, 0,
TAG_DONE), TAG_DONE), CHILD_WeightedWidth, 0, TAG_DONE), LAYOUT_AddChild, NewObject (NULL, "space.gadget",	/* Pal_SPACE_2 */
//SPACE_MinWidth, 10,
//SPACE_MinHeight, 10,
TAG_DONE), CHILD_WeightedHeight, 0, LAYOUT_AddChild, NewObject (NULL, "layout.gadget",	/* Pal_BUTTONLAYOUT_3 */
LAYOUT_Orientation,
LAYOUT_ORIENT_HORIZ,
LAYOUT_HorizAlignment,
LALIGN_CENTER,
LAYOUT_EvenSize,
TRUE,
//LAYOUT_SpaceInner, TRUE,
//LAYOUT_SpaceOuter, TRUE,
LAYOUT_AddChild,
NewObject
(NULL,
"button.gadget",
GA_ID,
Pal_BUTTON_Accept,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Accept),
BUTTON_Justification,
BCJ_CENTER,
TAG_DONE),
CHILD_WeightedWidth,
33,
CHILD_WeightedHeight,
0,
LAYOUT_AddChild,
NewObject
(NULL,
"button.gadget",
GA_ID,
Pal_BUTTON_Reset,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Reset),
BUTTON_Justification,
BCJ_CENTER,
TAG_DONE),
CHILD_WeightedWidth,
33,
CHILD_WeightedHeight,
0,
LAYOUT_AddChild,
NewObject
(NULL,
"button.gadget",
GA_ID,
Pal_BUTTON_Cancel,
GA_RelVerify,
TRUE,
GA_Text,
CATSTR
(Pal_TXT_Cancel),
BUTTON_Justification,
BCJ_CENTER,
TAG_DONE),
CHILD_WeightedWidth,
33,
CHILD_WeightedHeight,
0,
TAG_DONE),
	CHILD_WeightedHeight,
	0,
	LAYOUT_AddChild,
	NewObject
	(NULL,
	 "space.gadget",
	 //SPACE_MinWidth, 10,
	 //SPACE_MinHeight, 10,
	 TAG_DONE),
	CHILD_WeightedHeight,
	0,
	TAG_DONE),
					     TAG_DONE);

      if (pal_rwd->RWD_WindowObject)
	{

	  if (DoMethod (pal_rwd->RWD_WindowObject, WM_OPEN) != NULL)
	    {
	      GetAttr (WINDOW_Window, pal_rwd->RWD_WindowObject,
		       (uint32 *) & pal_rwd->RWD_IWindow);
	      GetAttr (WINDOW_SigMask, pal_rwd->RWD_WindowObject,
		       &pal_rwd->RWD_WSignals);
	      return (TRUE);
	    }
	}
    }
  return (FALSE);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  ClosePalettePrefWindow(): close window of palette-requester.

        SYNOPSIS: void = ClosePalettePrefWindow
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        res:
                            result of the request.
*/

static void
ClosePalettePrefWindow (void)
{
  if (pal_rwd)
    {
      if (pal_rwd->RWD_WindowObject)
	{
	  DisposeObject (pal_rwd->RWD_WindowObject);
	}
      FreeVec (pal_rwd);
      pal_rwd = NULL;
    }
	
  if (PaletteBase) CloseLibrary ((struct Library *) PaletteBase);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  HandlePalettePrefWindow(): Handle the messages of the palette-requester.

        SYNOPSIS: int32 = HandlePalettePrefWindow
                        (
                        struct Window   *Win;
                        struct loadrgb  *Palette32;
                        struct loadrgb  *WorkPal;
                        struct loadrgb  *UndoPal;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        Palette32:
                            Pointer to a loadrgb-structure of the Palette to edit.

                        WorkPal;
                            Pointer to a loadrgb-structure of the Work-Palette.

                        UndoPal:
                            Pointer to a loadrgb-structure of the Undo-Palette.

        RETURNS:        res:
                            result of messages: RESULT_NOTHING -> nothing happens.
                                                        RESULT_CANCEL  -> user wants to quit without a change.
                                                        RESULT_OK      -> accept changes and quit.
*/

static int32
HandlePalettePrefWindow (struct Window *Win, struct loadrgb *Palette32,
			 struct loadrgb *WorkPal, struct loadrgb *UndoPal)
{
  uint32 code, result, rc = RESULT_NOTHING;
  struct RGB (*Palette32Colors)[1] =
    (struct RGB (*)[1]) (((uint32) * (&Palette32)) +
			 sizeof (struct loadrgb_head));
  struct RGB (*WorkPalColors)[1] =
    (struct RGB (*)[1]) (((uint32) * (&WorkPal)) + sizeof (struct loadrgb_head));
  struct RGB (*UndoPalColors)[1] =
    (struct RGB (*)[1]) (((uint32) * (&UndoPal)) + sizeof (struct loadrgb_head));

  while ((result =
	  DoMethod (pal_rwd->RWD_WindowObject, WM_HANDLEINPUT,
		    &code)) != WMHI_LASTMSG)
    {

#ifndef NDEBUG
      PutStr ("result: %lX\n", result);
#endif /* !NDEBUG */

      switch (result & WMHI_CLASSMASK)
	{
	case WMHI_CLOSEWINDOW:
	  LoadRGB32 (ViewPortAddress (Win), (APTR) Palette32);
	  rc = RESULT_CANCEL;
	  break;

	case WMHI_GADGETUP:
	  switch (result & RL_GADGETMASK)
	    {
	    case Pal_BUTTON_Accept:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) Palette32Colors);
	      rc = RESULT_OK;
	      break;
	    case Pal_BUTTON_Reset:
	      LoadRGB32 (ViewPortAddress (Win), (APTR) Palette32);
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) WorkPalColors);
	      UpdateSliders (ActPen, WorkPal);
	      break;
	    case Pal_BUTTON_Cancel:
	      LoadRGB32 (ViewPortAddress (Win), (APTR) Palette32);
	      rc = RESULT_CANCEL;
	      break;
	    case Pal_BUTTON_Copy:
	      Copy (Win, ActPen, &ClipRGB);
	      Do_Copy = TRUE;
	      Do_Swap = FALSE;
	      Do_Spread = FALSE;
	      break;
	    case Pal_BUTTON_Swap:
	      OldPen = ActPen;
	      Do_Copy = FALSE;
	      Do_Swap = TRUE;
	      Do_Spread = FALSE;
	      break;
	    case Pal_BUTTON_Spread:
	      OldPen = ActPen;
	      Do_Copy = FALSE;
	      Do_Swap = FALSE;
	      Do_Spread = TRUE;
	      break;
	    case Pal_BUTTON_Invert:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);
	      InvertPalette (Win, STARTPEN, NumColors - 1, WorkPal);
	      UpdateSliders (ActPen, WorkPal);
	      break;
	    case Pal_BUTTON_ShiftLeft:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);
	      Shl (Win, STARTPEN, ENDPEN, WorkPal);
	      break;
	    case Pal_BUTTON_ShiftRight:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);
	      Shr (Win, STARTPEN, ENDPEN, WorkPal);
	      break;
	    case Pal_BUTTON_Undo:
	      LoadRGB32 (ViewPortAddress (Win), (APTR) UndoPal);
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) WorkPalColors);
	      UpdateSliders (ActPen, WorkPal);
	      break;
	    case Pal_PALETTE_1:
	      GetAttr (PALETTE_Colour, palette_gad, &ActPen);
	      if (Do_Copy == TRUE)
		{
		  GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
			    ColorMap, 0L, NumColors, (uint32 *) UndoPalColors);
		  Paste (Win, ActPen, &ClipRGB, WorkPal);
		  Do_Copy = FALSE;
		}

	      if (Do_Swap == TRUE)
		{
		  GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
			    ColorMap, 0L, NumColors, (uint32 *) UndoPalColors);
		  NewPen = ActPen;
		  Swap (Win, OldPen, NewPen, WorkPal);
		  Do_Swap = FALSE;
		}

	      if (Do_Spread == TRUE)
		{
		  GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
			    ColorMap, 0L, NumColors, (uint32 *) UndoPalColors);
		  NewPen = ActPen;
		  Spread (Win, OldPen, NewPen, WorkPal);
		  Do_Spread = FALSE;
		}

	      UpdateSliders (ActPen, WorkPal);
	      break;
	    case Pal_INTEGER_RedValue:
#ifndef INTERCONNECTION
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);
	      GetAttr (INTEGER_Number, integer_red_gad, &RedLevel);
	      SetGadgetAttrs (slider_red_gad, pal_rwd->RWD_IWindow, NULL,
			      SLIDER_Level, RedLevel, TAG_DONE);
	      WorkPalColors[ActPen]->RGB_Red = RedLevel << 24L;;
	      LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
	      break;
#endif /* INTERCONNECTION */
	    case Pal_SLIDER_Red:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);
	      GetAttr (SLIDER_Level, slider_red_gad, &RedLevel);
#ifndef INTERCONNECTION
	      SetGadgetAttrs (integer_red_gad, pal_rwd->RWD_IWindow, NULL,
			      INTEGER_Number, RedLevel, TAG_DONE);
#endif /* INTERCONNECTION */
	      WorkPalColors[ActPen]->RGB_Red = RedLevel << 24L;;
	      LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
	      break;
	    case Pal_INTEGER_GreenValue:
#ifndef INTERCONNECTION
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);

	      GetAttr (INTEGER_Number, integer_green_gad, &GreenLevel);
	      SetGadgetAttrs (slider_green_gad, pal_rwd->RWD_IWindow, NULL,
			      SLIDER_Level, GreenLevel, TAG_DONE);

	      WorkPalColors[ActPen]->RGB_Green = GreenLevel << 24L;;
	      LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
	      break;
#endif /* INTERCONNECTION */
	    case Pal_SLIDER_Green:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);

	      GetAttr (SLIDER_Level, slider_green_gad, &GreenLevel);
#ifndef INTERCONNECTION
	      SetGadgetAttrs (integer_green_gad, pal_rwd->RWD_IWindow, NULL,
			      INTEGER_Number, GreenLevel, TAG_DONE);
#endif /* INTERCONNECTION */
	      WorkPalColors[ActPen]->RGB_Green = GreenLevel << 24L;;
	      LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
	      break;
	    case Pal_INTEGER_BlueValue:
#ifndef INTERCONNECTION
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);

	      GetAttr (INTEGER_Number, integer_blue_gad, &BlueLevel);
	      SetGadgetAttrs (slider_blue_gad, pal_rwd->RWD_IWindow, NULL,
			      SLIDER_Level, BlueLevel, TAG_DONE);
				  
	      WorkPalColors[ActPen]->RGB_Blue = BlueLevel << 24L;;
	      LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
	      break;
#endif /* INTERCONNECTION */
	    case Pal_SLIDER_Blue:
	      GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			0L, NumColors, (uint32 *) UndoPalColors);

	      GetAttr (SLIDER_Level, slider_blue_gad, &BlueLevel);
#ifndef INTERCONNECTION
	      SetGadgetAttrs (integer_blue_gad, pal_rwd->RWD_IWindow, NULL,
			      INTEGER_Number, BlueLevel, TAG_DONE);
#endif /* INTERCONNECTION */
	      WorkPalColors[ActPen]->RGB_Blue = BlueLevel << 24L;;
	      LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
	      break;
#ifndef NDEBUG
	    default:
	      Printf ("unknown result: %lX\nunknown gadget: %lX\n", result, result & WMHI_GADGETMASK);
//                  DisplayBeep(NULL);
	      break;
#endif /* !NDEBUG */
	    }
	  break;

	case WMHI_ICONIFY:
	  DoMethod (pal_rwd->RWD_WindowObject, WM_ICONIFY);
	  GetAttr (WINDOW_Window, pal_rwd->RWD_WindowObject,
		   (uint32 *) & pal_rwd->RWD_IWindow);
	  break;

	case WMHI_UNICONIFY:
	  DoMethod (pal_rwd->RWD_WindowObject, WM_OPEN);
	  GetAttr (WINDOW_Window, pal_rwd->RWD_WindowObject,
		   (uint32 *) & pal_rwd->RWD_IWindow);
	  break;

	case WMHI_RAWKEY:
	  {
	    struct InputEvent *ie = NULL;
	    GetAttr (WINDOW_InputEvent, pal_rwd->RWD_WindowObject,
		     (uint32 *) & ie);
#ifndef NDEBUG
	    Printf ("Key: %ld, Qualifier: %ld\n", result & WMHI_KEYMASK, ie->ie_Qualifier);
#endif
	    switch (result & WMHI_KEYMASK)
	      {
	      case 0x44:	/* Return */
	      case 0x43:	/* Enter */
		GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap,
			  0L, NumColors, (uint32 *) Palette32Colors);
		rc = RESULT_OK;
		break;
	      case 69:		/* esc */
		LoadRGB32 (ViewPortAddress (Win), (APTR) Palette32);
		rc = RESULT_CANCEL;
		break;
	      case 18:		/* e */
		if (ie->ie_Qualifier & SHIFT_QUAL)
		  {		/* red - delta */
		    RedLevel = WorkPalColors[ActPen]->RGB_Red >> 24L;
		    if (RedLevel > MINVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			RedLevel =
			  (RedLevel <
			   (MINVALUE + DELTA)) ? MINVALUE : RedLevel - DELTA;

			SetGadgetAttrs (slider_red_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, RedLevel, TAG_DONE);

			WorkPalColors[ActPen]->RGB_Red = RedLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		else
		  {		/* red - 1 */
		    RedLevel = WorkPalColors[ActPen]->RGB_Red >> 24L;
		    if (RedLevel > MINVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			RedLevel =
			  (RedLevel < (MINVALUE + 1)) ? MINVALUE : RedLevel - 1;

			SetGadgetAttrs (slider_red_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, RedLevel, TAG_DONE);

			WorkPalColors[ActPen]->RGB_Red = RedLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		break;
	      case 20:		/* t */
		if (ie->ie_Qualifier & SHIFT_QUAL)
		  {		/* red + delta */
		    RedLevel = WorkPalColors[ActPen]->RGB_Red >> 24L;
		    if (RedLevel < MAXVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			RedLevel =
			  (RedLevel >
			   (MAXVALUE - DELTA)) ? MAXVALUE : RedLevel + DELTA;

			SetGadgetAttrs (slider_red_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, RedLevel, TAG_DONE);

			WorkPalColors[ActPen]->RGB_Red = RedLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		else
		  {		/* red + 1 */
		    RedLevel = WorkPalColors[ActPen]->RGB_Red >> 24L;
		    if (RedLevel < MAXVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			RedLevel =
			  (RedLevel > (MAXVALUE - 1)) ? MAXVALUE : RedLevel + 1;

			SetGadgetAttrs (slider_red_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, RedLevel, TAG_DONE);
			WorkPalColors[ActPen]->RGB_Red = RedLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		break;
	      case 35:		/* f */
		if (ie->ie_Qualifier & SHIFT_QUAL)
		  {		/* green - delta */
		    GreenLevel = WorkPalColors[ActPen]->RGB_Green >> 24L;
		    if (GreenLevel > MINVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			GreenLevel =
			  (GreenLevel <
			   (MINVALUE + DELTA)) ? MINVALUE : GreenLevel - DELTA;

			SetGadgetAttrs (slider_green_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, GreenLevel,
					TAG_DONE);
			WorkPalColors[ActPen]->RGB_Green = GreenLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		else
		  {		/* green - 1 */
		    if (GreenLevel > MINVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			GreenLevel =
			  (GreenLevel <
			   (MINVALUE + 1)) ? MINVALUE : GreenLevel - 1;

			SetGadgetAttrs (slider_green_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, GreenLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Green = GreenLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		break;
	      case 37:		/* h */
		if (ie->ie_Qualifier & SHIFT_QUAL)
		  {		/* green + delta */
		    GreenLevel = WorkPalColors[ActPen]->RGB_Green >> 24L;
		    if (GreenLevel < MAXVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			GreenLevel =
			  (GreenLevel >
			   (MAXVALUE - DELTA)) ? MAXVALUE : GreenLevel + DELTA;

			SetGadgetAttrs (slider_green_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, GreenLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Green = GreenLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		else
		  {		/* green + 1 */
		    GreenLevel = WorkPalColors[ActPen]->RGB_Green >> 24L;
		    if (GreenLevel < MAXVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			GreenLevel =
			  (GreenLevel >
			   (MAXVALUE - 1)) ? MAXVALUE : GreenLevel + 1;

			SetGadgetAttrs (slider_green_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, GreenLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Green = GreenLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		break;
	      case 52:		/* v */
		if (ie->ie_Qualifier & SHIFT_QUAL)
		  {		/* blue - delta */
		    BlueLevel = WorkPalColors[ActPen]->RGB_Blue >> 24L;
		    if (BlueLevel > MINVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			BlueLevel =
			  (BlueLevel <
			   (MINVALUE + DELTA)) ? MINVALUE : BlueLevel - DELTA;

			SetGadgetAttrs (slider_blue_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, BlueLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Blue = BlueLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		else
		  {		/* blue - 1 */
		    if (BlueLevel > MINVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			BlueLevel =
			  (BlueLevel <
			   (MINVALUE + 1)) ? MINVALUE : BlueLevel - 1;

			SetGadgetAttrs (slider_blue_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, BlueLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Blue = BlueLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		break;
	      case 54:		/* n */
		if (ie->ie_Qualifier & SHIFT_QUAL)
		  {		/* blue + delta */
		    BlueLevel = WorkPalColors[ActPen]->RGB_Blue >> 24L;
		    if (BlueLevel < MAXVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			BlueLevel =
			  (BlueLevel >
			   (MAXVALUE - DELTA)) ? MAXVALUE : BlueLevel + DELTA;

			SetGadgetAttrs (slider_blue_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, BlueLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Blue = BlueLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		else
		  {		/* blue + 1 */
		    BlueLevel = WorkPalColors[ActPen]->RGB_Blue >> 24L;
		    if (BlueLevel < MAXVALUE)
		      {
			GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->
				  ColorMap, 0L, NumColors,
				  (uint32 *) UndoPalColors);
			BlueLevel =
			  (BlueLevel >
			   (MAXVALUE - 1)) ? MAXVALUE : BlueLevel + 1;

			SetGadgetAttrs (slider_blue_gad, pal_rwd->RWD_IWindow,
					NULL, SLIDER_Level, BlueLevel,
					TAG_DONE);

			WorkPalColors[ActPen]->RGB_Blue = BlueLevel << 24L;
			LoadRGB32 (ViewPortAddress (Win), (APTR) WorkPal);
		      }
		  }
		break;
	      }
	    break;
	  }

#ifndef NDEBUG
	default:
	  Printf ("unknown result: %lX\nunknown class: %lX\n", result, result & WMHI_CLASSMASK);
//          DisplayBeep(NULL);
	  break;
#endif /* !NDEBUG */
	}
    }
  return (rc);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/* Copy: Copy the color-values of an desired pen to a RGB-structure.

        SYNOPSIS: static void = Copy
                        (
                        struct Window *Win;
                        uint32               PenNumber;
                        struct RGB    *RGBColor;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        PenNumber:
                            Number of the pen to copy.

                        RGBColor:
                            Pointer to a RGB-structure to store th color-values.

        RETURNS:        - 
*/

static void
Copy (struct Window *Win, uint32 PenNumber, struct RGB *RGBColor)
{
  GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, PenNumber,
	    1L, (uint32 *) RGBColor);
}

/* Paste: store the color-values of a RGB-structure to a pen of a palette.

        SYNOPSIS: static void = Paste
                        (
                        struct Window     *Win;
                        const uint32         PenNumber;
                        struct RGB        *RGBColor;
                        struct loadrgb    *Palette;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        PenNumber:
                            Number of the pen to copy.

                        RGBColor:
                            Pointer to a RGB-structure, which contains the color-values.

                        Palette:
                            Pointer to a loadrgb-structure to edit.

        RETURNS:        - 
*/
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
static void
Paste (struct Window *Win, const uint32 PenNumber, struct RGB *RGBColor,
       struct loadrgb *Palette)
{
  struct RGB (*PalColors)[1] =
    (struct RGB (*)[1]) (((uint32) * (&Palette)) + sizeof (struct loadrgb_head));

  PalColors[PenNumber]->RGB_Red = RGBColor->RGB_Red;
  PalColors[PenNumber]->RGB_Green = RGBColor->RGB_Green;
  PalColors[PenNumber]->RGB_Blue = RGBColor->RGB_Blue;
  LoadRGB32 (ViewPortAddress (Win), (uint32 *) Palette);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/* Swap: Swap two colors of a palette.

        SYNOPSIS: static void = Swap
                        (
                        struct Window     *Win;
                        const uint32         Pen_1;
                        const uint32         Pen_2;
                        struct loadrgb    *Palette;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        Pen_1:
                            Number of the first pen.

                        Pen_2:
                            Number of the second pen.

                        Palette:
                            Pointer to a loadrgb-structure to edit.

        RETURNS:        - 
*/

static void
Swap (struct Window *Win, const uint32 Pen_1, const uint32 Pen_2,
      struct loadrgb *Palette)
{
  struct RGB TmpRGB1, TmpRGB2;

  Copy (Win, Pen_1, &TmpRGB1);
  Copy (Win, Pen_2, &TmpRGB2);
  Paste (Win, Pen_1, &TmpRGB2, Palette);
  Paste (Win, Pen_2, &TmpRGB1, Palette);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  Spread(): Create a range of colors between two pens.

        SYNOPSIS: static int16 res = Spread
                        (
                        struct Window     *Win;
                        const uint32         OldPen;
                        const uint32         NewPen;
                        struct loadrgb    *Palette;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        OldPen:
                            Number of the old pen.

                        NewPen:
                            Number of the new Pen.

                        Palette:
                            Pointer to a loadrgb-structure of the palette to edit.

        RETURNS:        res: 
                            TRUE, if spread failed.
*/

static int16
Spread (struct Window *Win, const uint32 OldPen, const uint32 NewPen,
	struct loadrgb *Palette)
{
  uint32 *COLOR_RGB = (uint32 *) Palette;
  const uint32 StartPen = MIN (OldPen, NewPen), EndPen = MAX (OldPen, NewPen);
  const uint32 Range = EndPen - StartPen;
  int32 RedStep, GreenStep, BlueStep;
  uint32 Index, RedLevel, GreenLevel, BlueLevel, ColorBase;

  if (Range < 2L)
    return TRUE;
  ColorBase = 3L * StartPen + 1L;
  RedLevel = COLOR_RGB[ColorBase] >> 8L;
  GreenLevel = COLOR_RGB[ColorBase + 1L] >> 8L;
  BlueLevel = COLOR_RGB[ColorBase + 2L] >> 8L;
  ColorBase = 3L * EndPen + 1L;
  RedStep = (int32) (COLOR_RGB[ColorBase] >> 8L);
  GreenStep = (int32) (COLOR_RGB[ColorBase + 1L] >> 8L);
  BlueStep = (int32) (COLOR_RGB[ColorBase + 2L] >> 8L);
  RedStep -= (int32) RedLevel;
  GreenStep -= (int32) GreenLevel;
  BlueStep -= (int32) BlueLevel;
  RedStep /= (int32) Range;
  GreenStep /= (int32) Range;
  BlueStep /= (int32) Range;

  for (Index = (StartPen + 1L); Index < EndPen; Index++)
    {
      RedLevel += RedStep;
      GreenLevel += GreenStep;
      BlueLevel += BlueStep;
      ColorBase = 3L * Index + 1L;
      COLOR_RGB[ColorBase] = RedLevel << 8L;
      COLOR_RGB[ColorBase + 1L] = GreenLevel << 8L;
      COLOR_RGB[ColorBase + 2L] = BlueLevel << 8L;
    }

  LoadRGB32 (ViewPortAddress (Win), (APTR) Palette);

  return (FALSE);
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  InvertPalette(): Invert the colors of a palette.

        SYNOPSIS: static void = InvertPalette
                        (
                        struct Window     *Win;
                        uint32                   StartPen;
                        uint32                   EndPen;
                        struct loadrgb    *Palette;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        StartPen:
                            Number of the first pen.

                        EndPen:
                            Number of the last Pen.

                        Palette:
                            Pointer to a loadrgb-structure of the palette to edit.

        RETURNS:        -
*/

static void
InvertPalette (struct Window *Win, uint32 StartPen, uint32 EndPen,
	       struct loadrgb *Palette)
{
  while (StartPen < EndPen)
    Swap (Win, StartPen++, EndPen--, Palette);
}
#endif /* FM_REACT_SUPPORT */


#ifdef FM_REACT_SUPPORT
/*  Shl(): Shift palette-colors to left.

        SYNOPSIS: static void = Shl
                        (
                        struct Window     *Win;
                        uint32                   StartPen;
                        uint32                   EndPen;
                        struct loadrgb    *Palette;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        StartPen:
                            Number of the first pen.

                        EndPen:
                            Number of the last Pen.

                        Palette:
                            Pointer to a loadrgb-structure of the palette to edit.

        RETURNS:        -
*/

static void
Shl (struct Window *Win, uint32 StartPen, uint32 EndPen, struct loadrgb *Palette)
{
  while (StartPen < EndPen)
    Swap (Win, StartPen, StartPen + 1, Palette), StartPen++;
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  Shr(): Shift palette-colors to right.

        SYNOPSIS: static void = Shr
                        (
                        struct Window     *Win;
                        uint32                   StartPen;
                        uint32                   EndPen;
                        struct loadrgb    *Palette;
                        );

        INPUTS:     Win:
                            Pointer to a window-structure.

                        StartPen:
                            Number of the first pen.

                        EndPen:
                            Number of the last Pen.

                        Palette:
                            Pointer to a loadrgb-structure of the palette to edit.

        RETURNS:        res: -
*/
static void
Shr (struct Window *Win, uint32 StartPen, uint32 EndPen, struct loadrgb *Palette)
{
  while (EndPen > StartPen)
    Swap (Win, EndPen, EndPen - 1, Palette), EndPen--;
}
#endif /* FM_REACT_SUPPORT */

#ifdef FM_REACT_SUPPORT
/*  UpdateSliders(): Update slider-positions and integer-values.

        SYNOPSIS: static void = UpdateSliders
                        (
                        uint32                   Pen;
                        struct loadrgb   *Palette;
                        );

        INPUTS:     Pen:
                            Number of desired pen.

                        Palette:
                            Pointer to a loadrgb-structure of the palette to use.

        RETURNS:        -
*/


static void
UpdateSliders (uint32 Pen, struct loadrgb *Palette)
{
  struct RGB (*PalColors)[1] =
    (struct RGB (*)[1]) (((uint32) * (&Palette)) + sizeof (struct loadrgb_head));

  RedLevel = (PalColors[Pen]->RGB_Red >> 24L);
  GreenLevel = (PalColors[Pen]->RGB_Green >> 24L);
  BlueLevel = (PalColors[Pen]->RGB_Blue >> 24L);

  SetGadgetAttrs (slider_red_gad, pal_rwd->RWD_IWindow, NULL, SLIDER_Level,
		  RedLevel, TAG_DONE);
  SetGadgetAttrs (slider_green_gad, pal_rwd->RWD_IWindow, NULL, SLIDER_Level,
		  GreenLevel, TAG_DONE);
  SetGadgetAttrs (slider_blue_gad, pal_rwd->RWD_IWindow, NULL, SLIDER_Level,
		  BlueLevel, TAG_DONE);

#ifndef INTERCONNECTION
  SetGadgetAttrs (integer_red_gad, pal_rwd->RWD_IWindow, NULL, INTEGER_Number,
		  RedLevel, TAG_DONE);
  SetGadgetAttrs (integer_green_gad, pal_rwd->RWD_IWindow, NULL, INTEGER_Number,
		  GreenLevel, TAG_DONE);
  SetGadgetAttrs (integer_blue_gad, pal_rwd->RWD_IWindow, NULL, INTEGER_Number,
		  BlueLevel, TAG_DONE);
#endif /* INTERCONNECTION */
}
#endif /* FM_REACT_SUPPORT */
