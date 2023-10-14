/*
 *  FlashMandel - Render_GMP.c
 *
 *  Copyright (C) 2020 Dino Papararo
 *
 *    Revision 1.0 20/03/2020 dpapararo
 *    Initial release.
 *
 *    Revision 1.2 05/01/2021 dpapararo
 *    Use Checkbox and BlinkRect extern functions, rearranged datatypes
 *    removed FIRSTRENDER variable and rearranged COLORMAP calls
 *    rearranged  pointer math to gain full speed
 *
 *    Revision 1.3 10/01/2021 dpapararo
 *    Small fixes and optimizations
 *
 *    Revision 1.4 16/01/2021 dpapararo
 *    Added Histrogram coloring
 *    removed external mem vectors variables and made small bugfixes
 *
 *    Revision 1.5 09/10/2022 dpapararo
 *    Now (*COLORREMAP) is extern to to avoid conflicts
 *
 *	  Revision 1.6 07/01/2023 dpapararo
 *	  Mem renderings can be stopped and you can draw boxes for any rectangle processeds
 *
 */

#include <exec/types.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>

#include <GMP/gmp.h>
// #include <mpfr.h>
// #include <mpf2mpfr.h>

#include "Headers/FlashMandel.h"

extern uint32 LinearRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 LogRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 RepeatedRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 SquareRootRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 OneRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 TwoRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 ThreeRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 FourRemap (const float64, const float64, const float64, const float64, const float64);

extern uint32 (*COLORREMAP) (const float64, const float64, const float64, const float64, const float64);

extern int16 CheckBox (struct RastPort *, const int16, const int16, const int16, const int16);
extern int16 CheckBoxMem (struct MandelChunk *, uint32 *, const int16, const int16, const int16, const int16);
extern void BlinkRect (struct Window *, const int16, const int16, const int16, const int16);

extern uint32 MASK;

static void MCPointMem_GMP (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16);
static void JCPointMem_GMP (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16);
static void JVLineMem_GMP (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16, const int16);
static void JHLineMem_GMP (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16, const int16);
static void MVLineMem_GMP (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16, const int16);
static void MHLineMem_GMP (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16, const int16);
static void MCPoint_GMP (struct MandelChunk *, struct RastPort *, const int16, const int16);
static void JCPoint_GMP (struct MandelChunk *, struct RastPort *, const int16, const int16);
static void JVLine_GMP (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void JHLine_GMP (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void MVLine_GMP (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void MHLine_GMP (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void MCPoint_GMP_24bit (struct MandelChunk *, struct RastPort *, const int16, const int16);
static void JCPoint_GMP_24bit (struct MandelChunk *, struct RastPort *, const int16, const int16);
static void JVLine_GMP_24bit (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void JHLine_GMP_24bit (struct RastPort *, struct MandelChunk *, uint8 *,	const int16, const int16, const int16);
static void MVLine_GMP_24bit (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void MHLine_GMP_24bit (struct RastPort *, struct MandelChunk *, uint8 *,	const int16, const int16, const int16);			
static int16 RectangleDrawMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *, const int16, const int16, const int16, const int16);
static int16 RectangleDraw_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *,
				const int16, const int16, const int16, const int16);

void CalcFractalMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *);
void CalcFractal_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *);

extern uint32 Mandeln_GMP (uint32, int16);
extern uint32 Julian_GMP (uint32, int16);

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gjkre, gjkim, grmin, gimin,
  				grmax, gimax, gtmp, gdist, gmaxdist, gincremreal, gincremimag;

void (*C_POINT_MEM_GMP) (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16);
void (*V_LINE_MEM_GMP) (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16, const int16);
void (*H_LINE_MEM_GMP) (struct MandelChunk *, uint32 *, uint32 *, const int16, const int16, const int16);
void (*C_POINT_GMP) (struct MandelChunk *, struct RastPort *, const int16, const int16);
void (*V_LINE_GMP) (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
void (*H_LINE_GMP) (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);

/* We can compute the address of an element of the array by using the rows and colums of the array
   Use the following formula:
   &arr[i][j] = baseaddress + [(i * total_colums + j) * sizeof (datatype)];
   If we want to get the value at any given row, column of the array then we can use the value at
   the address of "*" operator and the following formula:
   arr[i][j] = *(ptr + (i * total_colums + j)); */

/* MCPointMem_GMP() */
static void MCPointMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, const int16 x, const int16 y)
{
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	Color = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

  	if (Color)
    {
      	*(RenderMem + (y * MandelInfo->Width + x)) = Color;
      	*(HistogramMem + Color) += 1;
    }
}

/* JCPointMem_GMP() */
static void JCPointMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, const int16 x, const int16 y)
{
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	Color = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

  	if (Color)
    {
      	*(RenderMem + (y * MandelInfo->Width + x)) = Color;
      	*(HistogramMem + Color) += 1;
    }
}

/* MVLineMem_GMP() */
static void MVLineMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, const int16 b1, const int16 b2, const int16 x)
{
  int16 y;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
    {
      	Color = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Color)
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = Color;
	  		*(HistogramMem + Color) += 1;
		}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
    }
}

