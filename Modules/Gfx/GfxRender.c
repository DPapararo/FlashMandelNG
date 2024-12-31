/* Gfx Render - created by Dino Papararo
**
**	Revision 1.0 03/12/2021 dpapararo
** 	Now (*COLORREMAP) is extern to to avoid conflicts 09/10*2022 dpapararo
**	 
**	Revision 1.1 10/11/2024 dpapararo
**	Added benchmark mode fail control
**
**	Revision 1.2 31/12/2024 dpapararo
**	New COLORREMAP scheme and various fixes
*/

#include <math.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>

#include <GMP/gmp.h>
#include "Headers/FlashMandel.h"

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2, gcre3,
  		gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp, gdist, gmaxdist,
  		gincremreal, gincremimag, gpzr, gpzi;

uint32 (*COLORREMAP) (const uint32, const uint32, const uint32);

void (*C_POINT) (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);
void (*H_LINE) (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
void (*V_LINE) (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);

extern void Scan (struct MandelChunk *, struct RastPort *, uint32 *, uint32, uint32 , int16);
extern void AddQueue (uint32, uint32);
extern int32 AllocateBoundary (uint32, uint32);
extern void DeallocateBoundary (void); 

uint32 LinearRemap (const uint32, const uint32, const uint32);
uint32 LogRemap (const uint32, const uint32, const uint32);
uint32 RepeatedRemap (const uint32, const uint32, const uint32);
uint32 SquareRootRemap (const uint32, const uint32, const uint32);
uint32 OneRemap (const uint32, const uint32, const uint32);
uint32 TwoRemap (const uint32, const uint32, const uint32);
uint32 ThreeRemap (const uint32, const uint32, const uint32);
uint32 FourRemap (const uint32, const uint32, const uint32);

int16 CheckBox (struct RastPort *, const int16, const int16, const int16, const int16);
static int16 RectangleDraw (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16, const int16);
static int16 BruteDraw (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16, const int16);
static int16 BoundaryDraw (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *,uint8 *, uint32 *, uint32 *, uint32 *);
void CalcFractal (struct MandelChunk *,struct Window *,uint8 *,uint8 *,uint8 *,uint8 *,uint32 *,uint32 *,uint32 *);

static void MCPoint (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);
static void JCPoint (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);

static void MHLine (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
static void MVLine (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
static void JHLine (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
static void JVLine (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);

static void MCPoint16_32bit (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);
static void JCPoint16_32bit (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);

static void MHLine16_32bit (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
static void MVLine16_32bit (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
static void JHLine16_32bit (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
static void JVLine16_32bit (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);

extern uint8 *DONE;
extern uint32 *DATA, *QUEUE;
extern int16 BENCHMARK_FAIL;

extern uint32 QueueHead;	
extern enum { Loaded = 1, Queued = 2 };

uint32 LinearRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{   /* linear */	// assumed 8bit 256 colors palette with first 4 reserved
  	uint32 result = lround ((float64) NewRange / (float64) OldRange * (float64) Value) + 4L;
	if (result > 255) result = 255;
	return result;
}

uint32 LogRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{   /* log (x) */	// assumed 8bit 256 colors palette with first 4 reserved
  	uint32 result = (lround) ((float64) NewRange / log10 ((float64) OldRange) * log10 ((float64) Value)) + 4L;
	if (result > 255) result = 255;
	return result;
}

uint32 SquareRootRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{   /* sqrt (x) */	// assumed 8bit 256 colors palette with first 4 reserved
	uint32 result = (lround) ((float64) NewRange / sqrt ((float64) OldRange) * sqrt ((float64) Value)) + 4L;
	if (result > 255) result = 255;
	return result;	
}

uint32 RepeatedRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{   /* modulo (x) */	// assumed 8bit 256 colors palette with first 4 reserved
  	return (uint32) ((Value % NewRange) + 4L);
}

uint32 OneRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{   /* x² */ 	// assumed 8bit 256 colors palette with first 4 reserved
  	uint32 OldRange2 = OldRange * OldRange;
	uint32 Value2 = Value * Value;
  	uint32 result = (lround) ((float64) NewRange / ((float64) OldRange2) * ((float64) Value2)) + 4L;
	if (result > 255) result = 255;
	return result;	
}	

uint32 TwoRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{ 	/* cos (x) */ // assumed 8bit 256 colors palette with first 4 reserved	
	return (uint32) (lround ((125.5f * sin (Value * M_PI / (float64) NewRange) + 125.5f) + 4L));
}

uint32 ThreeRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{	/* tanh (cos (x)) */ // assumed 8bit 256 colors palette with first 4 reserved
	return (uint32) (lround (251.0f * tanh (20.0 * ((cos ((float64) Value * M_PI / (float64) NewRange) * 0.5f) + 0.5f))) + 4L);
}

uint32 FourRemap (const uint32 Value, const uint32 OldRange, const uint32 NewRange)
{   /* small grayscale based un default GUI colors */
  	return (uint32) ((Value % 3) + 1L);
}

/* MCPoint() */
static void MCPoint (struct MandelChunk *MandelInfo, struct RastPort *Rp,
	 			uint32 *PixelVecBase, const int16 x, const int16 y)
{
  uint8 r, g, b;
  uint32 Color;
  
  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH
  	float32 Cre = (float32) mpf_get_d (gcre);
  	float32 Cim = (float32) mpf_get_d (gcim);

  	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim);

  	Color = *(PixelVecBase + 0);
#else 	
#ifdef USE_POWERPC_MATH
  	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
  	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
	Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWEPC_MATH */
#endif /* USE_ALTIVEC_MATH */  

	if (Color) Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);

	SetAPen (Rp, Color);		 
	WritePixel (Rp, x, y);
}

/* MCPoint16_32bit() */
static void MCPoint16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp,
				 	uint32 *PixelVecBase, const int16 x, const int16 y)
{
  uint8 r, g, b;
  uint32 Color, Color_ARGB;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH
  	float32 Cre = (float32) mpf_get_d (gcre);
  	float32 Cim = (float32) mpf_get_d (gcim);
	
  	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim);

  	Color = *(PixelVecBase + 0);
#else
#ifdef USE_POWERPC_MATH
  	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
  	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
	Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWEPC_MATH */
#endif /* USE_ALTIVEC_MATH */

	Color_ARGB = 0xff000000;
  
	if (Color)
	{
        r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
		Color_ARGB |= (r << 16);
		g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
		Color_ARGB |= (g << 8);
		b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
		Color_ARGB |= b;
	}

	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* MVLine() */
static void MVLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, uint32 *PixelVecBase,  
				const int16 b1, const int16 b2, const int16 x)
{
  uint8 *TmpArray = (PixelLine + b2 - b1);
  int16 y;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  uint32 tmp1;
  float32 Cre = (float32) mpf_get_d (gcre);

  	y = b2;

  	while (y >= b1)
    {
      	/* Cim0 = MandelInfo->IMax - gincremimag * (y-0); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 0));
      	mpf_sub (gcim, gimax, gtmp);

      	/* Cim1 = MandelInfo->IMax - gincremimag * (y-1); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 1));
      	mpf_sub (gcim1, gimax, gtmp);

      	/* Cim2 = MandelInfo->IMax - gincremimag * (y-2); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 2));
      	mpf_sub (gcim2, gimax, gtmp);

      	/* Cim3 = MandelInfo->IMax - gincremimag * (y-3); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 3));
      	mpf_sub (gcim3, gimax, gtmp);

      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
			  			Cre,(float32) mpf_get_d (gcim), Cre,(float32) mpf_get_d (gcim1), 
			  			Cre,(float32) mpf_get_d (gcim2), Cre,(float32) mpf_get_d (gcim3));
      	 		
		for (i = 0; i < 4; i++)
		{
			Color = *(PixelVecBase + i);
			
			if (Color)
			{
	  			Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
			}
      	
			*TmpArray-- = (uint8) Color;
		
			if ((--y) < b1) break;
		}
    }
#else
  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
  	{
#ifdef USE_POWERPC_MATH
    	Color =	MandelnPPC (MandelInfo->Iterations, MandelInfo->Power,  mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
      	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
		Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWERPC_MATH */
    	if (Color)
		{
	  		Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
		}

    	*TmpArray-- = (uint8) Color;

    	/* Cim += gincremimag; */
    	mpf_add (gcim, gcim, gincremimag);
  	}
#endif /* USE_ALTIVEC_MATH */

  	WriteChunkyPixels (Rp, x, b1, x, b2, PixelLine, 1);
}

/* MVLine16_32bit() */
static void MVLine16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint8 *PixelLine, 
					uint32 *PixelVecBase, const int16 b1, const int16 b2, const int16 x)
{
  uint8 r, g, b;
  int16 y;
  uint32 Color, Color_ARGB;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  	int16 i;
	float32 Cre = (float32) mpf_get_d (gcre);

  	y = b2;

  	while (y >= b1)
  	{
      	/* Cim0 = MandelInfo->IMax - gincremimag * (y-0); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 0));
      	mpf_sub (gcim, gimax, gtmp);
	
      	/* Cim1 = MandelInfo->IMax - gincremimag * (y-1); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 1));
      	mpf_sub (gcim1, gimax, gtmp);

      	/* Cim2 = MandelInfo->IMax - gincremimag * (y-2); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 2));
      	mpf_sub (gcim2, gimax, gtmp);

      	/* Cim3 = MandelInfo->IMax - gincremimag * (y-3); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 3));
      	mpf_sub (gcim3, gimax, gtmp);

    	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
	  					Cre,(float32) mpf_get_d (gcim), Cre,(float32) mpf_get_d (gcim1), 
	  					Cre,(float32) mpf_get_d (gcim2), Cre,(float32) mpf_get_d (gcim3));

		for (i = 0; i < 4; i++)
		{
			Color_ARGB = 0xff000000;
	  		Color = *(PixelVecBase + i);
			
			if (Color)
			{
	        	r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
				Color_ARGB |= (r << 16);
				g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
				Color_ARGB |= (g << 8);
				b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
				Color_ARGB |= b;
			}
	   
			WritePixelColor (Rp, x, y, Color_ARGB);
		    
			if (--y < b1)	break;
		}
	}
#else
  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
  	{
#ifdef USE_POWERPC_MATH
		Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
      	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
		Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWERPC_MATH */

		Color_ARGB = 0xff000000;

		if (Color)
		{
	    	r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;		
		}

		/* Cim += gincremimag; */
		mpf_add (gcim, gcim, gincremimag);

		WritePixelColor (Rp, x, y, Color_ARGB);
  	}
#endif /* USE_ALTIVEC_MATH */
}

/* MHLine() */
static void MHLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, 
				uint32 *PixelVecBase, const int16 a1, const int16 a2, const int16 y)
{
  uint8 *TmpArray = PixelLine;
  int16 x;
  uint32 Color;
  
  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  float32 Cim = (float32) mpf_get_d (gcim);

  	x = a1;

  	while (x <= a2)
    {
      	/* Cre = MandelInfo->RMin + gincremreal * (x+0); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 0));
      	mpf_add (gcre, grmin, gtmp);
	
      	/* Cre1 = MandelInfo->RMin + gincremreal * (x+1); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 1));
      	mpf_add (gcre1, grmin, gtmp);

      	/* Cre2 = MandelInfo->RMin + gincremreal * (x+2); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 2));
      	mpf_add (gcre2, grmin, gtmp);

      	/* Cre3 = MandelInfo->RMin + gincremreal * (x+3); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 3));
      	mpf_add (gcre3, grmin, gtmp);

      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
						(float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		      			(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);

		for (i = 0; i < 4; i++)
		{
			Color = *(PixelVecBase + i);
      		
			if (Color)
			{
	  			Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
			}
      
	  		*TmpArray++ = (uint8) Color;     		
			
      		if (++x > a2) break;      	
		}					
	}
#else
  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
    {
#ifdef USE_POWERPC_MATH
    	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
      	Color =	Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
		Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWERPC_MATH */

      	if (Color)
		{
	  		Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
		}

      	*TmpArray++ = (uint8) Color;
      	
		/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }
#endif /* USE_ALTIVEC_MATH */

  	WriteChunkyPixels (Rp, a1, y, a2, y, PixelLine, a2 - a1 + 1);
}

/* MHLine16_32bit() */
static void MHLine16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp,
					uint8 *PixelLine, uint32 *PixelVecBase, const int16 a1, const int16 a2, const int16 y)
{ 
  uint8 r, g, b;
  int16 x;
  uint32 Color, Color_ARGB;

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  float32 Cim = (float32) mpf_get_d (gcim);

  	x = a1;

  	while (x <= a2)
  	{
      	/* Cre = MandelInfo->RMin + gincremreal * (x+0); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 0));
      	mpf_add (gcre, grmin, gtmp);

      	/* Cre1 = MandelInfo->RMin + gincremreal * (x+1); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 1));
      	mpf_add (gcre1, grmin, gtmp);

      	/* Cre2 = MandelInfo->RMin + gincremreal * (x+2); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 2));
      	mpf_add (gcre2, grmin, gtmp);

      	/* Cre3 = MandelInfo->RMin + gincremreal * (x+3); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 3));
      	mpf_add (gcre3, grmin, gtmp);

      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
						(float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		      			(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
		
		for (i = 0; i < 4; i++)
		{
			Color_ARGB = 0xff000000;
			Color = *(PixelVecBase + i);
			
			if (Color)
			{
	        	r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
				Color_ARGB |= (r << 16);
				g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
				Color_ARGB |= (g << 8);
				b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
				Color_ARGB |= b;
			}
		    
			WritePixelColor (Rp, x, y, Color_ARGB);
			
        	if (++x > a2) break;
		}
  	}
#else

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
  	{
#ifdef USE_POWERPC_MATH
      	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
      	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
		Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWERPC_MATH */
  		Color_ARGB = 0xff000000;

      	if (Color)
      	{
	        r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
		}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
   	}
#endif /* USE_ALTIVEC_MATH */
}

