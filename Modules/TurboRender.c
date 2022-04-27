/**************************************************************************************************************
**  Coded by Dino Papararo
**  A bidimensional matrix[][] value can be accessed as vector using the following formula
**  Index = *(base + (row * tot_colums + colum));
**  using cpu memory instead of gfx memory improves speed a lot reducing also rom calls
**  overheads caused by readpixel, writepixel, etc.. functions.
**  First V1.0 Version 17-December-2001
**  Added code for Mandel & Julia formulas Z = (Z^(2^n)) + C   (1 <= n <= 11)
**  Added support for AmigaOS4, Edgar Schwan, 12.02.2004
**  Fixed Turbomode under 68K version forcing the Mandel&Julia C routine, Dino 14-11-2006
**  Fixed definitively Turbomode, now is possible to use optimized 68K asm version, Dino Papararo 28-Jan-2010
**  V1.6 Cleaned non PPC + OS4 code - Added Altivec Julia support 4.jun.2018 dpapararo
**  V2.1 added USE_POWERPC_MATH function and cleanup types - Dino Papararo 20 Jan 2020
**  2.3 vaious fixes - Dino Papararo 19 mar 2020
**  2.5 modified rendering recursion routine and gained some speedup
**  2.6 rearranged #ifdef sequence, rearranged COLORREMAP, removed FIRSTRENDER
**      Used full math pointers to assign GfxMem values -> nore spped gain - 05 jan 2021 dpapararo
**  2.7 Small fixes and optimizations - Histogram coloring in test - 09-01-2021 dpapararo
**  2.8 Small fixes and optimizations - implemented Histogram coloring - 18-01-2021 dpapararo
**  2.9 Small fixes in prototypes parameters datatypes - 24-01-2021 dpapararo
**************************************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <GMP/gmp.h>
#include "Headers/FlashMandel.h"

extern uint32 LinearRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 LogRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 RepeatedRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 SquareRootRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 OneRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 TwoRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 ThreeRemap (const float64, const float64, const float64, const float64, const float64);
extern uint32 FourRemap (const float64, const float64, const float64, const float64, const float64);

uint32 (*COLORREMAP) (const float64, const float64, const float64, const float64, const float64);

static void MCPointMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16);
static void JCPointMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16);

static void JVLineMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16);
static void JHLineMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16);
static void MVLineMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16);
static void MHLineMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16);
static int16 RectangleDrawMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16, const int16);

void (*C_POINT_MEM) (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16);
void (*V_LINE_MEM) (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16);
void (*H_LINE_MEM) (struct MandelChunk *, uint32 *, uint32 *, uint32 *, const int16, const int16, const int16);

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2,
  				gcre3, gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp, gdist, gmaxdist,
  				gincremreal, gincremimag;

#ifdef USE_POWERPC_MATH
extern uint32 MandelnPPC (uint32, int16, float64, float64);
extern uint32 JulianPPC (uint32, int16, float64, float64, float64, float64);
#elif USE_C_MATH
extern uint32 Mandeln (uint32, int16, float64, float64);
extern uint32 Julian (uint32, int16, float64, float64, float64, float64);
#elif USE_ALTIVEC_MATH
extern uint32 MandelnAltivec (uint32 *, uint32, int16, float32, float32, float32, float32, float32, float32, float32, float32);
extern uint32 JulianAltivec (uint32 *, uint32, int16, float32, float32, float32, float32, float32, float32, float32, float32, float32, float32);
#endif /* USE_POWERPC_MATH */

void CalcFractalMem (struct MandelChunk *, uint32 *, uint32 *, uint32 *);

/* We can compute the address of an element of the array by using the rows and colums of the array
   Use the following formula:
   &arr[i][j] = baseaddress + [(i * total_colums + j) * sizeof (datatype)];
   If we want to get the value at any given row, column of the array then we can use the value at
   the address of "*" operator and the following formula:
   arr[i][j] = *(ptr + (i * total_colums + j)); */

/* MCPointMem() */
static void MCPointMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 x, const int16 y)
{
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
#endif /* USE_ALTIVEC_MATH */

  	if (Color)
    {
      	*(RenderMem + (y * MandelInfo->Width + x)) = Color;
      	*(HistogramMem + Color) += 1;
    }
}

