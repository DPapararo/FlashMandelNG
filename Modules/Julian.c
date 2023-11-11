/*
   Julia main loop, C function for OS4 version

   Ver 1.3 Dino Papararo 5 Jun 2010
   Ver 1.4 removed includes and adopet c standard types - Dino Papararo 7 Mar 2010
   Ver 2.1 reworked code to look like asm handmade code present in julianppc.s - Dino Papararo 20 Jan 2020
   Ver 2.2 Added periodicity check - Dino Papararo 19 Apr 2020
   Ver 2.3 Modified iterations count loop and epsilon check - Dino Papararo 05 May 2020
   Ver 2.4 improved periodicity, implemented simpler and faster method - Dino Papararo 23 Dic 2020
   Ver:2.5 rearranged datatypes - Dino Papararo 5 Jan 2021
   $Ver:2.6 small changes - Dino Papararo 9 Jan 2021
*/

#include <exec/types.h>

#define MAXPERIOD 20

uint32 Julian (uint32 Iterations, int16 Power, float64 Cre, float64 Cim, float64 JKre,	float64 JKim)
{
  int16 Exp;
  uint32 PLoop;
  const float64 Maxdist = 4.0f;
// const float64 Epsilon = 1e-15; // arbitrary epsilon precision check set to 1e-15 or 1e-5 for float64 or single precision floats
  float64 Zr, Zi, Zr2, Zi2, PZr, PZi;
  float64 Tmp1, Tmp2;

  	Zr = Cre;
  	PLoop = 0L;
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