/* MHLineMem_GMP() */
static void MHLineMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 a2, const int16 y)
{
  int16 x;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	for (x = a1; x <= a2; x++)
    {
      	Color = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Color)
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = Color;
	  		*(HistogramMem + Color) += 1;
		}

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }
}

/* JVLineMem_GMP() */
static void JVLineMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, const int16 b1, const int16 b2, const int16 x)
{
  int16 y;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	/* JKre = MandelInfo->JKre; */
	// mpf_set_d (gjkre,MandelInfo->JKre);

  	/* JKim = MandelInfo->JKim; */
	// mpf_set_d (gjkim,MandelInfo->JKim);

  	for (y = b2; y >= b1; y--)
    {
      	Color = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Color)
	  	{
    	  	*(RenderMem + (y * MandelInfo->Width + x)) = Color;
    	  	*(HistogramMem + Color) += 1;
	  	}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
    }
}

/* JHLineMem_GMP() */
static void JHLineMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 a2, const int16 y)
{
  int16 x;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	/* JKre = MandelInfo->JKre; */
	// mpf_set_d (gjkre,MandelInfo->JKre);

  	/* JKim = MandelInfo->JKim; */
	// mpf_set_d (gjkim,MandelInfo->JKim);

  	for (x = a1; x <= a2; x++)
    {
      	Color = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Color)
		{
		  	*(RenderMem + (y * MandelInfo->Width + x)) = Color;
		  	*(HistogramMem + Color) += 1;
		}

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }
}

/* RectangleDrawMem_GMP() */
static int16 RectangleDrawMem_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  uint16 MyCode;
  uint32 MyClass, ColorBox;
  int16 helpx, helpy, halfx, halfy;
  uint32 ctmp;

#ifdef DRAWBORDERS  
  int16 Rectangle [5 * 2]; 
  struct Border ProcessingRect = { 0, 0, 0, 0, COMPLEMENT, 5, &Rectangle, 0 };
