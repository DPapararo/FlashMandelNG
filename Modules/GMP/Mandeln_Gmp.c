/*
   MandelnGMP main loop, GMP function for OS4 version

   $Ver:1.5 Dino Papararo 9 Jan 2021
    small changes

   1.4 Dino Papararo 5 Jan 2021
    rearranged datatypes

   1.3 Dino Papararo 23 Dic 2020
    changed periodicity method

   1.2 Dino Papararo 01 May 2020
    rearranged periodicity check for minor speedup

   1.1 Dino Papararo 13 Apr 2020
    added periodicity check (cardiod, period 2 check not implemented)

   1.0 Dino Papararo 20 Mar 2020
    first release
*/

#include <exec/types.h>
#include <GMP/gmp.h>
// #include <mpfr.h>
// #include <mpf2mpfr.h>

#define MAXPERIOD 20		// periodicity check interval

// extern mpf_t gtmp;
extern mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gdist, gmaxdist, gpzr, gpzi;

// void CheckBulb (void);

uint32 Mandeln_GMP (uint32 Iterations, int16 Power)
{
  int16 Exp;
  uint32 PLoop;

  	PLoop = 0;
	/*  CheckBulb (); */

  	/* start mandelbrot calculation with periodicity check */
	// mpf_set_d (gtmp,1e-17); // arbitrary epsilon precision check set to 1e-17
  	mpf_set (gzr, gcre);
  	mpf_set (gzi, gcim);
  	mpf_set (gpzr, gzr);
  	mpf_set (gpzi, gzi);

  	do
    {
      	for (Exp = Power; Exp > 0; Exp--)
		{
			/* 
			mpf_pow_ui (gzi2,gzi,2); // should be faster with higher precison bits due square vs mul
            mpf_pow_ui (gzr2,gzr,2);
            mpf_add (gzi,gzr,gzi);
            mpf_pow_ui (gzi,gzi,2);
            mpf_sub (gzr,gzr2,gzi2);
            mpf_sub (gzi,gzi,gzr2);
            mpf_sub (gzi,gzi,gzi2); */

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

      	/* periodicity check method1 - gzr2 and gzi2 can be reused as tmp vars */
		/* mpf_sub (gzr2,gzr,gpzr);
        mpf_abs (gzr2,gzr2);

        if (mpf_cmp (gzr2,gtmp) < 0)
        {
            mpf_sub (gzi2,gzi,gpzi);
            mpf_abs (gzi2,gzi2);

            if (mpf_cmp (gzi2,gtmp) < 0) return 0L; // return 0L or 254L for debug
        } */      	
		
		/* another simpler way to check periodicity */
      	if (mpf_cmp (gzi, gpzi) == 0)
		{
	  		if (mpf_cmp (gzr, gpzr) == 0) return 0L; // return 0L or 254L for debug
		}

      	if (PLoop++ >= MAXPERIOD)
		{
	  		mpf_set (gpzr, gzr);
	  		PLoop = 0;
	  		mpf_set (gpzi, gzi);
		}

    } while (--Iterations);

  	return Iterations;
}

/* void CheckBulb (void)
{
	// valid only for z = z^2 + c
	// start cardiod and period 2 check - recycling some vars..
    mpf_mul (gzi2,gcim,gcim);

	// cardioid check
    mpf_set_d (gtmp,0.25);
    mpf_sub (gtmp,gcre,gtmp);
    mpf_mul (gpzr,gtmp,gtmp);
    mpf_add (gpzr,gpzr,gzi2);
    mpf_add (gpzi,gpzr,gtmp);
    mpf_mul (gpzi,gpzr,gpzi);
    mpf_set_d (gpzr,0.25);
    mpf_mul (gpzr,gpzr,gzi2);
    if (mpf_cmp (gpzi,gpzr) < 0) return 0;

	// period 2 bulb check
    mpf_set_d (gpzr,1.0);
    mpf_add (gpzr,gpzr,gcre);
    mpf_mul (gpzr,gpzr,gpzr);
    mpf_add (gpzr,gpzr,gzi2);
    if (mpf_cmp_d (gpzr,0.0625) < 0) return 0;
} */
