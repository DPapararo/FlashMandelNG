/*******************************************************************************
**
**  Coded by Dino Papararo                 29-Nov-2000
**
**  Modified for locale- and arexx support by Edgar Schwan 23.1.2002
**  Modified for 68K/Mixed-versions 9.9.2003
**  Modified for AmigaOS4/GCC 28.06.2004, Edgar Schwan
**  Modified for different catalogs support 21-02-2010, Dino Papararo
**  removed 68K and fm_catalog_cd.h references 01-06-2018 dpapararo
**  $ver 2.3 various fixes 19-03-2020 dpapararo
*******************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/locale.h>

#include <intuition/gadgetclass.h>

#include "Headers/FlashMandel.h"
// #include "Headers/CompilerSpecific.h"
#include "Headers/FM_ReactionBasics.h"

#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "Headers/FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

extern struct Catalog *CatalogPtr;

#define MIN(a,b)    ((a)<(b)?(a):(b))
#define MAX(a,b)    ((a)>(b)?(a):(b))

#define ACCEPT2       (1L)
#define RESET2        (2L)
#define CANCEL2       (3L)
#define RED          (4L)
#define GREEN        (5L)
#define BLUE         (6L)
#define COPY         (7L)
#define SWAP         (8L)
#define SPREAD       (9L)
#define UNDO         (10L)
#define INVERT       (11L)
#define PALETTE      (12L)
#define SHL          (13L)
#define SHR          (14L)

#define STARTPEN     (4L)
#define ENDPEN       (255L)

#define MINVALUE     (0L)
#define MAXVALUE     (255L)
#define DELTA        (10L)

//int16 ModifyPalette  (struct Window *,int16,int16,uint32 *);
void KeepPalette (struct Window *);
void Copy (struct Window *, uint32);
void Paste (struct Window *, const uint32);
void Swap (struct Window *, const uint32, const uint32);
int16 Spread (struct Window *, const uint32, const uint32);
void InvertPalette (struct Window *, uint32, uint32);
void Shl (struct Window *Win, uint32, uint32);
void Shr (struct Window *Win, uint32, uint32);

uint32 COLOR_RGB[3L * 256L + 2L], UNDO_RGB[3L * 256L + 2L], COPY_RGB[3L];
struct NewGadget BUTTON_GAD, SLIDER_GAD, PALETTE_GAD;

int16 ModifyPalette (struct Window *Win, int16 LeftEdge, int16 TopEdge, uint32 * Palette32)
{
  struct Window *ColorWin;
  struct IntuiMessage *Message;
  struct Gadget *GadList = NULL, *MyButtonGad, *MyPaletteGad;
  struct Gadget *RedSliderGad, *GreenSliderGad, *BlueSliderGad, *MyGad;
  int16 Copy_Msg = FALSE, Swap_Msg = FALSE, Spread_Msg = FALSE, Exit = FALSE;
  uint32 SelectedPen = STARTPEN, OldPen = NULL, NewPen;
  uint16 MyCode, i;
  uint32 MyClass, Colors, ColorBase = 3L * STARTPEN + 1L;
  uint32 RedLevel, GreenLevel, BlueLevel;
  CPTR *VInfo;

  	Colors = 1L << Win->RPort->BitMap->Depth;
  	if (Colors < 2L) return FALSE;
  	VInfo = GetVisualInfo (Win->WScreen, TAG_DONE);
  	if (!VInfo) return FALSE;
  	COLOR_RGB[0L] = UNDO_RGB[0L] = Colors << 16L;
  	COLOR_RGB[3L * Colors + 1L] = UNDO_RGB[3L * Colors + 1L] = NULL;
  	COPY_RGB[0L] = COPY_RGB[1L] = COPY_RGB[2L] = NULL;
  	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, 0L, Colors, &COLOR_RGB[1L]);
  	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, 0L, Colors, &UNDO_RGB[1L]);
  	RedLevel = (COLOR_RGB[ColorBase] >> 24L);
  	GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
  	BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
  	MyButtonGad = CreateContext (&GadList);
  	BUTTON_GAD.ng_VisualInfo = PALETTE_GAD.ng_VisualInfo = SLIDER_GAD.ng_VisualInfo = VInfo;
  	BUTTON_GAD.ng_LeftEdge = MARGIN;
  	BUTTON_GAD.ng_TopEdge = MARGIN / 2;
  	BUTTON_GAD.ng_Width = (MIN_WIDTH - (MARGIN * 4)) / 5;
  	BUTTON_GAD.ng_Height = Win->WScreen->Font->ta_YSize * 3 / 2;
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Copy);
  	BUTTON_GAD.ng_GadgetID = COPY;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_TopEdge += (20 + BUTTON_GAD.ng_Height);
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Swap);
  	BUTTON_GAD.ng_GadgetID = SWAP;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_TopEdge += (20 + BUTTON_GAD.ng_Height);
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Spread);
  	BUTTON_GAD.ng_GadgetID = SPREAD;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_TopEdge += (20 + BUTTON_GAD.ng_Height);
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Invert);
  	BUTTON_GAD.ng_GadgetID = INVERT;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_TopEdge += (20 + BUTTON_GAD.ng_Height);
  	BUTTON_GAD.ng_GadgetText = "_<<";
  	BUTTON_GAD.ng_GadgetID = SHL;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_TopEdge += (20 + BUTTON_GAD.ng_Height);
  	BUTTON_GAD.ng_GadgetText = "_>>";
  	BUTTON_GAD.ng_GadgetID = SHR;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_TopEdge += (20 + BUTTON_GAD.ng_Height);
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Undo);
  	BUTTON_GAD.ng_GadgetID = UNDO;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	PALETTE_GAD.ng_LeftEdge = MARGIN + BUTTON_GAD.ng_LeftEdge + BUTTON_GAD.ng_Width;
  	PALETTE_GAD.ng_TopEdge = MARGIN / 2;
  	PALETTE_GAD.ng_Width = (MIN_WIDTH - BUTTON_GAD.ng_Width - (MARGIN * 5));
  	PALETTE_GAD.ng_Height = BUTTON_GAD.ng_TopEdge;
  	PALETTE_GAD.ng_GadgetID = PALETTE;
  	MyPaletteGad = CreateGadget (PALETTE_KIND, MyButtonGad, &PALETTE_GAD, GTPA_Depth, Win->RPort->BitMap->Depth, GTPA_Color, 4, GTPA_ColorOffset, 0,
		  							GTPA_IndicatorWidth, 10, GTPA_NumColors, (uint16) Colors, TAG_DONE);
  	SLIDER_GAD.ng_LeftEdge = MARGIN;
  	SLIDER_GAD.ng_TopEdge = 40 + PALETTE_GAD.ng_Height;
  	SLIDER_GAD.ng_Width = MIN_WIDTH - (MARGIN * 6);
  	SLIDER_GAD.ng_Height = Win->WScreen->Font->ta_YSize * 2;
  	SLIDER_GAD.ng_GadgetText = CATSTR (Pal_TXT_R);
  	SLIDER_GAD.ng_GadgetID = RED;
  	RedSliderGad = CreateGadget (SLIDER_KIND, MyPaletteGad, &SLIDER_GAD, GA_RelVerify, TRUE, GTSL_Max, MAXVALUE, GTSL_Level, (int16) RedLevel,
		  							GTSL_LevelFormat, "%03ld", GTSL_MaxLevelLen, 4, GTSL_LevelPlace, PLACETEXT_RIGHT, TAG_DONE);
  	SLIDER_GAD.ng_TopEdge += (10 + SLIDER_GAD.ng_Height);
  	SLIDER_GAD.ng_GadgetText = CATSTR (Pal_TXT_G);
  	SLIDER_GAD.ng_GadgetID = GREEN;
  	GreenSliderGad = CreateGadget (SLIDER_KIND, RedSliderGad, &SLIDER_GAD, GA_RelVerify, TRUE, GTSL_Max, MAXVALUE, GTSL_Level, (int16) GreenLevel,
		  							GTSL_LevelFormat, "%03ld", GTSL_MaxLevelLen, 4, GTSL_LevelPlace, PLACETEXT_RIGHT, TAG_DONE);
  	SLIDER_GAD.ng_TopEdge += (10 + SLIDER_GAD.ng_Height);
  	SLIDER_GAD.ng_GadgetText = CATSTR (Pal_TXT_B);
  	SLIDER_GAD.ng_GadgetID = BLUE;
  	BlueSliderGad = CreateGadget (SLIDER_KIND, GreenSliderGad, &SLIDER_GAD, GA_RelVerify, TRUE, GTSL_Max, MAXVALUE, GTSL_Level, (int16) BlueLevel,
		  							GTSL_LevelFormat, "%03ld", GTSL_MaxLevelLen, 4, GTSL_LevelPlace, PLACETEXT_RIGHT, TAG_DONE);
  	BUTTON_GAD.ng_LeftEdge = MARGIN;
  	BUTTON_GAD.ng_Width = (MIN_WIDTH - (MARGIN * 6)) / 3;
  	BUTTON_GAD.ng_TopEdge = 20 + SLIDER_GAD.ng_TopEdge + SLIDER_GAD.ng_Height;
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Accept);
  	BUTTON_GAD.ng_GadgetID = ACCEPT2;
  	MyButtonGad = CreateGadget (BUTTON_KIND, BlueSliderGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);
  	BUTTON_GAD.ng_LeftEdge += (MARGIN + BUTTON_GAD.ng_Width);
  	BUTTON_GAD.ng_GadgetText = "Reset";
  	BUTTON_GAD.ng_GadgetID = RESET2;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, TAG_DONE);
  	BUTTON_GAD.ng_LeftEdge += (MARGIN + BUTTON_GAD.ng_Width);
  	BUTTON_GAD.ng_GadgetText = CATSTR (Pal_TXT_Cancel);
  	BUTTON_GAD.ng_GadgetID = CANCEL2;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTON_GAD, GT_Underscore, '_', TAG_DONE);

  	if (!MyButtonGad)
    {
      	FreeGadgets (GadList);
      	FreeVisualInfo (VInfo);
      	return FALSE;
    }

  	ColorWin = OpenWindowTags (NULL, WA_Left, MARGIN,
			     				WA_Top, MARGIN / 2,
			     				WA_Width, MIN_WIDTH - (MARGIN * 2),
			     				WA_Height, MIN (20 + BUTTON_GAD.ng_TopEdge + SLIDER_GAD.ng_TopEdge + SLIDER_GAD.ng_Height, MIN_HEIGHT - (MARGIN / 2)), 
								WA_Title, CATSTR (TITLE_PaletteReq), 
								WA_SizeGadget, TRUE,
			     				WA_SizeBRight, TRUE, WA_MinWidth, 640,
			     				WA_MinHeight, 480, WA_MaxWidth, 1024, WA_MaxHeight, 768, 
								WA_ScreenTitle, "Modify palette...",
			     				WA_CustomScreen, Win->WScreen, 
								WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_REFRESHWINDOW | IDCMP_VANILLAKEY | IDCMP_GADGETDOWN | BUTTONIDCMP | SLIDERIDCMP | PALETTEIDCMP, 
								WA_Flags, WFLG_ACTIVATE | WFLG_DRAGBAR | WFLG_SIMPLE_REFRESH | WFLG_RMBTRAP | WFLG_GIMMEZEROZERO, 
								WA_Gadgets,	GadList, TAG_DONE);

  	if (!ColorWin)
    {
      	FreeGadgets (GadList);
      	FreeVisualInfo (VInfo);
      	return FALSE;
    }

  	GT_RefreshWindow (ColorWin, NULL);

  	do
    {
      	WaitPort (ColorWin->UserPort);

      	while ((!Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (ColorWin->UserPort)))
		{
	  		MyGad = (struct Gadget *) Message->IAddress;
	  		MyClass = Message->Class;
	  		MyCode = Message->Code;
	  		GT_ReplyIMsg ((struct IntuiMessage *) Message);

	  		switch (MyClass)
	    	{
	    		case IDCMP_REFRESHWINDOW:
	      			GT_BeginRefresh (ColorWin);
	      			GT_EndRefresh (ColorWin, TRUE);
	      		break;

	    		case IDCMP_VANILLAKEY:
	      			switch (MyCode)
					{
						case 'a':	/* Accept */
						case 'A':
		  					GetRGB32 (((struct ViewPort *) ViewPortAddress (ColorWin))->ColorMap, 0L, Colors, &Palette32[1L]);
		  					Exit = TRUE;
		  				break;
						case 'c':	/* Cancel */
						case 'C':
		  					LoadRGB32 (ViewPortAddress (ColorWin), Palette32);
		  					Exit = TRUE;
		  				break;
						case 'o':	/* Copy */
						case 'O':
		  					Copy (ColorWin, SelectedPen);
		  					Copy_Msg = TRUE;
		  					Swap_Msg = FALSE;
		  					Spread_Msg = FALSE;
		  				break;
						case 's':	/* Swap */
						case 'S':
		  					OldPen = SelectedPen;
		  					Copy_Msg = FALSE;
		  					Swap_Msg = TRUE;
		  					Spread_Msg = FALSE;
		  				break;
						case 'r':	/* Spread */
						case 'R':
					  		OldPen = SelectedPen;
		  					Copy_Msg = FALSE;
		  					Swap_Msg = FALSE;
		  					Spread_Msg = TRUE;
		  				break;
						case '<':
		  					KeepPalette (ColorWin);
		  					Shl (ColorWin, STARTPEN, ENDPEN);
		  				break;
						case '>':
		  					KeepPalette (ColorWin);
		  					Shr (ColorWin, STARTPEN, ENDPEN);
		  				break;
						case 'u':
						case 'U':
		  					LoadRGB32 (ViewPortAddress (ColorWin), UNDO_RGB);
		  					GetRGB32 (((struct ViewPort *) ViewPortAddress (ColorWin))->ColorMap, 0L, Colors, &COLOR_RGB[1L]);
		  					RedLevel = (COLOR_RGB[ColorBase] >> 24L);
		  					GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
		  					BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
		  					GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		  					GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		  					GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		  				break;
						case 'E':
							RedLevel = COLOR_RGB[ColorBase] >> 24L;
		  					if (RedLevel > MINVALUE)
		    				{
		      					KeepPalette (Win);
		      					RedLevel = (RedLevel < (MINVALUE + DELTA)) ? MINVALUE : RedLevel - DELTA;
		      					GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		      					COLOR_RGB[ColorBase] = RedLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'e':
		  					RedLevel = COLOR_RGB[ColorBase] >> 24L;
		  					if (RedLevel > MINVALUE)
		    				{
		      					KeepPalette (Win);
		      					RedLevel--;
		      					GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		      					COLOR_RGB[ColorBase] = RedLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'T':
		  					RedLevel = COLOR_RGB[ColorBase] >> 24L;
		  					if (RedLevel < MAXVALUE)
		    				{
		      					KeepPalette (Win);
		      					RedLevel += DELTA;
		      					if (RedLevel > MAXVALUE) RedLevel = MAXVALUE;
		      					GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		      					COLOR_RGB[ColorBase] = RedLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 't':
		  					RedLevel = COLOR_RGB[ColorBase] >> 24L;
		  					if (RedLevel < MAXVALUE)
		    				{
		      					KeepPalette (Win);
		      					RedLevel++;
		      					GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		      					COLOR_RGB[ColorBase] = RedLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'F':
							GreenLevel = COLOR_RGB[ColorBase + 1L] >> 24L;
		  					if (GreenLevel > MINVALUE)
		    				{
		      					KeepPalette (Win);
		      					GreenLevel = (GreenLevel < (MINVALUE + DELTA)) ? MINVALUE : GreenLevel - DELTA;
		      					GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		      					COLOR_RGB[ColorBase + 1L] = GreenLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;

						case 'f':
		  					GreenLevel = COLOR_RGB[ColorBase + 1L] >> 24L;
		  					if (GreenLevel > MINVALUE)
		    				{
		      					KeepPalette (Win);
		      					GreenLevel--;
		      					GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		      					COLOR_RGB[ColorBase + 1L] = GreenLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'H':
		  					GreenLevel = COLOR_RGB[ColorBase + 1L] >> 24L;
		  					if (GreenLevel < MAXVALUE)
		    				{
		      					KeepPalette (Win);
		      					GreenLevel += DELTA;
		      					if (GreenLevel > MAXVALUE) GreenLevel = MAXVALUE;
		      					GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		      					COLOR_RGB[ColorBase + 1L] = GreenLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'h':
		  					GreenLevel = COLOR_RGB[ColorBase + 1L] >> 24L;
		  					if (GreenLevel < MAXVALUE)
		    				{
		      					KeepPalette (Win);
		      					GreenLevel++;
		      					GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		      					COLOR_RGB[ColorBase + 1L] = GreenLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'V':
		  					BlueLevel = COLOR_RGB[ColorBase + 2L] >> 24L;
		  					if (BlueLevel > MINVALUE)
		    				{
		      					KeepPalette (Win);
		      					BlueLevel =	(BlueLevel < (MINVALUE + DELTA)) ? MINVALUE : BlueLevel - DELTA;
		      					GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		      					COLOR_RGB[ColorBase + 2L] = BlueLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'v':
		  					BlueLevel = COLOR_RGB[ColorBase + 2L] >> 24L;
		  					if (BlueLevel > MINVALUE)
		    				{
						      	KeepPalette (Win);
		      					BlueLevel--;
		      					GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		      					COLOR_RGB[ColorBase + 2L] = BlueLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'N':
		  					BlueLevel = COLOR_RGB[ColorBase + 2L] >> 24L;
		  					if (BlueLevel < MAXVALUE)
		    				{
		      					KeepPalette (Win);
		      					BlueLevel += DELTA;
		      					if (BlueLevel > MAXVALUE) BlueLevel = MAXVALUE;
		      					GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		      					COLOR_RGB[ColorBase + 2L] = BlueLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'n':
		  					BlueLevel = COLOR_RGB[ColorBase + 2L] >> 24L;
			  				if (BlueLevel < MAXVALUE)
		    				{
		      					KeepPalette (Win);
		      					BlueLevel++;
		      					GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		      					COLOR_RGB[ColorBase + 2L] = BlueLevel << 24L;
		      					LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		    				}
		  				break;
						case 'I':
						case 'i':
		  					KeepPalette (Win);
		  					InvertPalette (Win, STARTPEN, Colors - 1L);
		  					RedLevel = (COLOR_RGB[ColorBase] >> 24L);
		  					GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
		  					BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
		  					GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		  					GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		  					GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		  					break;
						case '#':
		  					for (i = 1; i <= 256 * 3; i += 3)
		    				{
		      					Printf ("0X%08X,", Palette32[i]);
		      					Printf ("0X%08X,", Palette32[i + 1]);
		      					Printf ("0X%08X,\n", Palette32[i + 2]);
		    				}
		  				break;
					}
	      			break;

	    			case IDCMP_GADGETUP:
	      				switch (MyGad->GadgetID)
						{
							case ACCEPT2:
		  						GetRGB32 (((struct ViewPort *) ViewPortAddress (ColorWin))->ColorMap, 0L, Colors, &Palette32[1L]);
		  						Exit = TRUE;
		  						break;
							case RESET2:
		  						KeepPalette (Win);
		  						LoadRGB32 (ViewPortAddress (ColorWin), Palette32);
		  						GetRGB32 (((struct ViewPort *) ViewPortAddress (ColorWin))->ColorMap, 0L, Colors, &COLOR_RGB[1L]);
		  						RedLevel = (COLOR_RGB[ColorBase] >> 24L);
		  						GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
		  						BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
		  						GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		  						GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		  						GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		  					break;
							case CANCEL2:
		  						LoadRGB32 (ViewPortAddress (ColorWin), Palette32);
		  						Exit = TRUE;
		  					break;
							case COPY:
		  						Copy (ColorWin, SelectedPen);
		  						Copy_Msg = TRUE;
		  						Swap_Msg = FALSE;
		  						Spread_Msg = FALSE;
		  					break;
							case SWAP:
		  						OldPen = SelectedPen;
		  						Copy_Msg = FALSE;
		  						Swap_Msg = TRUE;
		  						Spread_Msg = FALSE;
		  					break;
								case SPREAD:
		  						OldPen = SelectedPen;
		  						Copy_Msg = FALSE;
		  						Swap_Msg = FALSE;
		  						Spread_Msg = TRUE;
		  					break;
							case INVERT:
		  						KeepPalette (Win);
		  						InvertPalette (Win, STARTPEN, Colors - 1L);
		  						RedLevel = (COLOR_RGB[ColorBase] >> 24L);
		  						GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
		  						BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
		  						GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		  						GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		  						GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		  					break;
							case SHL:
		  						KeepPalette (ColorWin);
		  						Shl (ColorWin, STARTPEN, ENDPEN);
		  					break;
							case SHR:
		  						KeepPalette (ColorWin);
		  						Shr (ColorWin, STARTPEN, ENDPEN);
		  					break;
							case UNDO:
		  						LoadRGB32 (ViewPortAddress (ColorWin), UNDO_RGB);
		  						GetRGB32 (((struct ViewPort *) ViewPortAddress (ColorWin))->ColorMap, 0L, Colors, &COLOR_RGB[1L]);
		  						RedLevel = (COLOR_RGB[ColorBase] >> 24L);
		  						GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
		  						BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
		  						GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		  						GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		  						GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		  					break;
							case RED:
		  						KeepPalette (Win);
		  						GT_GetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, &RedLevel);
		  						COLOR_RGB[ColorBase] = RedLevel << 24L;
		  						LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		  					break;
							case GREEN:
		  						KeepPalette (Win);
		  						GT_GetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, &GreenLevel);
		  						COLOR_RGB[ColorBase + 1L] = GreenLevel << 24L;
		  						LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		  					break;
							case BLUE:
		  						KeepPalette (Win);
		  						GT_GetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, &BlueLevel);
		  						COLOR_RGB[ColorBase + 2L] = BlueLevel << 24L;
		  						LoadRGB32 (ViewPortAddress (ColorWin), COLOR_RGB);
		  					break;
							case PALETTE:
		  						GT_GetGadgetAttrs (MyPaletteGad, ColorWin, NULL, GTPA_Color, &SelectedPen);
		  						if (Copy_Msg)
		    					{
		      						KeepPalette (Win);
		      						Paste (ColorWin, SelectedPen);
		      						Copy_Msg = FALSE;
		    					}

		  						if (Swap_Msg)
		    					{
		      						KeepPalette (Win);
		      						NewPen = SelectedPen;
		      						Swap (ColorWin, OldPen, NewPen);
		      						Swap_Msg = FALSE;
		    					}

		  						if (Spread_Msg)
		    					{
		      						KeepPalette (Win);
		      						NewPen = SelectedPen;
		      						Spread (ColorWin, OldPen, NewPen);
		      						Spread_Msg = FALSE;
		    					}

		  						ColorBase = 3L * SelectedPen + 1L;
		  						RedLevel = (COLOR_RGB[ColorBase] >> 24L);
		  						GreenLevel = (COLOR_RGB[ColorBase + 1L] >> 24L);
		  						BlueLevel = (COLOR_RGB[ColorBase + 2L] >> 24L);
		  						GT_SetGadgetAttrs (RedSliderGad, ColorWin, NULL, GTSL_Level, (int16) RedLevel);
		  						GT_SetGadgetAttrs (GreenSliderGad, ColorWin, NULL, GTSL_Level, (int16) GreenLevel);
		  						GT_SetGadgetAttrs (BlueSliderGad, ColorWin, NULL, GTSL_Level, (int16) BlueLevel);
		  						break;
						}
	      				break;

	    				case IDCMP_CLOSEWINDOW:
	      					Exit = TRUE;
	      				break;
			}
		}
	}
  	while (!Exit);

  	CloseWindow (ColorWin);
  	FreeGadgets (GadList);
  	FreeVisualInfo (VInfo);
  	return TRUE;
}

void Copy (struct Window * Win, uint32 PenNumber)
{
  	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, PenNumber, 1L, COPY_RGB);
}

void Paste (struct Window *Win, const uint32 PenNumber)
{
  const uint32 ColorBase = 3L * PenNumber + 1L;
  uint32 Index;

  	for (Index = 0L; Index < 3L; Index++)
    	COLOR_RGB[ColorBase + Index] = COPY_RGB[Index];
		
  	LoadRGB32 (ViewPortAddress (Win), COLOR_RGB);
}

void Swap (struct Window *Win, const uint32 Pen_1, const uint32 Pen_2)
{
  uint32 Tmp_RGB[3L];
  uint32 Index;

  	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, Pen_1, 1L, Tmp_RGB);
  	Copy (Win, Pen_2);
  	Paste (Win, Pen_1);
  	for (Index = 0L; Index < 3L; Index++)
    	COPY_RGB[Index] = Tmp_RGB[Index];
  	Paste (Win, Pen_2);
}

int16 Spread (struct Window *Win, const uint32 OldPen, const uint32 NewPen)
{
  const uint32 StartPen = MIN (OldPen, NewPen), EndPen = MAX (OldPen, NewPen);
  const uint32 Range = EndPen - StartPen;
  int32 RedStep, GreenStep, BlueStep;
  uint32 Index, RedLevel, GreenLevel, BlueLevel, ColorBase;

  	if (Range < 2L) return TRUE;
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

  	LoadRGB32 (ViewPortAddress (Win), COLOR_RGB);
  	return FALSE;
}

void KeepPalette (struct Window * Win)
{
  uint32 Colors;

  	Colors = 1L << (Win->RPort->BitMap->Depth);
  	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, 0L, Colors, &UNDO_RGB[1L]);
}

void InvertPalette (struct Window *Win, uint32 StartPen, uint32 EndPen)
{
  	while (StartPen < EndPen) Swap (Win, StartPen++, EndPen--);
}

void Shl (struct Window *Win, uint32 StartPen, uint32 EndPen)
{
  	while (StartPen < EndPen) Swap (Win, StartPen, StartPen + 1), StartPen++;
}

void Shr (struct Window *Win, uint32 StartPen, uint32 EndPen)
{
  	while (EndPen > StartPen) Swap (Win, EndPen, EndPen - 1), EndPen--;
}
