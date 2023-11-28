/* Gfx Render 
** created by Dino Papararo v 1.1 03 Dic 2021
** Now (*COLORREMAP) is extern to to avoid conflicts 09/10*2022 dpapararo
*/

#include <exec/types.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>

#include <GMP/gmp.h>
#include "Headers/FlashMandel.h"

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2, gcre3,
  		gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp, gdist, gmaxdist,
  		gincremreal, gincremimag, gpzr, gpzi;

uint32 (*COLORREMAP) (const float64, const float64, const float64, const float64, const float64);

void (*C_POINT) (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);
void (*H_LINE) (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
void (*V_LINE) (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);

extern void Scan (struct MandelChunk *, struct RastPort *, uint32 *, uint32, uint32 , int16);
extern void AddQueue (uint32, uint32);
extern int16 AllocateBoundary (uint32, uint32);
extern void DeallocateBoundary (void); 

uint32 LinearRemap (const float64, const float64, const float64, const float64, const float64);
uint32 LogRemap (const float64, const float64, const float64, const float64, const float64);
uint32 RepeatedRemap (const float64, const float64, const float64, const float64, const float64);
uint32 SquareRootRemap (const float64, const float64, const float64, const float64, const float64);
uint32 OneRemap (const float64, const float64, const float64, const float64, const float64);
uint32 TwoRemap (const float64, const float64, const float64, const float64, const float64);
uint32 ThreeRemap (const float64, const float64, const float64, const float64, const float64);
uint32 FourRemap (const float64, const float64, const float64, const float64, const float64);

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

extern uint32 QueueHead;	
extern enum { Loaded = 1, Queued = 2 };

	/* Normalization formula to linearly rescale data values once having observed
   	Min and Max, into a new arbitrary range from NewMin to NewMax */
	/* NewValue = (NewMax - NewMin) / (Max - Min) * (Value - Max) + NewMax;
   	or  NewValue = (NewMax - NewMin) / (Max - Min) * (Value - Min) + NewMin; */
	/* To speedup calcs use:
   	a = (NewMax - NewMin) / (Max - Min); b = NewMax - a * Max; NewValue = a * Value + b;
   	note: for b you can use also b = NewMin - (a * Min); */

uint32 LinearRemap (const float64 Value, const float64 Min, const float64 Max,
         			const float64 NewMin, const float64 NewMax)
{   /* linear */
  	return (lround ((NewMax - NewMin) / (Max - Min) * (Value - Min) + NewMin));
}

uint32 LogRemap (const float64 Value, const float64 Min, const float64 Max,
      				const float64 NewMin, const float64 NewMax)
{   /* log (x) */
  	return (lround ((NewMax - NewMin) / (log10 (Max) - log10 (Min)) * (log10 (Value) - log10 (Min)) + NewMin));
}

uint32 RepeatedRemap (const float64 Value, const float64 Min, const float64 Max,
       					const float64 NewMin, const float64 NewMax)
{   /* modulo (x) */
  	return (lround (fmod (Value, (NewMax - NewMin + 1.0)) + NewMin));    /* (x % 251) -> 0<=x<=250 */
}

uint32 SquareRootRemap (const float64 Value, const float64 Min, const float64 Max, const float64 NewMin, const float64 NewMax)
{   /* sqrt (x) */
  	return (lround ((NewMax - NewMin) / (sqrt (Max) - sqrt (Min)) * (sqrt (Value) - sqrt (Min)) + NewMin));
}

uint32 OneRemap (const float64 Value, const float64 Min, const float64 Max, const float64 NewMin, const float64 NewMax)
{   /* x² */
  	return (lround (((NewMax - NewMin) / ((Max * Max) - (Min * Min)) * ((Value * Value) - (Min * Min)) + NewMin)));
}

uint32 TwoRemap (const float64 Value, const float64 Min, const float64 Max,
       			const float64 NewMin, const float64 NewMax)
{   /* sqrt (x³) */
  	return (lround ((NewMax - NewMin) / (sqrt (Max * Max * Max) - sqrt (Min * Min * Min)) * (sqrt (Value * Value * Value) - sqrt (Min * Min * Min)) + NewMin));
}

uint32 ThreeRemap (const float64 Value, const float64 Min, const float64 Max,
      				 const float64 NewMin, const float64 NewMax)
{   /* sqrt(log10(x²)) */
  	return (lround ((NewMax - NewMin) / (sqrt (log10 (Max * Max)) - sqrt (log10 (Min * Min))) * sqrt (log10 ((Value * Value)) - sqrt (log10 (Min * Min))) + NewMin));
}

uint32 FourRemap (const float64 Value, const float64 Min, const float64 Max,
      				 const float64 NewMin, const float64 NewMax)
{   /* small grayscale based un default GUI colors */
  	return (((uint32) Value % 3) + 1);
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

#ifdef USE_ALTIVEC_ASM
  	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim);
#elif USE_ALTIVEC_MATH
  	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim);