static void MVLineMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 b1, const int16 b2, const int16 x)
{
  int16 y;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

#ifdef USE_ALTIVEC_MATH	/* compute 4 pixels per time!! */
  	float32 Cre = (float32) mpf_get_d (gcre);

  	y = b2;

  	while (y >= b1)
    {				/* Cim = MandelInfo->IMax - gincremimag * (y-0); */
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
      	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, (float32) mpf_get_d (gcim), Cre, (float32) mpf_get_d (gcim1), Cre, (float32) mpf_get_d (gcim2), Cre, (float32) mpf_get_d (gcim3));
#elif USE_ALTIVEC_MATH 
      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, (float32) mpf_get_d (gcim), Cre, (float32) mpf_get_d (gcim1), Cre, (float32) mpf_get_d (gcim2), Cre, (float32) mpf_get_d (gcim3));
#endif

      	if (y < b1) break;
      	if (*(PixelVecBase + 0))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 0);
	  		*(HistogramMem + *(PixelVecBase + 0)) += 1;
		}
      	y--;

      	if (y < b1) break;
      	if (*(PixelVecBase + 1))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 1);
	  		*(HistogramMem + *(PixelVecBase + 1)) += 1;
		}
      	y--;

      	if (y < b1)	break;
      	if (*(PixelVecBase + 2))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 2);
	  		*(HistogramMem + *(PixelVecBase + 2)) += 1;
		}
      	y--;

      	if (y < b1)	break;
      	if (*(PixelVecBase + 3))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 3);
	  		*(HistogramMem + *(PixelVecBase + 3)) += 1;
		}
      	y--;
    }
#elif USE_POWERPC_MATH || USE_C_MATH
  uint32 Color;

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);

  	for (y = b2; y >= b1; y--)
    {
#ifdef USE_POWERPC_MATH
      	Color = MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
      	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#endif /* USE_POWERPC_MATH */

      	if (Color)
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = Color;
	  		*(HistogramMem + Color) += 1;
		}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
    }
#endif /* USE_ALTIVEC_MATH */
}

static void MHLineMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 a1, const int16 a2, const int16 y)
{
  int16 x;

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH /* compute 4 pixels per time!! */
  	float32 Cim = (float32) mpf_get_d (gcim);

  	x = a1;

  	while (x <= a2)
    {				/* Cre = MandelInfo->RMin + gincremreal * (x+0); */
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
      	MandelnAltivecPPC (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, (float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim, (float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
#elif USE_ALTIVEC_MATH
      	MandelnAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, (float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim, (float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim);
#endif
      	if (x > a2) 	break;
      	if (*(PixelVecBase + 0))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 0);
	  		*(HistogramMem + *(PixelVecBase + 0)) += 1;
		}
      	x++;

      	if (x > a2)	break;
      	if (*(PixelVecBase + 1))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 1);
	  		*(HistogramMem + *(PixelVecBase + 1)) += 1;
		}
      	x++;

      	if (x > a2) break;
      	if (*(PixelVecBase + 2))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 2);
	  		*(HistogramMem + *(PixelVecBase + 2)) += 1;
		}
      	x++;

      	if (x > a2)	break;
      	if (*(PixelVecBase + 3))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 3);
	  		*(HistogramMem + *(PixelVecBase + 3)) += 1;
		}
      	x++;
    }
#elif USE_POWERPC_MATH || USE_C_MATH
  	uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
    {
#ifdef USE_POWERPC_MATH
      	Color =	MandelnPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));
#elif USE_C_MATH
      	Color = Mandeln (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim));;
#endif /* USE_POWERPC_MATH */

      	if (Color)
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = Color;
	  		*(HistogramMem + Color) += 1;
		}

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }
#endif /* USE_ALTIVEC_MATH */
}

/* JCPointMem() */
static void JCPointMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 x, const int16 y)
{
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

  	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, Cim, Cre, Cim, Cre, Cim, Cre, Cim, (float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));
  	Color = *(PixelVecBase + 0);
#elif USE_POWERPC_MATH
  	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
  	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif

  	if (Color)
    {
      	*(RenderMem + (y * MandelInfo->Width + x)) = Color;
      	*(HistogramMem + Color) += 1;
    }
}