/* JCPoint() */
static void JCPoint (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint32 *PixelVecBase, const int16 x, const int16 y)
{
  uint8 r, g, b;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH
  	float32 Cre = (float32) mpf_get_d (gcre);
  	float32 Cim = (float32) mpf_get_d (gcim);

  	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim,
		 			(float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));
  	
	Color = *(PixelVecBase + 0);
#else	
#ifdef USE_POWERPC_MATH
  	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
  	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
	Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */
#endif /* USE_ALTIVEC_MATH */
  	
	if (Color) Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);

	SetAPen (Rp, Color);		 
	WritePixel (Rp, x, y);
}

/* JCPoint16_32bit() */
static void JCPoint16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp,
	 				uint32 *PixelVecBase, const int16 x, const int16 y)
{
  uint8 r,g,b;
  uint32 Color,Color_ARGB;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH
  	float32 Cre = (float32) mpf_get_d (gcre);
  	float32 Cim = (float32) mpf_get_d (gcim);

 	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim,
		 			(float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));

 	Color = *(PixelVecBase + 0);
#else 	
#ifdef USE_POWERPC_MATH
  	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
  	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
	Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */
#endif /* USE_ALTIVEC_MATH */
  
	Color_ARGB = 0xff000000;
  
	if (Color)
	{
        r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
		Color_ARGB |= (r << 16);
		g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
		Color_ARGB |= (g << 8);
		b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
		Color_ARGB |= b;
	}

	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* JVLine() */
