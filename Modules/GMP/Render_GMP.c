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
 *	  Revision 1.7 10/11/2024 dpapararo
 *	  Added benchmark mode fail control
 *
 *	  Revision 1.8 22/12/2024 dpapararo
 *	  modified coloring algorithm
 *
 */

#include <exec/types.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>

#include <GMP/gmp.h>
// #include <mpfr.h>
// #include <mpf2mpfr.h>

#include "Headers/FlashMandel.h"

extern void AddQueue (uint32, uint32);
extern int32 AllocateBoundary (uint32, uint32);
extern void DeallocateBoundary (void); 

extern uint32 (*COLORREMAP) (const uint32, const uint32, const uint32);

extern uint32 LinearRemap (const uint32, const uint32, const uint32);
extern uint32 LogRemap (const uint32, const uint32, const uint32);
extern uint32 RepeatedRemap (const uint32, const uint32, const uint32);
extern uint32 SquareRootRemap (const uint32, const uint32, const uint32);
extern uint32 OneRemap (const uint32, const uint32, const uint32);
extern uint32 TwoRemap (const uint32, const uint32, const uint32);
extern uint32 ThreeRemap (const uint32, const uint32, const uint32);
extern uint32 FourRemap (const uint32, const uint32, const uint32);

extern int16 CheckBox (struct RastPort *, const int16, const int16, const int16, const int16);
extern int16 CheckBoxMem (struct MandelChunk *, uint32 *, const int16, const int16, const int16, const int16);
extern void BlinkRect (struct Window *, const int16, const int16, const int16, const int16);

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
static void MCPoint_GMP_16_32bit (struct MandelChunk *, struct RastPort *, const int16, const int16);
static void JCPoint_GMP_16_32bit (struct MandelChunk *, struct RastPort *, const int16, const int16);
static void JVLine_GMP_16_32bit (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void JHLine_GMP_16_32bit (struct RastPort *, struct MandelChunk *, uint8 *,	const int16, const int16, const int16);
static void MVLine_GMP_16_32bit (struct RastPort *, struct MandelChunk *, uint8 *, const int16, const int16, const int16);
static void MHLine_GMP_16_32bit (struct RastPort *, struct MandelChunk *, uint8 *,	const int16, const int16, const int16);		

static int16 BoundaryDrawMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *);
static uint32 LoadMem_GMP (struct MandelChunk *, uint32 *, uint32 *, uint32);
static void ScanMem_GMP (struct MandelChunk *, uint32 *, uint32 *, uint32, uint32);

static int16 BoundaryDraw_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *);
static void Scan_GMP (struct MandelChunk *, struct RastPort *, uint32 *, uint32, uint32 , int16);
static uint32 Load_GMP (struct MandelChunk *, struct RastPort *, uint32 *, uint32, int16);

static int16 RectangleDrawMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *, const int16, const int16, const int16, const int16);
static int16 RectangleDraw_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *,	const int16, const int16, const int16, const int16);

static int16 BruteDrawMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *, const int16, const int16, const int16, const int16);
static int16 BruteDraw_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *,	const int16, const int16, const int16, const int16);

void CalcFractalMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *);
void CalcFractal_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *);

extern uint32 Mandeln_GMP (uint32, int16);
extern uint32 Julian_GMP (uint32, int16);

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gjkre, gjkim, grmin, gimin,
  				grmax, gimax, gtmp, gdist, gmaxdist, gincremreal, gincremimag;

extern uint8 *DONE;
extern uint32 *DATA, *QUEUE;
extern int16 BENCHMARK_FAIL;
extern uint32 MASK;
extern uint32 QueueHead;	
extern enum { Loaded = 1, Queued = 2 };			

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

/* BruteDrawMem_GMP() */
static int16 BruteDrawMem_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  uint16 MyCode;
  uint32 MyClass;
  int16 helpy;

	for (helpy = b1; helpy <= b2; helpy++)
	{
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
			  				if (ProcessMenu (MandelInfo, Win, 0, 0, 0,  NULL, NULL, 0, MyCode) & STOP_MSG) 
							{
								DisplayBeep (Win->WScreen);			  
								return TRUE;
							}
						}
					}	
		      		break;

		    		case IDCMP_RAWKEY:
		      		{
						if (MyCode == RAW_ESC)
						{
			  				DisplayBeep (Win->WScreen);
							if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																				
			  				return TRUE;
						}							      			
					}
					break;

		    		case IDCMP_CLOSEWINDOW:
					{
	                	DisplayBeep (Win->WScreen);					  				
		      			return TRUE;
					}
					break;
		    	}
			}
    	}
	
		(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, a1, a2, helpy);
	}

  	return FALSE;
}