#endif 
		  
  	Color = *(PixelVecBase + 0);
#elif USE_POWERPC_MATH
  	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
  	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
	Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_ALTIVEC_MATH */
  
	if (Color) Color = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);

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

#ifdef USE_ALTIVEC_ASM
  	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim);
#elif USE_ALTIVEC_MATH
  	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim);
#endif

  	Color = *(PixelVecBase + 0);
#elif USE_POWERPC_MATH
  	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
  	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_SPE_MATH
	Color = MandelnSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_ALTIVEC_MATH */

	Color_ARGB = 0xff000000;
  
	if (Color)
	{
        r = (uint8) lround ((sin(0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f));
		Color_ARGB |= (r << 16);
		g = (uint8) lround ((sin(0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f));
		Color_ARGB |= (g << 8);
		b = (uint8) lround ((sin(0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f));
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

#ifdef USE_ALTIVEC_ASM
      	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
			  				Cre,(float32) mpf_get_d (gcim), Cre,(float32) mpf_get_d (gcim1), 
			  				Cre,(float32) mpf_get_d (gcim2), Cre,(float32) mpf_get_d (gcim3));
#elif USE_ALTIVEC_MATH
      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
			  			Cre,(float32) mpf_get_d (gcim), Cre,(float32) mpf_get_d (gcim1), 
			  			Cre,(float32) mpf_get_d (gcim2), Cre,(float32) mpf_get_d (gcim3));
#endif

    	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 0))
		{
	  		*(PixelVecBase + 0) = COLORREMAP ((float64) *(PixelVecBase + 0), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray-- = (uint8) *(PixelVecBase + 0);
      	y--;

      	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 1))
		{
	  		*(PixelVecBase + 1) = COLORREMAP ((float64) *(PixelVecBase + 1), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      
	  	*TmpArray-- = (uint8) *(PixelVecBase + 1);
      	y--;

      	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 2))
		{
	  		*(PixelVecBase + 2) = COLORREMAP ((float64) *(PixelVecBase + 2), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray-- = (uint8) *(PixelVecBase + 2);
      	y--;

      	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 3))
		{
	  		*(PixelVecBase + 3) = COLORREMAP ((float64) *(PixelVecBase + 3), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray-- = (uint8) *(PixelVecBase + 3);
      	y--;
    }
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

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
#endif
    	if (Color)
		{
	  		*TmpArray-- = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}

    	else *TmpArray-- = 0;

    	/* Cim += gincremimag; */
    	mpf_add (gcim, gcim, gincremimag);
  	}
#endif

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

#ifdef USE_ALTIVEC_ASM
    	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
	  						Cre,(float32) mpf_get_d (gcim), Cre,(float32) mpf_get_d (gcim1), 
	  						Cre,(float32) mpf_get_d (gcim2), Cre,(float32) mpf_get_d (gcim3));
#elif USE_ALTIVEC_MATH
    	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
	  					Cre,(float32) mpf_get_d (gcim), Cre,(float32) mpf_get_d (gcim1), 
	  					Cre,(float32) mpf_get_d (gcim2), Cre,(float32) mpf_get_d (gcim3));
