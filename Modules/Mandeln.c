/*
   Mandelbrot main loop, C function for OS4 version

   Ver 1.1 unrolled loop twice - Dino Papararo 7 Mar 2010
   Ver 1.2 removed includes and adopet c standard types - Dino Papararo 7 Mar 2010
   Ver 2.1 reworked code to look like asm handmade code present in mandelnppc.s - Dino Papararo 20 Jan 2020
   Ver 2.2 Added periodicity check - Dino Papararo 19 Apr 2020
   Ver 2.3 Modified iterations count loop and epsilon check - Dino Papararo 05 May 2020
   Ver 2.4 improved periodicity, implemented simpler and faster method - Dino Papararo 23 Dic 2020
   Ver 2.5 rearranged datatypes - Dino Papararo 5 Jan 2021
   Ver 2.6 small changes - Dino Papararo 9 Jan 2021
   $Ver 2.7 rearranged and optimized code - Dino Papararo 8 Nov 2024   
*/

#include <math.h>
#include <exec/types.h>

#define MAXPERIOD 25 // periodicity check interval
#define TOLERANCE 0.000000000000001f // tolerance for precision check - set from 1e-5 to 1e-15 

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
    {
		for (Exp = Power; Exp > 0; Exp--)
		{
	  		Zr2 = Zr * Zr;
	  		Zi2x2 = Zi * Zi;
	  		ZrZi = Zr * Zi;
	  		Zr = Zr2 - Zi2x2;
	  		Zi = ZrZi + ZrZi;
		}

      	if ((Zr2 + Zi2x2) > Maxdist) break;
	
		if (PLoop-- <= 0) 
		{
       		if (fabs (Zr - PZr) < Epsilon)
        	{
				if (fabs (Zi - PZi) < Epsilon) 
				{
					Iterations = 0L; // return 0L or 254L for debug
					break;
				}        
			}	
			
	  		PZr = Zr;
	  		PLoop = MAXPERIOD;
	  		PZi = Zi;				
		}

      	Zr += Cre;
      	Zi += Cim;
		
    } while (--Iterations);

  	return Iterations;
} 