static void JVLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, uint32 *PixelVecBase, 
				const int16 b1, const int16 b2, const int16 x)
{
  uint8 *TmpArray = (PixelLine + b2 - b1);
  int16 y;
  uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  float32 Cre = (float32) mpf_get_d (gcre);

  	y = b2;

  	while (y >= b1)
    {
      	/* Cim0 = MandelInfo->IMax - gincremimag * (y-0); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 0));
      	mpf_sub (gcim, gimax, gtmp);

      	/* Cim1 = MandelInfo->IMax - gincremimag * (y-1); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 1));
      	mpf_sub (gcim1, gimax, gtmp);

      	/* Cim2 = MandelInfo->IMax - gincremimag * (y-2); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 2));
      	mpf_sub (gcim2, gimax, gtmp);

      	/* Cim3 = MandelInfo->IMax - gincremimag * (y-3); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 3));
      	mpf_sub (gcim3, gimax, gtmp);

	  	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, (float32) mpf_get_d (gcim),
		     			Cre, (float32) mpf_get_d (gcim1), Cre, (float32) mpf_get_d (gcim2), Cre, (float32) mpf_get_d (gcim3), 
						(float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));
  
  		for (i = 0; i < 4; i++)
		{		
      		Color = *(PixelVecBase + i);
			
			if (Color)
			{
	  			Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
			}
      	
			*TmpArray-- = (uint8) Color;
      	
      		if (--y < b1) break;
	  	}		
    }
#else
  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
  	{
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
		Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */
    	if (Color)
		{
	  		Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
		}

    	*TmpArray-- = (uint8) Color;

    	/* Cim += gincremimag; */
    	mpf_add (gcim, gcim, gincremimag);
  	}
#endif /* USE_ALTIVEC_MATH */

  	WriteChunkyPixels (Rp, x, b1, x, b2, PixelLine, 1);
}

