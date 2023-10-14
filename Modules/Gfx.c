/**********************************************************************************************************
**
**  Coded by Dino Papararo     11-Feb-1999
**
**  FUNCTION
**    Fade -- perform screen color fading from/to black.
**
**  SYNOPSIS
**
**    int16 Fade (struct Window *Win,uint32 *PaletteSrc,uint32 MaxStep,uint32 MyTimeDelay,int16 ToBlack)
**
**  DESCRIPTION
**
**    According with ToBlack value screen will fade from black to PaletteSrc or from PaletteSrc to black.
**    MaxStep is number of times colors will be decreased/increased from/to black.
**    MyTimeDelay is the wait time between two palette changes, needed for multitasking and faster cpus.
**
**  RETURN
**
**    FALSE if screen propreties not available or Depth < 2 or MaxStep < 2, TRUE color cycling performed.
**
***********************************************************************************************************
**
**  Coded by Claudio Pucci & Dino Papararo     26-Oct-1997
**
**  FUNCTION
**
**    Cycle -- perform screen color cycling.
**
**  SYNOPSIS
**
**    int16 Cycle (struct Window *Win,uint32 MyTimeDelay,int16 Left)
**
**  DESCRIPTION
**
**    Function uses a double sized table to manage color cycling.
**    So we do not need to save and shift all colors for every cycle,
**    but only save and set correct first and last values !
**    This function do not load the first four color registers reserved for GUI pens.
**
**  RETURN
**
**    FALSE if screen propreties not available or Depth < 2, TRUE color cycling performed.
**
**********************************************************************************************************/

/* Modified for ARexx-support by E. Schwan 23.1.2002 */
/* Modified for AmigaOS4/GCC 28.06.2004, Edgar Schwan */
/* Cleaned for GFX OS4 code 04.03.2020, dpapararo */
/* Various fixes 19.03.2020, dpapararo */
/* Rearranged datatypes 05.01.2021, dpapararo */
/* Now logic is based on DimensionInfo struct 04.12.2021 dpapararo */
/* Implemented ARGB fade and Random API call function 06.02.2022 dpapararo */ 

#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/gadtools.h>

#include "Headers/FlashMandel.h"

#define RESERVED_PENS 4L

//#define BLACK 0L
//int16 Fade (struct Window *,uint32 *,uint32,uint32,int16);
//int16 Cycle (struct Window *,uint32,int16);

int16 Fade (struct Window *Win, uint8 *ARGBMem, uint32 *PaletteSrc, uint32 MaxStep, uint32 MyTimeDelay, int16 ToBlack)
{
//  DisplayInfoHandle DHandle;
//  struct DimensionInfo DimInfo;
  static uint32 __attribute__ ((aligned (16))) PaletteTmp [3L * 252L + 2L];
//  uint8 *ARGB = NULL;
  uint8 R, G, B;
  int16 Success = FALSE;
  uint32 Var, Step, Rows, Cols, Modulo;
  uint32 DstWinWidth, DstWinHeight, Depth, Color, Range, ModeID;
  struct RandomState State;

/*
   	ModeID = GetVPModeID (ViewPortAddress (Win));
  	DHandle = FindDisplayInfo (ModeID);

  	if (!((DHandle) && GetDisplayInfoData (DHandle, (uint8 *) &DimInfo, sizeof (struct DimensionInfo), DTAG_DIMS, ModeID))) goto ExitFade;
*/	
	DstWinWidth = ((Win->Flags & WFLG_GIMMEZEROZERO) ? Win->GZZWidth : Win->Width);
    DstWinHeight = ((Win->Flags & WFLG_GIMMEZEROZERO) ? Win->GZZHeight : Win->Height);	
	Depth = GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH);
	
	if (Depth == MIN_DEPTH) // 8 bit screens
    {
	    if (ToBlack)
		{
      		Range = (1L << Depth) - 4L; 
      		PaletteTmp [0L] = (Range << 16L) + 4L;
      		GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, 4L, Range, &PaletteTmp [1L]);
      		PaletteTmp [3L * Range + 1L] = NULL;
			PaletteSrc += (3 * RESERVED_PENS); /* exclude reserved pens for GUI */
			
 	  		for (Step = 0L; Step <= MaxStep; Step++)
	    	{
					for (Var = 1; Var <= 3L * Range; Var += 3) /* unroll loop 3 times */
					{
		  				if (PaletteTmp [Var + 0])
		    			{
		      				Color = (uint32) (((PaletteSrc [Var + 0] >> 24L) * (MaxStep - Step)) / MaxStep);
							PaletteTmp [Var + 0] = (Color << 24L);
		    			}

	  					if (PaletteTmp [Var + 1])
		    			{
		      				Color = (uint32) (((PaletteSrc [Var + 1] >> 24L) * (MaxStep - Step)) / MaxStep);
							PaletteTmp [Var + 1] = (Color << 24L);
		    			}

	  					if (PaletteTmp [Var + 2])
		    			{
		      				Color = (uint32) (((PaletteSrc [Var + 2] >> 24L) * (MaxStep - Step)) / MaxStep);
							PaletteTmp [Var + 2] = (Color << 24L);
		    			}
					}

	      			WaitTOF ();
	      			LoadRGB32 (ViewPortAddress (Win), &PaletteTmp);
	      			Delay (MyTimeDelay);
			}
		}

      	else
		{
		  	State.rs_High = time (NULL);
  			State.rs_Low  = time (NULL);
			
			for (Var = 0; Var < ((DstWinWidth * DstWinHeight) / 3); Var++)
					WritePixelColor (Win->RPort, Random (&State) % (DstWinWidth - 1), Random (&State) % (DstWinHeight - 1), 0xff000000);
		}

      	Success = TRUE;
		goto ExitFade;		
    }
	
	else if (Depth == MAX_DEPTH) // 24bit screens
	{ 
		if (ToBlack)
		{				
			Modulo = DstWinWidth;
				
			ReadPixelArray (Win->RPort, 0, 0, ARGBMem, Win->LeftEdge, Win->TopEdge,
							Modulo * 4, PIXF_A8R8G8B8, DstWinWidth, DstWinHeight);

			for (Step = 0L; Step <= MaxStep; Step++)
	    	{
				for (Cols = Win->TopEdge; Cols < (DstWinHeight * 4); Cols += 4)
    			{
      				for (Rows = Win->LeftEdge; Rows < (DstWinWidth * 4); Rows += 4) // unroll loop twice
        			{
        				if ((R = *(ARGBMem + (Cols * Modulo + (Rows + 1)))))
							*(ARGBMem + (Cols * Modulo + (Rows + 1))) = (uint8) ((R * (MaxStep - Step)) / MaxStep);
	
       					if ((G = *(ARGBMem + (Cols * Modulo + (Rows + 2)))))
							*(ARGBMem + (Cols * Modulo + (Rows + 2))) = (uint8) ((G * (MaxStep - Step)) / MaxStep);

       					if ((B = *(ARGBMem + (Cols * Modulo + (Rows + 3)))))
							*(ARGBMem + (Cols * Modulo + (Rows + 3))) = (uint8) ((B * (MaxStep - Step)) / MaxStep);       					
					}
   				}	
	
				WaitTOF ();
   				WritePixelArray (ARGBMem, 0, 0, Modulo * 4, PIXF_A8R8G8B8,
									Win->RPort, Win->LeftEdge, Win->TopEdge, DstWinWidth, DstWinHeight);
				Delay (MyTimeDelay);
			}
		}
		
		else 
		{
			for (Cols = Win->TopEdge; Cols < DstWinHeight; Cols += 2)
			{
				for (Rows = Win->LeftEdge; Rows < DstWinWidth; Rows += 4) // unroll loop twice
				{
					WritePixelColor (Win->RPort, Rows, Cols, 0xff000000);
					WritePixelColor (Win->RPort, Rows + 2, Cols, 0xff000000);
				}
			
				Delay (MyTimeDelay / 2);
			}		
		}
		
		Success = TRUE;
	}	

