/*
   CalcMandelnOrbitGMP main loop, GMP function for OS4 version

	$Ver:1.4 Dino Papararo 13 feb 2024
    initialized gzr2 and gzi2

   	Ver:1.3 Dino Papararo 27 Feb 2022
    swapped PixelArray calc order first gzr and later gzi

   	1.2 Dino Papararo 5 Jan 2021
    rearranged datatypes

   	1.1 Dino Papararo 1 May 2020
    modified do-while loop

   	1.0 Dino Papararo 20 Mar 2020
    first release
*/

#include <exec/types.h>

#include <GMP/gmp.h>
// #include <mpfr.h>
// #include <mpf2mpfr.h>

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gdist, gmaxdist, gtmp;

uint32 CalcMandelnOrbit_GMP (int16 * PixelArray, uint32 MaxIterations, int16 Power, int16 SWidth, int16 SHeight, mpf_t GCre, mpf_t GCim)
{
  int16 Exp;
  uint32 Iterations = NULL;

  	mpf_set (gzr, GCre);
  	mpf_set (gzi, GCim);
	mpf_set (gzr2, gzr);
  	mpf_set (gzi2, gzi);	

  	do
    {
      	mpf_mul_ui (gtmp, gzr, SWidth);
      	mpf_div (gtmp, gtmp, gmaxdist);
      	mpf_ui_sub (gtmp, (SWidth / 2), gtmp);
      	*PixelArray++ = (int16) mpf_get_si (gtmp);

     	mpf_mul_ui (gtmp, gzi, SHeight);
      	mpf_div (gtmp, gtmp, gmaxdist);
      	mpf_ui_sub (gtmp, (SHeight / 2), gtmp);
      	*PixelArray++ = (int16) mpf_get_si (gtmp);

      	for (Exp = Power; Exp > 0; Exp--)
		{
	  		mpf_mul (gzi2, gzi, gzi);
	  		mpf_mul (gzr2, gzr, gzr);
	  		mpf_mul (gzi, gzi, gzr);
	  		mpf_sub (gzr, gzr2, gzi2);
	  		mpf_add (gzi, gzi, gzi);
		}

      	mpf_add (gdist, gzr2, gzi2);

      	if (mpf_cmp (gdist, gmaxdist) > 0) break;

      	mpf_add (gzi, gzi, GCim);
      	mpf_add (gzr, gzr, GCre);

    } while (Iterations++ < MaxIterations);

  	return Iterations;
}