/* JVLine16_32bit() */
static void JVLine16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint8 *PixelLine, uint32 *PixelVecBase, 
					const int16 b1, const int16 b2, const int16 x)
{
  uint8 r, g, b;
  int16 y;
  uint32 Color, Color_ARGB;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  float32 Cre = (float32) mpf_get_d (gcre);

  	y = b2;

  	while (y >= b1)
  	{
      	/* Cim0 = MandelInfo->IMax - gincremimag * (y-0); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 0));
      	mpf_sub (gcim, gimax, gtmp);

      	/* Cim1 = MandelInfo->IMax - gincremimag * (y-1); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 1));
      	mpf_sub (gcim1, gimax, gtmp);

      	/* Cim2 = MandelInfo->IMax - gincremimag * (y-2); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 2));
      	mpf_sub (gcim2, gimax, gtmp);

      	/* Cim3 = MandelInfo->IMax - gincremimag * (y-3); */
      	mpf_mul_ui (gtmp, gincremimag, (y - 3));
      	mpf_sub (gcim3, gimax, gtmp);

	  	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, 
						(float32) mpf_get_d (gcim), Cre, (float32) mpf_get_d (gcim1), Cre,
		     			(float32) mpf_get_d (gcim2), Cre, (float32) mpf_get_d (gcim3),
		     			(float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));
 
 		for (i = 0; i < 4; i++)
		{		
			Color_ARGB = 0xff000000;
	  		Color = *(PixelVecBase + i);
		
			if (Color)
			{
	        	r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
				Color_ARGB |= (r << 16);
				g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
				Color_ARGB |= (g << 8);
				b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
				Color_ARGB |= b;
			}
	   
			WritePixelColor (Rp, x, y, Color_ARGB);
		
    		if (--y < b1) break;
		}
  	}
