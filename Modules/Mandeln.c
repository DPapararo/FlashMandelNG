/*
   Mandelbrot main loop, C function for OS4 version

   Ver 1.1 unrolled loop twice - Dino Papararo 7 Mar 2010
   Ver 1.2 removed includes and adopet c standard types - Dino Papararo 7 Mar 2010
   Ver 2.1 reworked code to look like asm handmade code present in mandelnppc.s - Dino Papararo 20 Jan 2020
   Ver 2.2 added periodicity check - Dino Papararo 19 Apr 2020
   Ver 2.3 modified iterations count loop and epsilon check - Dino Papararo 05 May 2020
   Ver 2.4 improved periodicity, implemented simpler and faster method - Dino Papararo 23 Dic 2020
   Ver 2.5 rearranged datatypes - Dino Papararo 5 Jan 2021
   Ver 2.6 small changes - Dino Papararo 9 Jan 2021
   Ver 2.7 rearranged and optimized code - Dino Papararo 8 Nov 2024 
   $Ver:2.8 fixed speed regression - Dino Papararo 14 Dec 2024     
*/

#include <math.h>
#include <exec/types.h>

#define MAXPERIOD 20 // periodicity check interval
#define TOLERANCE 1e-15 // tolerance for precision check - set from 1e-5 to 1e-15 

// Absolute difference between two float values
/*
static inline float64 fabs_diff (float64 a, float64 b) 
{
    return (a > b) ? (a - b) : (b - a);
}
*/

uint32 Mandeln (uint32 Iterations, int16 Power, float64 Cre, float64 Cim)
{
  int16 Exp, PLoop;
  const float64 Maxdist = 4.0f;
  const float64 Epsilon = TOLERANCE; 
  float64 Zr, Zi, Zr2, Zi2x2, ZrZi, PZr, PZi;

	PZr = Cre;
	PZi = Cim;
  	PLoop = MAXPERIOD;
  	Zr = Cre;
  	Zi = Cim;

  	do
    {	Exp = Power;

		do
		{	Zr2 = Zr * Zr;
	  		Zi2x2 = Zi * Zi;
	  		ZrZi = Zr * Zi;
	  		Zr = Zr2 - Zi2x2;
	  		Zi = ZrZi + ZrZi;
        } while (--Exp > 0);

      	if ((Zr2 + Zi2x2) > Maxdist) break;

      	Zr += Cre;
      	Zi += Cim;

//		if ((fabs_diff (Zr, PZr) < Epsilon) && (fabs_diff (Zi, PZi) < Epsilon))
		if ((Zr == PZr) && (Zi == PZi))
		{
			Iterations = 0L; // return 0L or 254L for debug
			break;
		}		

		if (--PLoop <= 0) 
		{
	  		PZr = Zr;
	  		PLoop = MAXPERIOD;
	  		PZi = Zi;				
		}

    } while (--Iterations);

  	return Iterations;
} 