uint32 LoadMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, uint32 P) 
{
  int16 X, Y;
  uint32 ResX, ResY;

	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;		

	if (DONE [P] & Loaded) return DATA [P];
	
	X = P % ResX;
    Y = P / ResX;

	(*C_POINT_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, X, Y);
			
    DONE [P] |= Loaded;
    DATA [P] = *(RenderMem + (Y * MandelInfo->Width + X));
	return DATA [P];
}

void ScanMem_GMP (struct MandelChunk *MandelInfo, uint32 *RenderMem, uint32 *HistogramMem, uint32 QueueSize, uint32 P)
{
  uint32 Center;
  int32 LL, RR, UU, DD;
  int32 L, R, U, D; 
  uint32 ResX, ResY;
  uint32 X, Y;

	Center = LoadMem_GMP (MandelInfo, RenderMem, HistogramMem, P);
	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;
	X = P % ResX;
	Y = P / ResX;
	LL = X >= 1L;
	UU = Y >= 1L;
	RR = X < ResX - 1L;
	DD = Y < ResY - 1L;
	
	/* booleans */
   	L = LL && LoadMem_GMP (MandelInfo, RenderMem, HistogramMem, P - 1L) != Center;
    R = RR && LoadMem_GMP (MandelInfo, RenderMem, HistogramMem, P + 1L) != Center;
    U = UU && LoadMem_GMP (MandelInfo, RenderMem, HistogramMem, P - ResX) != Center;
    D = DD && LoadMem_GMP (MandelInfo, RenderMem, HistogramMem, P + ResX) != Center;
	
    /* process the queue (which is actuaLLy a ring buffer) */
    if (L) AddQueue (QueueSize, P - 1L);
    if (R) AddQueue (QueueSize, P + 1L);
    if (U) AddQueue (QueueSize, P - ResX);
    if (D) AddQueue (QueueSize, P + ResX);
	
    /* the corner pixels (nw,ne,sw,se) are also neighbors */
    if ((UU && LL) && (L || U)) AddQueue (QueueSize, P - ResX - 1L);
    if ((UU && RR) && (R || U)) AddQueue (QueueSize, P - ResX + 1L);
    if ((DD && LL) && (L || D)) AddQueue (QueueSize, P + ResX - 1L);
    if ((DD && RR) && (D || D)) AddQueue (QueueSize, P + ResX + 1L);
}
/* end boundary trace functions */

