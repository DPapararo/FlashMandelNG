/*
	Ver 1.0 Wrapper function for MandelnSPE_Core - 22 nov 2024 Dino Papararo
*/

#include <exec/types.h>

uint32 JulianSPE (uint32 Iterations, int16 Power, float64 Cre, float64 Cim, float64 JKre, float64 JKim)
{
	return JulianSPE_Core (Iterations, Power, &Cre, &Cim, &JKre, &JKim);
}