#endif

    if (Win->UserPort->mp_SigBit)
    {
      	while (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort))
		{
	  		MyClass = Message->Class;
	  		MyCode = Message->Code;
	  		GT_ReplyIMsg ((struct IntuiMessage *) Message);

	  		switch (MyClass)
	    	{
	    		case IDCMP_MENUPICK:
	    		{
    				if (MyCode != MENUNULL)
					{
		  				if (ProcessMenu (MandelInfo, Win, NULL, NULL, NULL, NULL, NULL, MyCode) & STOP_MSG) return TRUE;
					}
				}	
	      		break;

	    		case IDCMP_RAWKEY:
	      		{
					switch (MyCode)
					{
						case RAW_TAB:
						{
							BlinkRect (Win, a1, b1, a2, b2);
		  					return FALSE;
						}
						break;
						
	      				case RAW_ESC:
						{
		  					DisplayBeep (Win->WScreen);
		  					return TRUE;
						}
						break;
	      			}
				}
				break;

	    		case IDCMP_CLOSEWINDOW:
	      			return TRUE;
				break;
	    	}
		}
    }

	helpy = b2 - b1; // catch edge case  
    if (helpy < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!	
    helpx = a2 - a1; // catch edge case  
	if (helpx < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!

  	/* it'a a waste of time to render recursively very small rectangles so brute force goes on */
  	if ((helpx <= (MINLIMIT2)) && (helpy <= (MINLIMIT2)))
    {
      	for (helpy = b1 + 1; helpy < b2; helpy++)
			(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, a1 + 1, a2 - 1, helpy);
      
	  	return FALSE;
    }
	
  	halfx = (a1 + a2) / 2;
  	halfy = (b1 + b2) / 2;	// center point coords
	
  	(*C_POINT_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, halfx, halfy);	/* set center point */

  	if (CheckBoxMem (MandelInfo, RenderMem, a1, b1, a2, b2))
    {
      	if (ctmp = *(RenderMem + (b1 * MandelInfo->Width + a1)))
		{
		  	for (helpy = b1 + 1; helpy < b2; helpy++)
		    {
		      	for (helpx = a1 + 1; helpx < a2; helpx++)
				{
				  	*(RenderMem + (helpy * MandelInfo->Width + helpx)) = ctmp;
			  		*(HistogramMem + ctmp) += 1;
				}
		    }
		}

      	return FALSE;
    }
 
  	(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, a1 + 1, halfx - 1, halfy);
  	(*V_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, b1 + 1, halfy - 1, halfx);

#ifdef DRAWBORDERS  	
	Rectangle[6] = Rectangle[8] = a1;
  	Rectangle[1] = Rectangle[3] = Rectangle[9] = b1;
  	Rectangle[2] = Rectangle[4] = halfx;
  	Rectangle[5] = Rectangle[7] = halfy;
  	Rectangle[0] = a1 + 1;	
	DrawBorder (Win->RPort, &ProcessingRect, 0, 0);
#endif
  
  	if (RectangleDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, a1, b1, halfx, halfy)) return TRUE;

  	(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, halfx + 1, a2 - 1, halfy);

#ifdef DRAWBORDERS  
	Rectangle[6] = Rectangle[8] = halfx;
  	Rectangle[1] = Rectangle[3] = Rectangle[9] = b1;
  	Rectangle[2] = Rectangle[4] = a2;
  	Rectangle[5] = Rectangle[7] = halfy;
  	Rectangle[0] = halfx + 1;	
	DrawBorder (Win->RPort, &ProcessingRect, 0, 0);
#endif 
  
  	if (RectangleDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, halfx, b1, a2, halfy)) return TRUE;

  	(*V_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, halfy + 1, b2 - 1, halfx);

#ifdef DRAWBORDERS  
	Rectangle[6] = Rectangle[8] = a1;
  	Rectangle[1] = Rectangle[3] = Rectangle[9] = halfy;
  	Rectangle[2] = Rectangle[4] = halfx;
  	Rectangle[5] = Rectangle[7] = b2;
  	Rectangle[0] = a1 + 1;	
	DrawBorder (Win->RPort, &ProcessingRect, 0, 0);  	
#endif
  
  	if (RectangleDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, a1, halfy, halfx, b2)) return TRUE;

#ifdef DRAWBORDERS  
	Rectangle[6] = Rectangle[8] = halfx;
  	Rectangle[1] = Rectangle[3] = Rectangle[9] = halfy;
  	Rectangle[2] = Rectangle[4] = a2;
  	Rectangle[5] = Rectangle[7] = b2;
  	Rectangle[0] = halfx + 1;	
	DrawBorder (Win->RPort, &ProcessingRect, 0, 0);
#endif

  	if (RectangleDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, halfx, halfy, a2, b2)) return TRUE;

  	return FALSE;
}