int16 BoundaryDrawMem_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint32 *RenderMem, uint32 *HistogramMem)
{
  uint16 MyCode;
  uint32 MyClass;  
  struct IntuiMessage *Message;

  int16 Flag = 0;
  uint32 P, QueueTail, QueueSize;
  uint32 X, Y, ResX, ResY;
 
 	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;
	
	QueueTail = 0L;
	QueueSize = sizeof (int32) * ResX * ResY * 4L;
	
	/* allocation check */
	if (AllocateBoundary (ResX, ResY) != RETURN_OK) return FALSE;	
    
	/* (1) begin by adding the screen edges into the queue */
    for (Y = 0L; Y < ResY; ++Y) 
	{
        AddQueue (QueueSize, Y * ResX /*+ 0L*/);		
        AddQueue (QueueSize, Y * ResX + (ResX - 1L));
    }
	
    for (X = 1L; X < (ResX - 1L); ++X) 
	{
        AddQueue (QueueSize, /*0L*Width* +*/ X);
        AddQueue (QueueSize, (ResY - 1L) * ResX + X);
    }
	
	/* (2) process the queue (which is actually a ring buffer) */	
    while (QueueTail != QueueHead) 
	{
        if ((QueueHead <= QueueTail) || (++Flag & 3))
		{
            P = QUEUE [QueueTail++];
            if (QueueTail == QueueSize) QueueTail = 0L;
        } 
		
		else P = QUEUE [--QueueHead];
        
		ScanMem_GMP (MandelInfo, RenderMem, HistogramMem, QueueSize, P);
		
		if (Win->UserPort->mp_SigBit)
  		{
     		while (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort))
     		{
     			MyClass = Message->Class;
     		    MyCode  = Message->Code;
        		GT_ReplyIMsg ((struct IntuiMessage *) Message);

        		switch (MyClass)
				{
        			case IDCMP_MENUPICK:
					{
						if (MyCode != MENUNULL)
                    	{
                   			if (ProcessMenu (MandelInfo, Win, 0, 0, 0,  NULL, NULL, 0, MyCode) & STOP_MSG)							
							{
                    			DisplayBeep (Win->WScreen);					  							
								DeallocateBoundary();										
								return TRUE;
							}
						}
                    }
					break;

                    case IDCMP_RAWKEY:
                	{
						if (MyCode == RAW_ESC) 
						{						
    	               		DisplayBeep (Win->WScreen);
							DeallocateBoundary();
							if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																				
    	               		return TRUE;    	           		
						}
					}
					break;

        			case IDCMP_CLOSEWINDOW: 
					{
                    	DisplayBeep (Win->WScreen);					  					
						DeallocateBoundary();									
						return TRUE;
					}	
					break;
        		}
     		}
  		}
    }
						
    /* (3) lastly, fill uncalculated areas with neighbor color */
    for (P = 0L; P < ResX * ResY - 1L; ++P) 
	{
        if (DONE [P] & Loaded)
		{
        	if (! (DONE [P + 1L] & Loaded)) 
			{
            	DATA [P + 1L] = DATA [P];
		  		*(RenderMem + ((P + 1L) / ResX * MandelInfo->Width + (P + 1L) % ResX)) = DATA [P];
				*(HistogramMem + DATA [P]) += 1;
				DONE [P + 1L] |= Loaded;
        	}
		}
    }	   
	
	DeallocateBoundary();	
	return TRUE;
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
		  				if (ProcessMenu (MandelInfo, Win, 0, 0, 0,  NULL, NULL, 0, MyCode) & STOP_MSG) 
						{
							DisplayBeep (Win->WScreen);					  
							return TRUE;
						}
					}
				}	
	      		break;

	    		case IDCMP_RAWKEY:
	      		{
					if (MyCode == RAW_ESC) 
					{								      				
		  				DisplayBeep (Win->WScreen);
						if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																			
		  				return TRUE;
					}					
				}
				break;

	    		case IDCMP_CLOSEWINDOW:
				{
                	DisplayBeep (Win->WScreen);					  				
	      			return TRUE;
				}
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

	if ((MandelInfo->Flags & BOUNDARY_BIT) && (MandelInfo->Flags & MANDEL_BIT))
	{ 
		BoundaryDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem);
	}

	else if (MandelInfo->Flags & TILING_BIT)	
	{			
	  	(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
	  	(*H_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	  	(*V_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
	  	(*V_LINE_MEM_GMP) (MandelInfo, RenderMem, HistogramMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);

	  	RectangleDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	}
	
	else if (MandelInfo->Flags & BRUTE_BIT)
	{
	  	BruteDrawMem_GMP (MandelInfo, Win, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	}	
		
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

  	if (Iteration) Color = COLORREMAP (Iteration, MandelInfo->Iterations, 252L);
  
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

      	if (Iteration) *TmpArray-- = COLORREMAP (Iteration, MandelInfo->Iterations, 252L);
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

      	if (Iteration) *TmpArray++ = COLORREMAP (Iteration, MandelInfo->Iterations, 252L);
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

  	if (Iteration) Color = COLORREMAP (Iteration, MandelInfo->Iterations, 252L);

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

      	if (Iteration) *TmpArray-- = COLORREMAP (Iteration, MandelInfo->Iterations, 252L);
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

      	if (Iteration) *TmpArray++ = COLORREMAP (Iteration, MandelInfo->Iterations, 252L);
	  	else *TmpArray++ = 0;

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }

  	WriteChunkyPixels (Rp, a1, y, a2, y, PixelLine, a2 - a1 + 1);
}

/* 16_32bit rendering */

/* MCPoint_GMP_16_32bit() */
static void MCPoint_GMP_16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, const int16 x, const int16 y)
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
        	r = (uint8) lround (cos (0.016f * (float64) Iteration + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Iteration + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Iteration + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
  	}

  	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* MVLine_GMP_16_32bit() */
static void MVLine_GMP_16_32bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 b1, const int16 b2, const int16 x)
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
	    	r = (uint8) lround (cos (0.016f * (float64) Iteration + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Iteration + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Iteration + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
      	}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
	
      	WritePixelColor (Rp, x, y, Color_ARGB);
    }
}

