/*
 *  CalcMandelOrbit.c
 
 Ver 2.5 small changes - Dino Papararo 24 jan 2021
 Ver 2.4 rearranged datatypes - Dino Papararo 05 jan 2021
 Ver 2.3 modified do-while loop - Dino Papararo 01 may 2020
 Ver 2.2 modified some int32 types to int16 - Dino Papararo 25 mar 2020
 Ver 2.1 reworked code to reflect asm handmade code mandelppc.s - Dino Papararo 20 Jan 2020 
 Ver 2.6 swapped PixelArray calc order first zr and later zi - Dino Papararo 27 Feb 2022
 $Ver 2.7 Initialized to zero Zr2 and Zi2 - Dino Papararo 11 feb 2024
*/

#include <exec/types.h>

uint32 CalcMandelnOrbit (int16 *PixelArray, uint32 MaxIterations, int16 Power,
		  					int16 SWidth, int16 SHeight, float64 Cre, float64 Cim)
{
  int16 Exp;
  uint32 Iterations = 0L;
  float64 zr, zi, zr2, zi2;
  const float64 maxdist = 4.0f, zero = 0.0f;

  	zr = Cre;
  	zi = Cim;
	zr2 = zero;
	zi2 = zero;

  	do 
	{
		*PixelArray++ =	(int16) (((float64) SWidth / 2.0) +	((zr * (float64) SWidth) / maxdist));
      	*PixelArray++ = (int16) (((float64) SHeight / 2.0) - ((zi * (float64) SHeight) / maxdist));
      	
      	for (Exp = Power; Exp > 0; Exp--)
		{
	  		zi2 = zi * zi;
	  		zr2 = zr * zr;
	  		zi *= zr;
	  		zr = zr2 - zi2;
	  		zi += zi;
		}

      	if ((zr2 + zi2) > maxdist) break;

      	zi += Cim;
      	zr += Cre;
    } 
	while (Iterations++ < MaxIterations);

  	return Iterations;
}