ExitFade:
  	return (Success);
}

int16 Cycle (struct Window * Win, uint32 MyTimeDelay, int16 Left)
{
  DisplayInfoHandle DHandle;
  struct DimensionInfo DimInfo;
  struct IntuiMessage *Message = NULL;
  static uint32 __attribute__ ((aligned (16))) Palette_Tmp [2L * 3L * 252L + 2L];
  int16 Loop = TRUE, Success = FALSE;
  uint16 MyCode;
  uint32 MyClass, Counter = NULL, OldBlue, OldRed, Tmp_1, Tmp_2, HalfRange;
  uint32 ModeID, Range;

	ModeID = GetVPModeID (ViewPortAddress (Win));
  	DHandle = FindDisplayInfo (ModeID);

	if (!((DHandle) && GetDisplayInfoData (DHandle, (STRPTR) &DimInfo, sizeof (struct DimensionInfo), DTAG_DIMS, ModeID))) goto ExitCycle;
	{
		Range = (1L << DimInfo.MaxDepth) - 4L; 
      	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, 4L, Range, &Palette_Tmp [1L]);
      	GetRGB32 (((struct ViewPort *) ViewPortAddress (Win))->ColorMap, 4L, Range, &Palette_Tmp [3L * Range + 1L]);
      	Palette_Tmp [2L * 3L * Range + 1L] = NULL;
      	Palette_Tmp [0L] = (Range << 16L) + 4L;
      	if (!Left) Counter = Range + 1L;

    	do
		{
	  		if (Win->UserPort->mp_SigBit)
	    	{
	      		while ((Loop) && (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort)))
				{
			  		MyClass = Message->Class;
			  		MyCode = Message->Code;
			  		GT_ReplyIMsg ((struct IntuiMessage *) Message);

			  		switch (MyClass)
			  	  	{
				    	case IDCMP_MOUSEBUTTONS:  	    
	    					Loop = FALSE;
				     	break;
				    	case IDCMP_MENUPICK:
				      		Loop = FALSE;
				      	break;
				    	case IDCMP_RAWKEY:
				      		if (MyCode == RAW_ESC)
							Loop = FALSE;
				      	break;
				  	}
				}
	    	}

	  		if (Left)
	    	{
	      		Counter++;
	      		if (Counter > Range)
				Counter = 1L;
	    	}

	  		else
	    	{
	      		Counter--;
	      		if (Counter < 1L)
				Counter = Range;
	    	}

	  		Tmp_1 = 3L * Counter;
	  		Tmp_2 = 3L * (Counter + Range) + 1L;
	  		OldBlue = Palette_Tmp [Tmp_1];
	  		OldRed = Palette_Tmp [Tmp_2];
	  		Palette_Tmp [Tmp_1] = (Range << 16L) + 4L;
	  		Palette_Tmp [Tmp_2] = NULL;
	  		WaitTOF ();
	  		LoadRGB32 (ViewPortAddress (Win), &Palette_Tmp [3L * Counter]);
	  		Delay (MyTimeDelay);
	  		Palette_Tmp [Tmp_1] = OldBlue;
	  		Palette_Tmp [Tmp_2] = OldRed;
		} while (Loop);

    	Success =  TRUE;
  	}

ExitCycle:
  	return (Success);
}