#else
  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
  	{
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
		Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */
 
		Color_ARGB = 0xff000000;

		if (Color)
		{
	    	r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
		}

		/* Cim += gincremimag; */
		mpf_add (gcim, gcim, gincremimag);

		WritePixelColor (Rp, x, y, Color_ARGB);
  	}
#endif /* USE_ALTIVEC_MATH */
}

/* JHLine() */
static void JHLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, uint32 *PixelVecBase, 
				const int16 a1, const int16 a2, const int16 y)
{
  uint8 *TmpArray = PixelLine;
  int16 x;
  uint32 Color;
 
  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  float32 Cim = (float32) mpf_get_d (gcim);

  	x = a1;

  	while (x <= a2)
    {
      	/* Cre = MandelInfo->RMin + gincremreal * (x+0); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 0));
      	mpf_add (gcre, grmin, gtmp);
	
      	/* Cre1 = MandelInfo->RMin + gincremreal * (x+1); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 1));
      	mpf_add (gcre1, grmin, gtmp);

      	/* Cre2 = MandelInfo->RMin + gincremreal * (x+2); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 2));
      	mpf_add (gcre2, grmin, gtmp);

      	/* Cre3 = MandelInfo->RMin + gincremreal * (x+3); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 3));
      	mpf_add (gcre3, grmin, gtmp);

      	JulianAltivec (PixelVecBase, MandelInfo->Iterations,  MandelInfo->Power, (float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		     			(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim,
		     			(float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));
 
       	for (i = 0; i < 4; i++)
		{
			Color = *(PixelVecBase + i);
			
			if (Color)
			{
	  			Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
			}
      	
			*TmpArray++ = (uint8) Color;
      								
      		if (++x > a2) break;
		}		
    }
#else
  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
    {
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
		Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */
 	   	if (Color)
		{
		  	Color = COLORREMAP (Color, MandelInfo->Iterations, 252L);
		}

  	    *TmpArray++ = (uint8) Color;

  	    /* Cre += gincremreal; */
  	    mpf_add (gcre, gcre, gincremreal);

    }
#endif /* USE_ALTIVEC_MATH */
  	WriteChunkyPixels (Rp, a1, y, a2, y, PixelLine, a2 - a1 + 1);
}

