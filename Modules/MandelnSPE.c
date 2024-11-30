/*
  	Ver 1.0 Wrapper function for MandelnSPE_Core - 22 nov 2024 Dino Papararo  
*/

#include <exec/types.h>

uint32 MandelnSPE (uint32 Iterations, int16 Power, float64 Cre, float64 Cim)
{
	return MandelnSPE_Core (Iterations, Power, &Cre, &Cim);
}
