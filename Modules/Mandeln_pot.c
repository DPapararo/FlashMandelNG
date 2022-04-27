/*
   Mandelbrot main loop, C function for OS4 version

   Ver 1.1 unrolled loop twice - Dino Papararo 7 Mar 2010
   Ver 1.2 removed includes and adopet c standard types - Dino Papararo 7 Mar 2010
   Ver 2.1 reworked code to look like asm handmade code present in mandelnppc.s - Dino Papararo 20 Jan 2020
   Ver 2.2 Added periodicity check - Dino Papararo 19 Apr 2020
   Ver 2.3 Modified iterations count loop and epsilon check - Dino Papararo 05 May 2020
   Ver 2.4 improved periodicity, implemented simpler and faster method - Dino Papararo 23 Dic 2020
   Ver:2.5 rearranged datatypes - Dino Papararo 5 Jan 2021
   Ver:2.6 small changes - Dino Papararo 9 Jan 2021
   $Ver 2.7 Modified for continous potential algorithm  - Dino Papararo 23 Gen 2021 
*/

#include <stdio.h>
#include <math.h>
#include <exec/types.h>

#define MAXPERIOD 20

uint32
Mandeln (int32 Iterations, int32 Power, float64 * Dist, float64 Cre,
	 float64 Cim)
{
  int32 Exp, PLoop;
  const float64 Maxdist = 4.0;
// const float64 Epsilon = 1e-15; // arbitrary epsilon precision check set to 1e-15 or 1e-5 for float64 or single precision floats
  float64 Zr, Zi, Zr2, Zi2, PZr, PZi;
  float64 Tmp1, Tmp2;

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

      *Dist = Zr2 + Zi2;
      if (*Dist > Maxdist)
	break;

      Zr += Cre;
      Zi += Cim;

      /* periodicity check */
/*      Tmp1 = Zr - PZr;
        Tmp1 = fabs (Tmp1);

        if (Tmp1 < Epsilon)
        {
            Tmp2 = Zi - PZi;
            Tmp2 = fabs (Tmp2);

            if (Tmp2 < Epsilon)return 0L; // return 0L or 254L for debug
        } */

      /* another simpler way to check periodicity */
      if (Zr == PZr)
	{
	  if (Zi == PZi)
	    return 0L;		// return 0L or 254L for debug
	}

      if (PLoop++ >= MAXPERIOD)
	{
	  PZr = Zr;
	  PLoop = 0;
	  PZi = Zi;
	}

    }
  while (--Iterations);

  return Iterations;
}
