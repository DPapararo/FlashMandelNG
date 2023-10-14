/*
   Julia main loop, C function for OS4 SPE version

   Ver 1.0 Dino Papararo 27 Dec 2022
*/

#include <spe.h>

#define MAXPERIOD 20

unsigned JulianSPE (unsigned Iterations, short Power, float Cre, float Cim, float JKre, float JKim)
{
  short Exp;
  unsigned PLoop;
  const float Maxdist = 4.0f;
// const float64 Epsilon = 1e-15; // arbitrary epsilon precision check set to 1e-15 or 1e-5 for float64 or single precision floats
  float Zr, Zi, Zr2, Zi2, PZr, PZi;
  float Tmp1, Tmp2;

  	Zr = Cre;
  	PLoop = 0;
  	Zi = Cim;

  	do
    {
		for (Exp = Power; Exp > 0; Exp--)
		{
	  		Zi2 = Zi * Zi;
	  		Zr2 = Zr * Zr;
	  		Zi *= Zr;
	  		Zr = Zr2 - Zi2;
	  		Zi += Zi;
		}

      	if ((Zr2 + Zi2) > Maxdist) break;

      	Zr += JKre;
      	Zi += JKim;

		/* periodicity check */
/*      Tmp1 = Zr - PZr;
        Tmp1 = fabs (Tmp1);

        if (Tmp1 < Epsilon)
        {
            Tmp2 = Zi - PZi;
            Tmp2 = fabs (Tmp2);

            if (Tmp2 < Epsilon) return 0L; // return 0L or 254L for debug
        }
*/
      /* another simpler way to check periodicity */
      	if (Zr == PZr)
		{
	  		if (Zi == PZi) return 0L;		// return 0L or 254L for debug
		}

      	if (PLoop++ >= MAXPERIOD)
		{
	  		PZr = Zr;
	  		PLoop = 0L;
	  		PZi = Zi;
		}

    } while (--Iterations);

  	return Iterations;
}
