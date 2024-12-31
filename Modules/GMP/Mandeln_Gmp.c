/*
   MandelnGMP main loop, GMP function for OS4 version

	$Ver:1.7 Dino Papararo 09 nov 2024
    removed period check

	Ver:1.6 Dino Papararo 13 feb 2024
    initialized gzr2 and gzi2

   	Ver:1.5 Dino Papararo 9 Jan 2021
    small changes

   	Ver:1.4 Dino Papararo 5 Jan 2021
    rearranged datatypes

   	1.3 Dino Papararo 23 Dic 2020
    changed periodicity method

   	1.2 Dino Papararo 01 May 2020
    rearranged periodicity check for minor speedup

   	1.1 Dino Papararo 13 Apr 2020
    added cardiod, period 2 and periodicity check

   	1.0 Dino Papararo 20 Mar 2020
    first release
*/
  
#include <exec/types.h>
#include <GMP/gmp.h>

extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gdist, gmaxdist, gpzr, gpzi;

uint32 Mandeln_GMP (uint32 Iterations, int16 Power)
{
  int16 Exp;
  
  	mpf_set (gzr, gcre);
  	mpf_set (gzi, gcim);
  	mpf_set (gpzr, gzr);
  	mpf_set (gpzi, gzi);
	mpf_set (gzr2, gzr);
  	mpf_set (gzi2, gzi);

  	do
    {
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

		mpf_add (gzr, gzr, gcre);
      	mpf_add (gzi, gzi, gcim);

    } while (--Iterations);

  	return Iterations;
}