static void JVLineMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 b1, const int16 b2, const int16 x)
{
  int16 y;

  	/* Cre = MandelInfo->RMin + gincremreal * x; */
  	mpf_mul_ui (gtmp, gincremreal, x);
  	mpf_add (gcre, grmin, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  	float32 Cre = (float32) mpf_get_d (gcre);

  	y = b2;

  	while (y >= b1)
    {				/* Cim = MandelInfo->IMax - gincremimag * (y-0); */
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

      	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, Cre, (float32) mpf_get_d (gcim), Cre, (float32) mpf_get_d (gcim1), Cre, (float32) mpf_get_d (gcim2), Cre, (float32) mpf_get_d (gcim3), (float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));

      	if (y < b1)	break;
      	if (*(PixelVecBase + 0))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 0);
	  		*(HistogramMem + *(PixelVecBase + 0)) += 1;
		}
    	y--;

      	if (y < b1) break;
      	if (*(PixelVecBase + 1))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 1);
	  		*(HistogramMem + *(PixelVecBase + 1)) += 1;
		}
      	y--;

      	if (y < b1)	break;
      	if (*(PixelVecBase + 2))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 2);
	  		*(HistogramMem + *(PixelVecBase + 2)) += 1;
		}
      	y--;

      	if (y < b1)	break;
      	if (*(PixelVecBase + 3))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 3);
	  		*(HistogramMem + *(PixelVecBase + 3)) += 1;
		}
      	y--;
    }
#elif USE_POWERPC_MATH || USE_C_MATH
  uint32 Color;

  	/* Cim = MandelInfo->IMax - gincremimag * b2; */
  	mpf_mul_ui (gtmp, gincremimag, b2);
  	mpf_sub (gcim, gimax, gtmp);
	
  	for (y = b2; y >= b1; y--)
    {
#ifdef USE_POWERPC_MATH
      	Color = JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color = Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */

      	if (Color)
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = Color;
	  		*(HistogramMem + Color) += 1;
		}

      	/* Cim += gincremimag; */
      	mpf_add (gcim, gcim, gincremimag);
    }
#endif /* USE_ALTIVEC_MATH */
}

static void JHLineMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 a1, const int16 a2, const int16 y)
{
  int16 x;

  	/* Cim = MandelInfo->IMax - gincremimag * y; */
  	mpf_mul_ui (gtmp, gincremimag, y);
  	mpf_sub (gcim, gimax, gtmp);

#ifdef USE_ALTIVEC_MATH		/* compute 4 pixels per time!! */
  	float32 Cim = (float32) mpf_get_d (gcim);

  	x = a1;

  	while (x <= a2)
    {				/* Cre = MandelInfo->RMin + gincremreal * (x+0); */
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

      	JulianAltivec (PixelVecBase, MandelInfo->Iterations, MandelInfo->Power, (float32) mpf_get_d (gcre), Cim, (float32) mpf_get_d (gcre1), Cim, (float32) mpf_get_d (gcre2), Cim, (float32) mpf_get_d (gcre3), Cim, (float32) mpf_get_d (gjkre), (float32) mpf_get_d (gjkim));

      	if (x > a2) break;
      	if (*(PixelVecBase + 0))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 0);
	  		*(HistogramMem + *(PixelVecBase + 0)) += 1;
		}
      	x++;

      	if (x > a2)	break;
      	if (*(PixelVecBase + 1))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 1);
	  		*(HistogramMem + *(PixelVecBase + 1)) += 1;
		}
      	x++;

      	if (x > a2)	break;
      	if (*(PixelVecBase + 2))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 2);
	  		*(HistogramMem + *(PixelVecBase + 2)) += 1;
		}
      	x++;

      	if (x > a2)	break;
      	if (*(PixelVecBase + 3))
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = *(PixelVecBase + 3);
	  		*(HistogramMem + *(PixelVecBase + 3)) += 1;
		}
      	x++;
    }
#elif USE_POWERPC_MATH || USE_C_MATH
  	uint32 Color;

  	/* Cre = MandelInfo->RMin + gincremreal * a1; */
  	mpf_mul_ui (gtmp, gincremreal, a1);
  	mpf_add (gcre, grmin, gtmp);

  	for (x = a1; x <= a2; x++)
    {
#ifdef USE_POWERPC_MATH
      	Color =	JulianPPC (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#elif USE_C_MATH
      	Color =	Julian (MandelInfo->Iterations, MandelInfo->Power, mpf_get_d (gcre), mpf_get_d (gcim), mpf_get_d (gjkre), mpf_get_d (gjkim));
#endif /* USE_POWERPC_MATH */

      	if (Color)
		{
	  		*(RenderMem + (y * MandelInfo->Width + x)) = Color;
	  		*(HistogramMem + Color) += 1;
		}

      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);
    }