/* MHLine_GMP_16_32bit() */
static void MHLine_GMP_16_32bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 a1, const int16 a2, const int16 y)
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
	        r = (uint8) lround (cos (0.016f * (float64) Iteration + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Iteration + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Iteration + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
    }
}

/* JCPoint_GMP_16_32bit() */
static void JCPoint_GMP_16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, const int16 x, const int16 y)
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
        r = (uint8) lround (cos (0.016f * (float64) Iteration + 0.20f) * 127.5f + 127.5f);
		Color_ARGB |= (r << 16);
		g = (uint8) lround (cos (0.013f * (float64) Iteration + 0.15f) * 127.5f + 127.5f);
		Color_ARGB |= (g << 8);
		b = (uint8) lround (cos (0.010f * (float64) Iteration + 0.10f) * 127.5f + 127.5f);
		Color_ARGB |= b;
  	}

  	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* JVLine_GMP_16_32bit() */
static void JVLine_GMP_16_32bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 b1, const int16 b2, const int16 x)
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
	    	r = (uint8) lround (cos (0.016f * (float64) Iteration + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Iteration + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Iteration + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
      	}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);

      	WritePixelColor (Rp, x, y, Color_ARGB);
    }      
}

/* JHLine_GMP_16_32bit() */
static void JHLine_GMP_16_32bit (struct RastPort *Rp, struct MandelChunk *MandelInfo, uint8 *PixelLine, const int16 a1, const int16 a2, const int16 y)
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
	        r = (uint8) lround (cos (0.016f * (float64) Iteration + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Iteration + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Iteration + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
   	}
}

uint32 Load_GMP (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint32 *PixelVecBase, uint32 P, int16 HiTrueColor) 
{
  int16 X, Y;
  uint32 ResX, ResY, Color, Color_ARGB;

	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;		

	if (DONE [P] & Loaded) return DATA [P];
	
	X = P % ResX;
    Y = P / ResX;
	
	(*C_POINT_GMP) (MandelInfo, Rp, X, Y);
			
    DONE [P] |= Loaded;
    
	return DATA [P] = ((HiTrueColor) ? ReadPixelColor (Rp, X, Y) : ReadPixel (Rp, X, Y));
}

void Scan_GMP (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint32 *PixelVecBase, uint32 QueueSize, uint32 P, int16 HiTrueColor)
{
  uint32 Center;
  int32 LL, RR, UU, DD;
  int32 L, R, U, D; 
  uint32 ResX, ResY;
  uint32 X, Y;

	Center = Load_GMP (MandelInfo, Rp, PixelVecBase, P, HiTrueColor);
	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;
	X = P % ResX;
	Y = P / ResX;
	LL = X >= 1L;
	UU = Y >= 1L;
	RR = X < ResX - 1L;
	DD = Y < ResY - 1L;
	
	/* booleans */
   	L = LL && Load_GMP (MandelInfo, Rp, PixelVecBase, P - 1L, HiTrueColor) != Center;
    R = RR && Load_GMP (MandelInfo, Rp, PixelVecBase, P + 1L, HiTrueColor) != Center;
    U = UU && Load_GMP (MandelInfo, Rp, PixelVecBase, P - ResX, HiTrueColor) != Center;
    D = DD && Load_GMP (MandelInfo, Rp, PixelVecBase, P + ResX, HiTrueColor) != Center;
	
    /* process the queue (which is actuaLLy a ring buffer) */
    if (L) AddQueue (QueueSize, P - 1L);
    if (R) AddQueue (QueueSize, P + 1L);
    if (U) AddQueue (QueueSize, P - ResX);
    if (D) AddQueue (QueueSize, P + ResX);
	
    /* the corner pixels (nw,ne,sw,se) are also neighbors */
    if ((UU && LL) && (L || U)) AddQueue (QueueSize, P - ResX - 1L);
    if ((UU && RR) && (R || U)) AddQueue (QueueSize, P - ResX + 1L);
    if ((DD && LL) && (L || D)) AddQueue (QueueSize, P + ResX - 1L);
    if ((DD && RR) && (D || D)) AddQueue (QueueSize, P + ResX + 1L);
}
/* end boundary trace functions */