/* JHLine16_32bit() */
static void JHLine16_32bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint8 *PixelLine, uint32 *PixelVecBase, 
					const int16 a1, const int16 a2, const int16 y)
{ 
  uint8 r, g, b;
  int16 x;
  uint32 Color, Color_ARGB;

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  int16 i;
  float32 Cim = (float32) mpf_get_d (gcim);

  	x = a1;

  	while (x <= a2)
  	{
      	/* Cre = MandelInfo->RMin + gincremreal * (x+0); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 0));
      	mpf_add (gcre, grmin, gtmp);

      	/* Cre1 = MandelInfo->RMin + gincremreal * (x+1); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 1));
      	mpf_add (gcre1, grmin, gtmp);

      	/* Cre2 = MandelInfo->RMin + gincremreal * (x+2); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 2));
      	mpf_add (gcre2, grmin, gtmp);

      	/* Cre3 = MandelInfo->RMin + gincremreal * (x+3); */
      	mpf_mul_ui (gtmp, gincremreal, (x + 3));
      	mpf_add (gcre3, grmin, gtmp);

      	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, (float32) mpf_get_d (gcre), Cim,
		     			(float32) mpf_get_d (gcre1), Cim, (float32) mpf_get_d (gcre2), Cim,
		     			(float32) mpf_get_d (gcre3), Cim, (float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));
 
	    for (i = 0; i < 4; i++)
		{
			Color_ARGB = 0xff000000;
			Color = *(PixelVecBase + i);
		
			if (Color)
			{
	        	r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
				Color_ARGB |= (r << 16);
				g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
				Color_ARGB |= (g << 8);
				b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
				Color_ARGB |= b;
			}
		    
			WritePixelColor (Rp, x, y, Color_ARGB);			
			
        	if (++x > a2)	break;
		}
  	}
