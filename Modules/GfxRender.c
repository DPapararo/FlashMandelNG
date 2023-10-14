/* Gfx Render 
** created by Dino Papararo v 1.1 03 Dic 2021
** Now (*COLORREMAP) is extern to to avoid conflicts 09/10*2022 dpapararo
*/

#include <proto/graphics.h>
#include <GMP/gmp.h>

#include "Headers/FlashMandel.h"

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2, gcre3,
  		gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp, gdist, gmaxdist,
  		gincremreal, gincremimag, gpzr, gpzi;

extern uint32 (*COLORREMAP) (const float64, const float64, const float64, const float64, const float64);

/* MCPoint() */
void MCPoint (struct MandelChunk *MandelInfo, struct RastPort *Rp,
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

/* MCPoint24bit() */
void MCPoint24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp,
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
        r = (uint8) lround ((sin(0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
		Color_ARGB |= (r << 16);
		g = (uint8) lround ((sin(0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
		Color_ARGB |= (g << 8);
		b = (uint8) lround ((sin(0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
		Color_ARGB |= b;
	}

	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* MVLine() */
void MVLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, uint32 *PixelVecBase,  
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

/* MVLine24bit() */
void MVLine24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint8 *PixelLine, 
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
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 0) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 0) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 0) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
	   
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;

    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 1) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 1) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 1) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
	
    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 2) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 2) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 2) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;

    	if (y < b1)	break;
		Color_ARGB = 0xff000000;	
	
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 3) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 3) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 3) + 0.10) * 127.5 + 127.5));
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
	    	r = (uint8) lround ((sin(0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}

		/* Cim += gincremimag; */
		mpf_add (gcim, gcim, gincremimag);

		WritePixelColor (Rp, x, y, Color_ARGB);
  	}
#endif
}

/* MHLine() */
void MHLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, 
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

/* MHLine24bit() */
void MHLine24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp,
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
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 0) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 0) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 0) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;

		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 1) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 1) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 1) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;
	
        if (x > a2)	break;
		Color_ARGB = 0xff000000;	

		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 2) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 2) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 2) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;
		
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 3) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 3) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 3) + 0.10) * 127.5 + 127.5));
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
	        r = (uint8) lround ((sin(0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

	  	WritePixelColor (Rp, x, y, Color_ARGB);
   	}
#endif
}

/* JCPoint() */
void JCPoint (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint32 *PixelVecBase, const int16 x, const int16 y)
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

/* JCPoint24bit() */
void JCPoint24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp,
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
        r = (uint8) lround ((sin(0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
		Color_ARGB |= (r << 16);
		g = (uint8) lround ((sin(0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
		Color_ARGB |= (g << 8);
		b = (uint8) lround ((sin(0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
		Color_ARGB |= b;
	}

	WritePixelColor (Rp, x, y, Color_ARGB);
}

/* JVLine() */
void JVLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, uint32 *PixelVecBase, 
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

/* JVLine24bit() */
void JVLine24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint8 *PixelLine, uint32 *PixelVecBase, 
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
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 0) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 0) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 0) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
	   
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
	
    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 1) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 1) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 1) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;
	
    	if (y < b1)	break;
		Color_ARGB = 0xff000000;
	
		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 2) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 2) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 2) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
		y--;

    	if (y < b1)	break;
		Color_ARGB = 0xff000000;	
	
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 3) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 3) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 3) + 0.10) * 127.5 + 127.5));
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
	    	r = (uint8) lround ((sin(0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}

		/* Cim += gincremimag; */
		mpf_add (gcim, gcim, gincremimag);

		WritePixelColor (Rp, x, y, Color_ARGB);
  	}
#endif
}

/* JHLine() */
void JHLine (struct MandelChunk *MandelInfo, struct RastPort *Rp,uint8 *PixelLine, uint32 *PixelVecBase, 
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

/* JHLine24bit() */
void JHLine24bit (struct MandelChunk *MandelInfo, struct RastPort *Rp, uint8 *PixelLine, uint32 *PixelVecBase, 
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
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 0) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 0) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 0) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;

		if (*(PixelVecBase + 1))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 1) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 1) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 1) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;
	
        if (x > a2)	break;
		Color_ARGB = 0xff000000;	

		if (*(PixelVecBase + 2))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 2) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 2) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 2) + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
		}
		    
		WritePixelColor (Rp, x, y, Color_ARGB);
    	x++;

        if (x > a2)	break;
		Color_ARGB = 0xff000000;
		
		if (*(PixelVecBase + 3))
		{
	        r = (uint8) lround ((sin(0.016 * (float64) *(PixelVecBase + 3) + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) *(PixelVecBase + 3) + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) *(PixelVecBase + 3) + 0.10) * 127.5 + 127.5));
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
	        r = (uint8) lround ((sin(0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
			Color_ARGB |= (r << 16);
			g = (uint8) lround ((sin(0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
			Color_ARGB |= (g << 8);
			b = (uint8) lround ((sin(0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
			Color_ARGB |= b;
	  	}
		
      	/* Cre += gincremreal; */
      	mpf_add (gcre, gcre, gincremreal);

      	WritePixelColor (Rp, x, y, Color_ARGB);
   }
#endif
}
