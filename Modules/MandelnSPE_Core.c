/*
   Mandelbrot main loop, C function for OS4 SPE version

   Ver 1.0 Dino Papararo 22 Non 2024
*/

#include <spe.h>

#define MAXPERIOD 25
#define TOLERANCE 0.00000000000001f // tolerance for precision check - set from 1e-5 to 1e-15 

// Absolute difference between two float values
static inline float fabs_diff (float a, float b) 
{
    return (a > b) ? (a - b) : (b - a);
}

unsigned MandelnSPE_Core (unsigned Iterations, short Power, int *Cre, int *Cim)
{  
  short Exp, PLoop;
  const float Maxdist = 4.0f;
  const float Epsilon = TOLERANCE; 
  float Zr, Zi, Zr2, Zi2x2, ZrZi, PZr, PZi, VCre, VCim;

	VCre = (float) *Cre;
	VCim = (float) *Cim;
	PZr = VCre;
	PZi = VCim;
  	PLoop = MAXPERIOD;	
  	Zr = VCre;
  	Zi = VCim;

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
	
		if (PLoop-- <= 0) 
		{
        	if ((fabs_diff (Zr, PZr) < Epsilon) && (fabs_diff (Zi, PZi) < Epsilon))
			{
				Iterations = 0L; // return 0L or 254L for debug
				break;
			}
        	
	  		PZr = Zr;
	  		PLoop = MAXPERIOD;
	  		PZi = Zi;				
		}

      	if ((Zr2 + Zi2x2) > Maxdist) break;

      	Zr += VCre;
      	Zi += VCim;
    } while (--Iterations);

  	return Iterations;
}    
