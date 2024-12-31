/*
  Mandelbrot main loop, C function for OS4 SPE version

  Ver 1.0 First version - Dino Papararo 22 Nov 2024
  $Ver:1,1 Fixed speed regression - Dino Papararo 15 Dec 2024
*/

#include <spe.h>

#define MAXPERIOD 20
#define TOLERANCE 1e-15 // tolerance for precision check - set from 1e-5 to 1e-15 

// Absolute difference between two float values
/*
static inline float fabs_diff (float a, float b) 
{
    return (a > b) ? (a - b) : (b - a);
}
*/

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
    {	Exp = Power;

		do
		{	Zr2 = Zr * Zr;
	  		Zi2x2 = Zi * Zi;
	  		ZrZi = Zr * Zi;
	  		Zr = Zr2 - Zi2x2;
	  		Zi = ZrZi + ZrZi;
        } while (--Exp > 0);

      	if ((Zr2 + Zi2x2) > Maxdist) break;

      	Zr += VCre;
      	Zi += VCim;
			
//      if ((fabs_diff (Zr, PZr) < Epsilon) && (fabs_diff (Zi, PZi) < Epsilon))
		if ((Zr == PZr) && (Zi == PZi))
		{
			Iterations = 0L; // return 0L or 254L for debug
			break;
		}

		if (PLoop-- <= 0) 
		{        	
	  		PZr = Zr;
	  		PLoop = MAXPERIOD;
	  		PZi = Zi;				
		}

    } while (--Iterations);

  	return Iterations;
}    