#endif

		Color_ARGB = 0xff000000;
	  
		if (*(PixelVecBase + 0))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 0) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 0) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 0) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
	   
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;

    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 1) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 1) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 1) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
	
    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 2) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 2) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 2) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;

    	if (y < b1)	break;
		Color_ARGB = 0xff000000;	
	
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 3) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 3) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 3) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
	    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
  	}
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

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
#endif

		Color_ARGB = 0xff000000;

		if (Color)
		{
	    	r = (uint8) lround ((sin(0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}

		/* Cim += gincremimag; */
		mpf_add (gcim, gcim, gincremimag);

		WritePixelColor (Rp, x, y, Color_ARGB);
  	}
#endif
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

#ifdef USE_ALTIVEC_ASM
      	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
							(float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		      				(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
#elif USE_ALTIVEC_MATH
      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
						(float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		      			(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
#endif

      	if (*(PixelVecBase + 0))
		{
	  		*(PixelVecBase + 0) = COLORREMAP ((float64) *(PixelVecBase + 0), 1.0,	(float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      
	  	*TmpArray++ = (uint8) *(PixelVecBase + 0);
      	x++;

      	if (x > a2)	break;
      	
		if (*(PixelVecBase + 1))
		{
	  		*(PixelVecBase + 1) =  COLORREMAP ((float64) *(PixelVecBase + 1), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      
	  	*TmpArray++ = (uint8) *(PixelVecBase + 1);
      	x++;

      	if (x > a2)	break;
      	if (*(PixelVecBase + 2))
		{
	  		*(PixelVecBase + 2) = COLORREMAP ((float64) *(PixelVecBase + 2), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
		
      	*TmpArray++ = (uint8) *(PixelVecBase + 2);
      	x++;

      	if (x > a2)	break;
      	
		if (*(PixelVecBase + 3))
		{
	  		*(PixelVecBase + 3) = COLORREMAP ((float64) *(PixelVecBase + 3), 1.0,	(float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      
	  	*TmpArray++ = (uint8) *(PixelVecBase + 3);
      	x++;
	}
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

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
#endif

      	if (Color)
		{
	  		Color = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
      	*TmpArray++ = (uint8) Color;
    }
#endif

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

#ifdef USE_ALTIVEC_ASM
      	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
							(float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		      				(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
#elif USE_ALTIVEC_MATH
      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, 
						(float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim,
		      			(float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
#endif
	    Color_ARGB = 0xff000000;

		if (*(PixelVecBase + 0))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 0) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 0) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 0) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;

		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 1) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 1) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 1) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;
	
        if (x > a2)	break;
		Color_ARGB = 0xff000000;	

		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 2) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 2) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 2) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;
		
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 3) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 3) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 3) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;
  	}
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

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
#endif
  	Color_ARGB = 0xff000000;

      	if (Color)
      	{
	        r = (uint8) lround ((sin(0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
   	}
#endif
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
#elif USE_POWERPC_MATH
  	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
  	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
	Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif
  	if (Color) Color = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);

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
#elif USE_POWERPC_MATH
  	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
  	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_SPE_MATH
	Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif
  
	Color_ARGB = 0xff000000;
  
	if (Color)
	{
        r = (uint8) lround ((sin(0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f));
		Color_ARGB |= (r << 16);
		g = (uint8) lround ((sin(0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f));
		Color_ARGB |= (g << 8);
		b = (uint8) lround ((sin(0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f));
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
  
      	if (*(PixelVecBase + 0))
		{
	  		*(PixelVecBase + 0) = COLORREMAP ((float64) *(PixelVecBase + 0), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray-- = (uint8) *(PixelVecBase + 0);
      	y--;
      	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 1))
		{
	  		*(PixelVecBase + 1) = COLORREMAP ((float64) *(PixelVecBase + 1), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      
	  	*TmpArray-- = (uint8) *(PixelVecBase + 1);
      	y--;	
      	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 2))
		{
	  		*(PixelVecBase + 2) =  COLORREMAP ((float64) *(PixelVecBase + 2), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray-- = (uint8) *(PixelVecBase + 2);
      	y--;
      	if (y < b1)	break;
	  
      	if (*(PixelVecBase + 3))
		{
	  		*(PixelVecBase + 3) = COLORREMAP ((float64) *(PixelVecBase + 3), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      
	  	*TmpArray-- = (uint8) *(PixelVecBase + 3);
      	y--;
    }
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

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
#endif
    	if (Color)
		{
	  		*TmpArray-- = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}

    	else *TmpArray-- = 0;

    	/* Cim += gincremimag; */
    	mpf_add (gcim, gcim, gincremimag);
  	}
#endif

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
 
		Color_ARGB = 0xff000000;
	  
		if (*(PixelVecBase + 0))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 0) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 0) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 0) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
	   
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
	
    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 1) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 1) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 1) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
	
    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 2) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 2) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 2) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;

    	if (y < b1)	break;
		Color_ARGB = 0xff000000;	
	
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 3) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 3) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 3) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
	    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
  	}
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

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
#endif
 
		Color_ARGB = 0xff000000;

		if (Color)
		{
	    	r = (uint8) lround ((sin(0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}

		/* Cim += gincremimag; */
		mpf_add (gcim, gcim, gincremimag);

		WritePixelColor (Rp, x, y, Color_ARGB);
  	}
#endif
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
 
       	if (*(PixelVecBase + 0))
		{
	  		*(PixelVecBase + 0) = COLORREMAP ((float64) *(PixelVecBase + 0), 1.0,	(float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray++ = (uint8) *(PixelVecBase + 0);
      	x++;
      	if (x > a2)	break;

      	if (*(PixelVecBase + 1))
		{
	  		*(PixelVecBase + 1) = COLORREMAP ((float64) *(PixelVecBase + 1), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray++ = (uint8) *(PixelVecBase + 1);
      	x++;
      	if (x > a2)	break;

      	if (*(PixelVecBase + 2))
		{
	  		*(PixelVecBase + 2) = COLORREMAP ((float64) *(PixelVecBase + 2), 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray++ = (uint8) *(PixelVecBase + 2);	
      	x++;
      	if (x > a2)	break;
      	
		if (*(PixelVecBase + 3))
		{
	  		*(PixelVecBase + 3) = COLORREMAP ((float64) *(PixelVecBase + 3), 1.0,	(float64) MandelInfo->Iterations, 4.0, 255.0);
		}
      	
		*TmpArray++ = (uint8) *(PixelVecBase + 3);
      	x++;
    }
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
    {
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre),	mpf_get_d (gjkim));
#elif USE_SPE_MATH
		Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif
 	   	if (Color)
		{
		  	*TmpArray++ = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
		}

  	    else *TmpArray++ = 0;

  	    /* Cre += gincremreal; */
  	    mpf_add (gcre, gcre, gincremreal);

    }
#endif
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
 
	    Color_ARGB = 0xff000000;

		if (*(PixelVecBase + 0))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 0) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 0) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 0) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;

		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 1) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 1) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 1) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;
	
        if (x > a2)	break;
		Color_ARGB = 0xff000000;	

		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 2) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 2) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 2) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;
		
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016f * (float64) *(PixelVecBase + 3) + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) *(PixelVecBase + 3) + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) *(PixelVecBase + 3) + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;
  	}