#endif /* USE_ALTIVEC_MATH */
}

/* CheckBoxMem() */
int16 CheckBoxMem (struct MandelChunk *MandelInfo, uint32 * RenderMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  const uint32 Color = *(RenderMem + (b1 * MandelInfo->Width + a1));
  int16 Index;

  	if (Color != *(RenderMem + (b2 * MandelInfo->Width + a2))) return FALSE;
  	if (Color != *(RenderMem + (b1 * MandelInfo->Width + a2))) return FALSE;
  	if (Color != *(RenderMem + (b2 * MandelInfo->Width + a1))) return FALSE;

  	if (Color != *(RenderMem + (((b1 + b2) / 2) * MandelInfo->Width + ((a1 + a2) / 2)))) return FALSE; /* check also center point */

  	for (Index = a1 + 1; Index < a2; Index++)
  	{
      	if (Color != *(RenderMem + (b1 * MandelInfo->Width + Index))) return FALSE;
      	if (Color != *(RenderMem + (b2 * MandelInfo->Width + Index))) return FALSE;
    }

  	for (Index = b1 + 1; Index < b2; Index++)
    {
      	if (Color != *(RenderMem + (Index * MandelInfo->Width + a1))) return FALSE;
      	if (Color != *(RenderMem + (Index * MandelInfo->Width + a2))) return FALSE;
    }
  	
	return TRUE;
}

/* RectangleDrawMem() */
static int16 RectangleDrawMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  int16 helpx, helpy, halfx, halfy;
  uint32 ctmp;

	helpy = b2 - b1; // catch edge case  
    if (helpy < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!	
    helpx = a2 - a1; // catch edge case  
	if (helpx < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!
	
  	/* it'a a waste of time to render recursively very small rectangles so brute force goes on */
	if ((helpx < MINLIMIT2) && (helpy < MINLIMIT2))
    {
      	for (helpy = b1 + 1; helpy < b2; helpy++)
			(*H_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, a1 + 1, a2 - 1, helpy);
	
		return FALSE;
    }

	halfx = (a1 + a2) / 2;
  	halfy = (b1 + b2) / 2;	// center point coords
 
  	(*C_POINT_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, halfx, halfy);	/* set center point */

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

  	(*H_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, a1 + 1, halfx - 1, halfy);
  	(*V_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, b1 + 1, halfy - 1, halfx);
  	
	if (RectangleDrawMem (MandelInfo, PixelVecBase, RenderMem, HistogramMem, a1, b1, halfx, halfy)) return TRUE;

  	(*H_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, halfx + 1, a2 - 1, halfy);
  	
	if (RectangleDrawMem (MandelInfo, PixelVecBase, RenderMem, HistogramMem, halfx, b1, a2, halfy)) return TRUE;

  	(*V_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, halfy + 1, b2 - 1, halfx);
  	
	if (RectangleDrawMem (MandelInfo, PixelVecBase, RenderMem, HistogramMem, a1, halfy, halfx, b2)) return TRUE;
  	if (RectangleDrawMem (MandelInfo, PixelVecBase, RenderMem, HistogramMem, halfx, halfy, a2, b2)) return TRUE;

  	return FALSE;
}

void CalcFractalMem (struct MandelChunk *MandelInfo, uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem)
{
  	if (MandelInfo->Flags & JULIA_BIT)
    {
      	C_POINT_MEM = JCPointMem;
      	H_LINE_MEM = JHLineMem;
      	V_LINE_MEM = JVLineMem;
    }

  	else if (MandelInfo->Flags & MANDEL_BIT)
    {
      	C_POINT_MEM = MCPointMem;
      	H_LINE_MEM = MHLineMem;
      	V_LINE_MEM = MVLineMem;
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
  	(*H_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
  	(*H_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
  	(*V_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
  	(*V_LINE_MEM) (MandelInfo, PixelVecBase, RenderMem, HistogramMem, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);

	// start divide et impera recursively!
  	RectangleDrawMem (MandelInfo, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
}
