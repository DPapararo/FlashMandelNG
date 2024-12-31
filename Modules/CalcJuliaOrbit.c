/*  
 * CalcJuliaOrbit.c

 Ver 2.5 small changes - Dino Papararo 24 jan 2021
 Ver 2.4 rearranged datatypes - Dino Papararo 05 jan 2021    
 Ver 2.3 modified do-while loop - Dino Papararo 01 may 2020
 Ver 2.2 modified some int32 types to int16 - Dino Papararo 25 mar 2020
 Ver 2.1 reworked code to reflect asm handmade code julianppc.s - Dino Papararo 20 Jan 2020
 Ver 2.6 swapped PixelArray calc order first zr and later zi - Dino Papararo 27 Feb 2022
 Ver 2.7 Initialized to zero Zr2 and Zi2 - Dino Papararo 11 feb 2024
 $Ver 2.8 Refactored algorithm - Dino Papararo 14 Dec 2024
*/

#include <exec/types.h>

uint32 CalcJulianOrbit (int16 *PixelArray, uint32 MaxIterations, int16 Power,
		 int16 SWidth, int16 SHeight, float64 Cre, float64 Cim, float64 JKre, float64 JKim)
{
  int16 Exp;
  uint32 Iterations = 0L;
  float64 Zr, Zi, Zr2, Zi2x2, ZrZi;
  const float64 Maxdist = 4.0f;

  	Zr = Cre;
  	Zi = Cim;

  	do
    {	Exp = Power;
      	*PixelArray++ = (int16) (((float64) SWidth / 2.0) + ((Zr * (float64) SWidth) / Maxdist));
    	*PixelArray++ = (int16) (((float64) SHeight / 2.0) - ((Zi * (float64) SHeight) / Maxdist));

		do
		{	Zr2 = Zr * Zr;
	  		Zi2x2 = Zi * Zi;
	  		ZrZi = Zr * Zi;
	  		Zr = Zr2 - Zi2x2;
	  		Zi = ZrZi + ZrZi;
        } while (--Exp > 0);

      	if ((Zr2 + Zi2x2) > Maxdist) break;

      	Zi += JKim;
      	Zr += JKre;
		
    } while (Iterations++ < MaxIterations);

  	return Iterations;
}