int16 BoundaryDraw_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem,
                    	uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem)
{
  uint16 MyCode;
  uint32 MyClass;  
  struct IntuiMessage *Message;

  int16 Flag = 0, LoColor = 0, HiColor = 0, TrueColor = 0;
  uint32 P, QueueTail, QueueSize;
  uint32 X, Y, ResX, ResY;
 
 	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;
	
	QueueTail = 0L;
	QueueSize = sizeof (int32) * ResX * ResY * 4L;
	TrueColor = (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MAX_DEPTH) ? 1 : 0;
	HiColor = (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MID_DEPTH) ? 1 : 0;	
	LoColor = (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MIN_DEPTH) ? 1 : 0;
	
	/* allocation check */
	if (! AllocateBoundary (ResX, ResY)) return FALSE;
    
	/* (1) begin by adding the screen edges into the queue */
    for (Y = 0L; Y < ResY; ++Y) 
	{
        AddQueue (QueueSize, Y * ResX /*+ 0L*/);		
        AddQueue (QueueSize, Y * ResX + (ResX - 1L));
    }
	
    for (X = 1L; X < (ResX - 1L); ++X) 
	{
        AddQueue (QueueSize, /*0L*Width* +*/ X);
        AddQueue (QueueSize, (ResY - 1L) * ResX + X);
    }
	
	/* (2) process the queue (which is actually a ring buffer) */	
    while (QueueTail != QueueHead) 
	{
        if ((QueueHead <= QueueTail) || (++Flag & 3))
		{
            P = QUEUE [QueueTail++];
            if (QueueTail == QueueSize) QueueTail = 0L;
        } 
		
		else P = QUEUE [--QueueHead];
        
		Scan_GMP (MandelInfo, Win->RPort, PixelVecBase, QueueSize, P, TrueColor || HiColor);
		
		if (Win->UserPort->mp_SigBit)
  		{
     		while (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort))
     		{
     			MyClass = Message->Class;
     		    MyCode  = Message->Code;
        		GT_ReplyIMsg ((struct IntuiMessage *) Message);

        		switch (MyClass)
				{
        			case IDCMP_MENUPICK : 
					{
						if (MyCode != MENUNULL)
                    	{
							if (ProcessMenu (MandelInfo, Win, ARGBMem, RGBMem, PixMem, PixelVecBase, RenderMem, GfxMem, MyCode) & STOP_MSG) 
							{
                    			DisplayBeep (Win->WScreen);					  							
								DeallocateBoundary();										
								return TRUE;
							}
						}
                    }
					break;

                    case IDCMP_RAWKEY:
                	{						
						if (MyCode == RAW_ESC)
						{																				
    	               		DisplayBeep (Win->WScreen);
							DeallocateBoundary();
							if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																				
    	               		return TRUE;    	           			
						}																		
					}
					break;

        			case IDCMP_CLOSEWINDOW : 
					{
                    	DisplayBeep (Win->WScreen);					  					
						DeallocateBoundary();									
						return TRUE;
					}	
					break;
        		}
     		}
  		}
    }
						
    /* (3) lastly, fill uncalculated areas with neighbor color */
    for (P = 0L; P < ResX * ResY - 1L; ++P) 
	{
        if (DONE [P] & Loaded)
		{
        	if (! (DONE [P + 1L] & Loaded)) 
			{
            	DATA [P + 1L] = DATA [P];
				
				if (TrueColor || HiColor) WritePixelColor (Win->RPort, (P + 1L) % ResX, (P + 1L) / ResX, DATA [P]);
						
				else if (LoColor)
				{			
					SetAPen (Win->RPort, DATA [P]);			
					WritePixel (Win->RPort, (P + 1L) % ResX, (P + 1L) / ResX);
				}		
					
				DONE [P + 1L] |= Loaded;
        	}
		}
    }
	
	DeallocateBoundary();	
	return TRUE;
}