#else
  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
  	{
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
		Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */
	  	Color_ARGB = 0xff000000;

      	if (Color)
      	{
	        r = (uint8) lround (cos (0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f);
			Color_ARGB |= (r << 16);
			g = (uint8) lround (cos (0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f);
			Color_ARGB |= (g << 8);
			b = (uint8) lround (cos (0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f);
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

      	WritePixelColor (Rp, x, y, Color_ARGB);
   }
#endif /* USE_ALTIVEC_MATH */
}

/*  Histogram() */
void Histogram (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *GfxMem, uint32 *RndMem, uint32 *HistogramMem)
{
  uint32 Iterations, Rows, Cols, Result, Color, Total = 0L;
  float64 Normalized;

  	for (Iterations = 1L; Iterations <= MandelInfo->Iterations; Iterations++) // Iterations=number of pixels reached that iteration before bailout
    {	
		Total += *(HistogramMem + Iterations); // Total=sum of all stored values outside mandelbrot or julia set
	}
	
  	for (Cols = MandelInfo->TopEdge; Cols < MandelInfo->Height; Cols++)
    {
      	for (Rows = MandelInfo->LeftEdge; Rows < MandelInfo->Width; Rows++)
    	{
      		if (Result = *(RndMem + (Cols * MandelInfo->Width + Rows)))
        	{
				Normalized = 0.0f;
			
          		for (Iterations = 1L; Iterations <= Result; Iterations++)
        		{
          			Normalized += (float64) (*(HistogramMem + Iterations)) / (float64) Total;
        		}

          		Color = (uint32) lround (Normalized * 100000.0f); // Normalize for new ColorRemap algorithm
				
				*(GfxMem + (Cols * MandelInfo->Width + Rows)) = COLORREMAP (Color, 100000L, 252L);
        	}
    	}
    }

  	WriteChunkyPixels (Win->RPort, MandelInfo->LeftEdge, MandelInfo->TopEdge,
             MandelInfo->Width - 1, MandelInfo->Height - 1, GfxMem, MandelInfo->Modulo);
}

static int16 BoundaryDraw (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem,
                    	uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem)
{
  uint16 MyCode;
  uint32 MyClass;
  struct IntuiMessage *Message;

  int16 Flag = 0, LoColor = 0, HiColor = 0, TrueColor = 0;
  uint32 Color, P, QueueTail, QueueSize;
  uint32 X, Y, ResX, ResY;
 
 	ResX = MandelInfo->Width - MandelInfo->LeftEdge;
 	ResY = MandelInfo->Height - MandelInfo->TopEdge;
	
	QueueTail = 0L;
	QueueSize = sizeof (int32) * ResX * ResY * 4L;
	TrueColor = (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MAX_DEPTH) ? 1 : 0;
	HiColor = (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MID_DEPTH) ? 1 : 0;	
	LoColor = (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MIN_DEPTH) ? 1 : 0;
	
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
        
		Scan (MandelInfo, Win->RPort, PixelVecBase, QueueSize, P, TrueColor || HiColor);
		
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

/* BruteDraw() */
static int16 BruteDraw (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem,
                    	uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  int16 helpy;
  uint16 MyCode;
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

    	        switch (MyClass) // for speed reasons return without jumps at end of function!
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

 	    (*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, a1, a2, helpy); 
	}

    return FALSE;
}

/* CheckBox() */
int16 CheckBox (struct RastPort *Rp, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  const uint32 Color = ReadPixelColor (Rp, a1, b1);
  int16 Index;

    if (Color != ReadPixelColor (Rp, a2, b2)) return FALSE; // for speed reasons exit now no jumps at end of function!
    if (Color != ReadPixelColor (Rp, a2, b1)) return FALSE;
    if (Color != ReadPixelColor (Rp, a1, b2)) return FALSE;
    if (Color != ReadPixelColor (Rp, (a1 + a2) / 2, (b1 + b2) / 2)) return FALSE; /* check also center point */

    for (Index = a1 + 1; Index < a2; Index++)
  	{
        if (Color != ReadPixelColor (Rp, Index, b1)) return FALSE;
        if (Color != ReadPixelColor (Rp, Index, b2)) return FALSE;
  	}

	for (Index = b1 + 1; Index < b2; Index++)
    {
    	if (Color != ReadPixelColor (Rp, a1, Index)) return FALSE;
        if (Color != ReadPixelColor (Rp, a2, Index)) return FALSE;
    }

    return TRUE;
}	

/* RectangleDraw() */
static int16 RectangleDraw (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem,
                    	uint32 *PixelVecBase, uint32 *RenderMem, uint32 * HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  int16 helpx, helpy, halfx, halfy;
  uint16 MyCode;
  uint32 MyClass, ColorBox, Color;
	
	helpy = b2 - b1; // catch edge case, if difference is equal to 1 we have a line already computed 
    if (helpy < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!	
    helpx = a2 - a1; // catch edge case, if difference is equal to 1 we have a line already computed 
	if (helpx < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!

    if (Win->UserPort->mp_SigBit)
    {
        while (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort))
        {
        	MyClass = Message->Class;
            MyCode = Message->Code;
            GT_ReplyIMsg ((struct IntuiMessage *) Message);

            switch (MyClass) // for speed reasons return without jumps at end of function!
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

	if ((helpx <= MINLIMIT2) && (helpy <= MINLIMIT2)) // intercept edge cases up to 4x4 (MINLIMIT2 == 3) pixels block and return
	{
		for (helpy = b1 + 1; helpy < b2; helpy++)
			for (helpx = a1 + 1; helpx < a2; helpx++)
			    (*C_POINT) (MandelInfo, Win->RPort, PixelVecBase, helpx, helpy); /* draw center points */
 	
		return FALSE; 
	}

    halfx = (a1 + a2) / 2; // x center point coords
    halfy = (b1 + b2) / 2; // y center point coords

    (*C_POINT) (MandelInfo, Win->RPort, PixelVecBase, halfx, halfy); /* draw center point */

    if (CheckBox (Win->RPort, a1, b1, a2, b2))
    {
        RectFillColor (Win->RPort, a1 + 1, b1 + 1, a2 - 1, b2 - 1, ReadPixelColor (Win->RPort, a2, b1));
        return FALSE;
    }

    (*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, a1 + 1, halfx - 1, halfy); // don't recalc center point
    (*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, b1 + 1, halfy - 1, halfx); // don't recalc center point

    if (RectangleDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, b1, halfx, halfy)) return TRUE; // rectangle 1 sx upper
    (*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, halfx + 1, a2 - 1, halfy);

    if (RectangleDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, b1, a2, halfy)) return TRUE; // rectangle 2 dx upper
    (*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, halfy + 1, b2 - 1, halfx);

    if (RectangleDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, halfy, halfx, b2)) return TRUE; // rectangle 3 sx bottom
    if (RectangleDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, halfy, a2, b2)) return TRUE; // rectangle 4 dx bottom

    return FALSE;
}

/* CalcFractal() */
void CalcFractal (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *RGBMem, uint8 *PixMem, uint8 *GfxMem,
         			uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem)
{
	if (MandelInfo->Flags & JULIA_BIT)
  	{
      	if ((GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MAX_DEPTH) ||	(GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MID_DEPTH))
		{
          	C_POINT = JCPoint16_32bit;
          	H_LINE = JHLine16_32bit;
          	V_LINE = JVLine16_32bit;
      	}
      
       	else if (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MIN_DEPTH)      	
      	{
           	C_POINT = JCPoint;
            H_LINE = JHLine;
          	V_LINE = JVLine;
      	} 
  	}

  	else if (MandelInfo->Flags & MANDEL_BIT)
  	{  
           	if ((GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MAX_DEPTH) || (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MID_DEPTH))
        	{
          		C_POINT = MCPoint16_32bit;
           		H_LINE = MHLine16_32bit;
           		V_LINE = MVLine16_32bit;
          	}
 			
          	else if (GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH) == MIN_DEPTH)
			{
              	C_POINT = MCPoint;
              	H_LINE = MHLine;
           		V_LINE = MVLine;
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
		BoundaryDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem);
	}
    
	else if (MandelInfo->Flags & TILING_BIT)
	{
		// draw perimeter
  		(*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
  		(*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
  		(*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
  		(*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);
		// start divide et impera recursively!
  		RectangleDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge,
         				MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	}
	
	else if (MandelInfo->Flags & BRUTE_BIT)
	{
		BruteDraw (MandelInfo, Win, ARGBMem, RGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge,
    			MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
	}
}