#elif USE_POWERPC_MATH || USE_C_MATH || USE_SPE_MATH

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
  	{
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre),	mpf_get_d (gjkim));
#elif USE_SPE_MATH
		Color = JulianSPE (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif
	  	Color_ARGB = 0xff000000;

      	if (Color)
      	{
	        r = (uint8) lround ((sin(0.016f * (float64) Color + 0.20f) * 127.5f + 127.5f));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013f * (float64) Color + 0.15f) * 127.5f + 127.5f));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010f * (float64) Color + 0.10f) * 127.5f + 127.5f));
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

      	WritePixelColor (Rp, x, y, Color_ARGB);
   }
#endif
}

/*  Histogram() */
void Histogram (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *GfxMem, uint32 *RndMem, uint32 *HistogramMem)
{
  uint32 Iterations, Rows, Cols, Result, Total = 0L;
  float64 Normalized;

  	for (Iterations = 1L; Iterations <= MandelInfo->Iterations; Iterations++) // Iterations=number of pixels reached that iteration before bailout
    	Total += *(HistogramMem + Iterations); // Total=sum of all stored values outside mandelbrot or julia set

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

          		*(GfxMem + (Cols * MandelInfo->Width + Rows)) = COLORREMAP (Normalized, 0.0f, 1.0f, 4.0f, 255.0f);
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
						switch (MyCode)
                    	{
							case RAW_ESC:
							{
                    			DisplayBeep (Win->WScreen);					  
								DeallocateBoundary();
                        		return TRUE;
							}
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