/* CalcFractalMem_GMP() */
void CalcFractalMem_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint32 *RenderMem, uint32 *HistogramMem)
{
  STRPTR PleaseWaitTxt = "Processing rendering in memory, please wait until it's done or press ESC to stop.";

	ShowTitle (Win->WScreen, TRUE);
	SetWindowTitles (Win, (STRPTR) ~0, PleaseWaitTxt);
	
  	if (MandelInfo->Flags & JULIA_BIT)
    {
      	C_POINT_MEM_GMP = JCPointMem_GMP;
      	H_LINE_MEM_GMP = JHLineMem_GMP;
      	V_LINE_MEM_GMP = JVLineMem_GMP;
    }

  	else if (MandelInfo->Flags & MANDEL_BIT)
    {
      	C_POINT_MEM_GMP = MCPointMem_GMP;
      	H_LINE_MEM_GMP = MHLineMem_GMP;
      	V_LINE_MEM_GMP = MVLineMem_GMP;
    }

  	if (MandelInfo->Flags & LINEAR_BIT) COLORREMAP = LinearRemap;
  	else if (MandelInfo->Flags & LOG_BIT) COLORREMAP = LogRemap;
  		else if (MandelInfo->Flags & REPEATED_BIT) COLORREMAP = RepeatedRemap;
  			else if (MandelInfo->Flags & SQUARE_BIT) COLORREMAP = SquareRootRemap;
  				else if (MandelInfo->Flags & ONE_BIT) COLORREMAP = OneRemap;
  					else if (MandelInfo->Flags & TWO_BIT) COLORREMAP = TwoRemap;
  						else if (MandelInfo->Flags & THREE_BIT) COLORREMAP = ThreeRemap;
  							else if (MandelInfo->Flags & FOUR_BIT) COLORREMAP = FourRemap;
			
  	(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
  	(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
  	(*V_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
  	(*V_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);

  	RectangleDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);

	if (! (MASK & TMASK)) ShowTitle (Win->WScreen, FALSE);
}

/****** No Mem ******/

/* MCPoint_GMP() */
static void MCPoint_GMP (struct MandelChunk *MandelInfo, struct RastPort *Rp, const int16 x, const int16 y)
{
  uint32 Iteration,Color=NULL;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	Iteration = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

  	if (Iteration) Color = COLORREMAP ((float64) Iteration, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
  
  	SetAPen (Rp, Color);		  
  	WritePixel (Rp, x, y);
}

/* MVLine_GMP() */
static void MVLine_GMP (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 b1, const int16 b2, const int16 x)
{
  uint8 *TmpArray = (PixelLine + b2 - b1);
  int16 y;
  uint32 Iteration;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
    {
      	Iteration = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Iteration) *TmpArray-- = COLORREMAP ((float64) Iteration, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
      	else *TmpArray-- = 0;

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
    }

  	WriteChunkyPixels (Rp, x, b1, x, b2, PixelLine, 1);
}

/* MHLine_GMP() */
static void MHLine_GMP (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 a1, const int16 a2, const int16 y)
{
  uint8 *TmpArray = PixelLine;
  int16 x;
  uint32 Iteration;

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	for (x = a1; x <= a2; x++)
    {
      	Iteration = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Iteration) *TmpArray++ = COLORREMAP ((float64) Iteration, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
	  	else *TmpArray++ = 0;

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }

  	WriteChunkyPixels (Rp, a1, y, a2, y, PixelLine, a2 - a1 + 1);
}

/* JCPoint_GMP() */
static void JCPoint_GMP (struct MandelChunk *MandelInfo, struct RastPort *Rp, const int16 x, const int16 y)
{
  uint32 Iteration,Color=NULL;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	Iteration = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

  	if (Iteration) Color = COLORREMAP ((float64) Iteration, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);

  	SetAPen (Rp, Color);		  
  	WritePixel (Rp, x, y);
}

/* JVLine_GMP() */
static void JVLine_GMP (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 b1, const int16 b2, const int16 x)
{
  uint8 *TmpArray = (PixelLine + b2 - b1);
  int16 y;
  uint32 Iteration;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
    {
      	Iteration = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Iteration) *TmpArray-- = COLORREMAP ((float64) Iteration, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
      	else *TmpArray-- = 0;
      
	  	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);      
    }

  	WriteChunkyPixels (Rp, x, b1, x, b2, PixelLine, 1);
}

/* JHLine_GMP() */
static void JHLine_GMP (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 a1, const int16 a2, const int16 y)
{
  int16 x;
  uint8 *TmpArray = PixelLine;
  uint32 Iteration;

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	for (x = a1; x <= a2; x++)
    {
      	Iteration = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

      	if (Iteration) *TmpArray++ = COLORREMAP ((float64) Iteration, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
	  	else *TmpArray++ = 0;

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }

  	WriteChunkyPixels (Rp, a1, y, a2, y, PixelLine, a2 - a1 + 1);
}

/* 24bit rendering */

/* MCPoint_GMP_24bit() */
static void MCPoint_GMP_24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, const int16 x, const int16 y)
{
  uint8 r,g,b;
  uint32 Iteration,Color_ARGB=0xff000000;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	Iteration = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);

  	if (Iteration)
  	{
        	r = (uint8) lround ((sin(0.016 * (float64) Iteration + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Iteration + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Iteration + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
  	}

  	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* MVLine_GMP_24bit() */
static void MVLine_GMP_24bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 b1, const int16 b2, const int16 x)
{
  uint8 r,g,b;
  int16 y;
  uint32 Iteration,Color_ARGB;
 
  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
    {
      	Iteration = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);
		Color_ARGB = 0xff000000;

      	if (Iteration)
      	{
	    	r = (uint8) lround ((sin(0.016 * (float64) Iteration + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Iteration + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Iteration + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
      	}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
	
      	WritePixelColor (Rp, x, y, Color_ARGB);
    }
}

/* MHLine_GMP_24bit() */
static void MHLine_GMP_24bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 a1, const int16 a2, const int16 y)
{
  uint8 r,g,b;
  int16 x;
  uint32 Iteration,Color_ARGB;
 
  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	for (x = a1; x <= a2; x++)
    {
      	Iteration = Mandeln_GMP (MandelInfo->Iterations, MandelInfo->Power);
      	Color_ARGB = 0xff000000;

      	if (Iteration)
      	{
	        r = (uint8) lround ((sin(0.016 * (float64) Iteration + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Iteration + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Iteration + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
    }
}

/* JCPoint_GMP_24bit() */
static void JCPoint_GMP_24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, const int16 x, const int16 y)
{
  uint8 r,g,b;
  uint32 Iteration,Color_ARGB=0xff000000;
 
  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	Iteration = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);

  	if (Iteration)
  	{
        r = (uint8) lround ((sin(0.016 * (float64) Iteration + 0.20) * 127.5 + 127.5));
		Color_ARGB |= (r << 16);
		g = (uint8) lround ((sin(0.013 * (float64) Iteration + 0.15) * 127.5 + 127.5));
		Color_ARGB |= (g << 8);
		b = (uint8) lround ((sin(0.010 * (float64) Iteration + 0.10) * 127.5 + 127.5));
		Color_ARGB |= b;
  	}

  	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* JVLine_GMP_24bit() */
static void JVLine_GMP_24bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 b1, const int16 b2, const int16 x)
{
  uint8 r,g,b;
  int16 y;
  uint32 Iteration,Color_ARGB;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
    {
      	Iteration = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);
      	Color_ARGB = 0xff000000;

	  	if (Iteration)
      	{
	    	r = (uint8) lround ((sin(0.016 * (float64) Iteration + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Iteration + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Iteration + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
      	}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);

      	WritePixelColor (Rp, x, y, Color_ARGB);
    }      
}

/* JHLine_GMP_24bit() */
static void JHLine_GMP_24bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 a1, const int16 a2, const int16 y)
{
  uint8 r,g,b;
  int16 x;
  uint32 Iteration,Color_ARGB;
 
  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

  	for (x = a1; x <= a2; x++)
    {
      	Iteration = Julian_GMP (MandelInfo->Iterations, MandelInfo->Power);
      	Color_ARGB = 0xff000000;

      	if (Iteration)
      	{
	        r = (uint8) lround ((sin(0.016 * (float64) Iteration + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Iteration + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Iteration + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
   	}
}

/* RectangleDraw_GMP() */
static int16 RectangleDraw_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint8 * ARGBMem, uint8 * PixMem, uint8 * GfxMem, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  uint16 MyCode;
  int16 helpx, helpy, halfx, halfy;
  uint32 MyClass, ColorBox;

	helpy = b2 - b1; // catch edge case  
    if (helpy < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!	
    helpx = a2 - a1; // catch edge case  
	if (helpx < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!
 
  	if (Win->UserPort->mp_SigBit)
    {
      	while (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort))
		{
	  		MyClass = Message->Class;
	  		MyCode = Message->Code;
	  		GT_ReplyIMsg ((struct IntuiMessage *) Message);

	  		switch (MyClass)
	    	{
	    		case IDCMP_MENUPICK:
	    		{
					if (MyCode != MENUNULL)
					{
		  				if (ProcessMenu (MandelInfo, Win, ARGBMem, PixMem, PixelVecBase, RenderMem, GfxMem, MyCode) & STOP_MSG) return TRUE;
					}
				}	
	      		break;

	    		case IDCMP_RAWKEY:
	      		{
					switch (MyCode)
					{
						case RAW_TAB:
						{
							BlinkRect (Win, a1, b1, a2, b2);
		  					return FALSE;
						}
						break;
						
	      				case RAW_ESC:
						{
		  					DisplayBeep (Win->WScreen);
		  					return TRUE;
						}
						break;
	      			}
				}
				break;

	    		case IDCMP_CLOSEWINDOW:
	      			return TRUE;
				break;
	    	}
		}
    }

  	if ((helpx <= (MINLIMIT2)) && (helpy <= (MINLIMIT2)))
    {
      	for (helpy = b1 + 1; helpy < b2; helpy++)
			(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, a1 + 1, a2 - 1, helpy);
      	
		return FALSE;
    }

   	halfx = (a1 + a2) / 2; // x center point coords
    halfy = (b1 + b2) / 2; // y center point coords

  	(*C_POINT_GMP) (MandelInfo, Win->RPort, halfx, halfy);	/* set center point */

  	if (CheckBox (Win->RPort, a1, b1, a2, b2))
    {
      	RectFillColor (Win->RPort, a1 + 1, b1 + 1, a2 - 1, b2 - 1, ReadPixelColor (Win->RPort, a2, b1));
      	return FALSE;
    }
 
  	(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, a1 + 1, halfx - 1, halfy); // don't recalc center point
  	(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, b1 + 1, halfy - 1, halfx);
  	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, b1, halfx, halfy)) return TRUE;

  	(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, halfx + 1, a2 - 1, halfy);
  	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, b1, a2, halfy)) return TRUE;

  	(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, halfy + 1, b2 - 1, halfx); 
	
	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, halfy, halfx, b2)) return TRUE;
  	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, halfy, a2, b2)) return TRUE;

  	return FALSE;
}

/* CalcFractal_GMP() */
void CalcFractal_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint8 * ARGBMem, uint8 * PixMem, uint8 * GfxMem, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem)
{
  	if (MandelInfo->Flags & JULIA_BIT)
  	{  
      	if (GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MAX_DEPTH)
	  	{
          	C_POINT_GMP = JCPoint_GMP_24bit;
	      	H_LINE_GMP = JHLine_GMP_24bit;
	      	V_LINE_GMP = JVLine_GMP_24bit;
	  	}
	  
	  	else
	  	{
      	  	C_POINT_GMP = JCPoint_GMP;
      	  	H_LINE_GMP = JHLine_GMP;
	      	V_LINE_GMP = JVLine_GMP;
	  	}    
  	}
	
  	else if (MandelInfo->Flags & MANDEL_BIT)
    {
		if (GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MAX_DEPTH)
    	{
   		   	C_POINT_GMP = MCPoint_GMP_24bit;
	       	H_LINE_GMP = MHLine_GMP_24bit;
	       	V_LINE_GMP = MVLine_GMP_24bit;
  		}
	  
  		else
        {
		   	C_POINT_GMP = MCPoint_GMP;
           	H_LINE_GMP = MHLine_GMP;
           	V_LINE_GMP = MVLine_GMP;
		}
    }

  	if (MandelInfo->Flags & LINEAR_BIT) COLORREMAP = LinearRemap;
  	else if (MandelInfo->Flags & LOG_BIT) COLORREMAP = LogRemap;
  		else if (MandelInfo->Flags & REPEATED_BIT) COLORREMAP = RepeatedRemap;
  			else if (MandelInfo->Flags & SQUARE_BIT) COLORREMAP = SquareRootRemap;
  				else if (MandelInfo->Flags & ONE_BIT) COLORREMAP = OneRemap;
  					else if (MandelInfo->Flags & TWO_BIT) COLORREMAP = TwoRemap;
  						else if (MandelInfo->Flags & THREE_BIT) COLORREMAP = ThreeRemap;
  							else if (MandelInfo->Flags & FOUR_BIT) COLORREMAP = FourRemap;
	// draw perimeter
  	(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
  	(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
  	(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
  	(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);
	// start divide et impera recursively!
  	RectangleDraw_GMP (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
}