/* RectangleDraw_GMP() */
static int16 RectangleDraw_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem, uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
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
		  				if (ProcessMenu (MandelInfo, Win, ARGBMem, RGBMem, PixMem, PixelVecBase, RenderMem, GfxMem, MyCode) & STOP_MSG) 
						{
                    		DisplayBeep (Win->WScreen);					  
							return TRUE;
						}
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
							BlinkRect (Win, a1, b1, a2, b2);
							BlinkRect (Win, a1, b1, a2, b2);	
							if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																										
		  					return FALSE;
						}
						break;
						
	      				case RAW_ESC:
						{
		  					DisplayBeep (Win->WScreen);
							if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																				
		  					return TRUE;
						}
						break;
	      			}
				}
				break;

	    		case IDCMP_CLOSEWINDOW:
				{
					DisplayBeep (Win->WScreen);					  				
	      			return TRUE;
				}
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
  	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, b1, halfx, halfy)) return TRUE;

  	(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, halfx + 1, a2 - 1, halfy);
  	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, b1, a2, halfy)) return TRUE;

  	(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, halfy + 1, b2 - 1, halfx); 
	
	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, halfy, halfx, b2)) return TRUE;
  	if (RectangleDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, halfy, a2, b2)) return TRUE;

  	return FALSE;
}

/* BruteDraw_GMP() */
static int16 BruteDraw_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem, uint32 * PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  uint16 MyCode;
  int16 helpy;
  uint32 MyClass;

   	for (helpy = b1; helpy <= b2; helpy++)
	{
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
			  				if (ProcessMenu (MandelInfo, Win, ARGBMem, RGBMem, PixMem, PixelVecBase, RenderMem, GfxMem, MyCode) & STOP_MSG) 
							{
    	                		DisplayBeep (Win->WScreen);					  
								return TRUE;
							}
						}
					}	
		      		break;
	
		    		case IDCMP_RAWKEY:
		      		{
						switch (MyCode)
						{
							case RAW_TAB:    		            	
    	            		{								
                    			BlinkRect (Win, a1, helpy-1, a2, helpy);
								helpy += 19;
								if (helpy > b2) helpy = b2;
                    			BlinkRect (Win, a1, helpy-1, a2, helpy);
								if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																													
							}
							break;
							
		      				case RAW_ESC:
							{
			  					DisplayBeep (Win->WScreen);
								if (BENCHMARK_FAIL == FALSE) BENCHMARK_FAIL = TRUE;																					
			  					return TRUE;
							}
							break;
		      			}
					}
					break;
	
		    		case IDCMP_CLOSEWINDOW:
					{
						DisplayBeep (Win->WScreen);					  				
		      			return TRUE;
					}
					break;
		    	}
			}
    	}

  		(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, a1, a2, helpy); 
	}
  
   	return FALSE;
}

/* CalcFractal_GMP() */
void CalcFractal_GMP (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem, uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem)
{
  	if (MandelInfo->Flags & JULIA_BIT)
  	{  
      	if ((GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MAX_DEPTH) || (GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MID_DEPTH))
	  	{
          	C_POINT_GMP = JCPoint_GMP_16_32bit;
	      	H_LINE_GMP = JHLine_GMP_16_32bit;
	      	V_LINE_GMP = JVLine_GMP_16_32bit;
	  	}
	  
	  	else if (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MIN_DEPTH)
	  	{
      	  	C_POINT_GMP = JCPoint_GMP;
      	  	H_LINE_GMP = JHLine_GMP;
	      	V_LINE_GMP = JVLine_GMP;
	  	}    
  	}
	
  	else if (MandelInfo->Flags & MANDEL_BIT)
    {
		if ((GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MAX_DEPTH) || (GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MID_DEPTH))
    	{
   		   	C_POINT_GMP = MCPoint_GMP_16_32bit;
	       	H_LINE_GMP = MHLine_GMP_16_32bit;
	       	V_LINE_GMP = MVLine_GMP_16_32bit;
  		}
		
       	else if (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MIN_DEPTH)

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

	if ((MandelInfo->Flags & BOUNDARY_BIT) && (MandelInfo->Flags & MANDEL_BIT))
	{ 
		BoundaryDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem);
	}
    
	else if (MandelInfo->Flags & TILING_BIT)
	{
		// draw perimeter
  		(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
  		(*H_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
  		(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
  		(*V_LINE_GMP) (Win->RPort, MandelInfo, PixMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);
		// start divide et impera recursively!
  		RectangleDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	}
	
	else if (MandelInfo->Flags & BRUTE_BIT)
	{
		BruteDraw_GMP (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	}
}
