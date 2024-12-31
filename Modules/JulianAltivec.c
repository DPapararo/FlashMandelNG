/*
   JulianAltivec main loop, C function for OS4 version

   Ver 1.5 all altivec functions are tested with a powermac g5 quad and xcode 3 with asciimandelbrot test program :-) - Dino Papararo 03 Feb 2019
   Ver 1.6 normalized types, small changes to loops - Dino Papararo 16 Jan 2020
   Ver 2.1 reworked code to look like asm handmade code present in julianppc.s - Dino Papararo 20 Jan 2020
   Ver 2.2 added periodicity check   - Dino Papararo 19 Apr 2020
   Ver 2.3 small improvements - Dino Papararo 05 May 2020
   Ver 2.4 speeded up altivec until 3x, now are processed 4 pixels per time - Dino Papararo 14 May 2020
   Ver 2.5 using intrinsics for vector assignments - Dino Papararo 14 May 2020
   Ver 2.6 improved periodicity, implemented simpler and faster method - Dino Papararo 23 Dic 2020
   Ver 2.7 rearranged datatypes - Dino Papararo 5 Jan 2021
   Ver:2.8 removed external Pixel vector - Dino Papararo 16 Jan 2021  
   Ver:2.9 Rearranged some instructions to gain some cpu cycles - Dino Papararo 02 Dic 2021
   Ver:3.0 Fixed vec_cmpgt and removed periodicity check and int16 controls - Dino Papararo 04 Jun 2023
   Ver:3.1 readded int16 controls to speedup - Dino Papararo 28 jul 2023
   Ver:3.3 values from/to memory are loaded/stored at star/end od calculatione - Dino Papararo 05 nov 2023   
   Ver:3.4 initialized VZr2 and VZi2 to zero - Dino Papararo 11 Feb 2024
   Ver:3.5 reimplemented old code, faster and more precise - Dino Papararo 25 mar 2024
   Ver:3.6 Fixed regression, no more scalar now use highly optimized altivec code with periodicity check - Dino Papararo 08 nov 2024
   $Ver:3.7 fixed regression speed - Dino Papararo 15 dec 2024
*/

#pragma altivec_codegen on
#pragma altivec_model on
#pragma altivec_vrsave off

#include <altivec.h>
#include <exec/types.h>

#define MAXPERIOD 20 // intervallo di controllo di periodicità 
#define TOLERANCE 0.00001f // tolerance for precision check - set from 1e-5 to 1e-15 
// 0.0000610352 is lowest decimal numder with float16 altivec

uint32 JulianAltivec (uint32 * PixelVecBase, uint32 Iterations, int16 Power,
	       				float32 Cre, float32 Cim, float32 Cre1, float32 Cim1,
	       				float32 Cre2, float32 Cim2, float32 Cre3, float32 Cim3,
	       				float32 JKre, float32 JKim)
{
    int16 Exp, PLoop;
    
    vector bool int VMask, VPeriodicityMask;
    vector bool int VExitMask = (vector bool int) vec_splat_u32 (0L); // Vettore per memorizzare i pixel che hanno superato il limite    
    const vector bool int VNegOne = (vector bool int) vec_splat_u32 (-1L);

   	vector unsigned int VIIterations = (vector unsigned int) {Iterations, Iterations, Iterations, Iterations};
    vector unsigned int VIResult = vec_splat_u32 (0L);
	const vector unsigned int VIZero = vec_splat_u32 (0L);
	const vector unsigned int VIOne = vec_splat_u32 (1L);

    vector float VZr, VZi, VZr2, VZi2x2, VZrZi, VDist, VPZr, VPZi;
	//vector float VDiffZr, VDiffZi;
    const vector float VCre = (vector float) {Cre, Cre1, Cre2, Cre3};
    const vector float VCim = (vector float) {Cim, Cim1, Cim2, Cim3};
    const vector float VJKre = (vector float) {JKre, JKre, JKre, JKre};
    const vector float VJKim = (vector float) {JKim, JKim, JKim, JKim};	
    const vector float VMaxDist = (vector float) {4.0f, 4.0f, 4.0f, 4.0f};
    const vector float VFZero = (vector float) vec_splat_u32 (0L);
    //const vector float VTolerance = (vector float) vec_splats (TOLERANCE);

   	VZr = VCre;
    VZi = VCim;
	PLoop = MAXPERIOD;
	VPZr = VCre;
	VPZi = VCim;

    do 
	{	Exp = Power; // inizializza il grado del frattale

     	do
		{	VZr2 = vec_madd (VZr, VZr, VFZero); // Zr^2
			VZi2x2 = vec_madd (VZi, VZi, VFZero);// Zi^2			
	  		VZrZi = vec_madd (VZi, VZr, VFZero); // Zi * Zr
	  		VZr = vec_sub (VZr2, VZi2x2); // Zr = Zr^2 - Zi^2
	  		VZi = vec_add (VZrZi, VZrZi); // Zi = 2 * Zi * Zr
 		} while (--Exp > 0);

		// Calcola la distanza Zr^2 + Zi^2
        VDist = vec_add (VZr2, VZi2x2);	
		// Verifica se ogni pixel supera il limite di distanza
        VMask = vec_cmpgt (VDist, VMaxDist); 
		// Aggiorna il risultato solo per i pixel che superano il limite
        VIResult = vec_sel (VIResult, VIIterations, VMask);
		// Mantiene traccia di quali pixel sono usciti
        VExitMask = vec_or (VExitMask, VMask);
		// Se tutti i pixel hanno superato il limite, usciamo dal ciclo
        if (vec_all_eq (VExitMask, VNegOne)) break;	
		
		// Incremento di Z e decremento delle iterazioni 
        VZr = vec_add (VZr, VJKre);
        VZi = vec_add (VZi, VJKim);
        			
		// Controllo di periodicità
		// VDiffZr = vec_abs (vec_sub (VZr, VPZr));
		// VDiffZi = vec_abs (vec_sub (VZi, VPZi));
        // VPeriodicityMask = vec_and (vec_cmplt (VDiffZr, VTolerance), vec_cmplt (VDiffZi, VTolerance));
		VPeriodicityMask = vec_and (vec_cmpeq (VZr, VPZr), vec_cmpeq (VZi, VPZi));
		
        if (vec_all_eq (VPeriodicityMask, VNegOne)) break;

       	if (PLoop-- <= 0) 
		{
			// Aggiorna i valori precedenti di Zr e Zi ed azzera il loop
            VPZr = VZr;  
            PLoop = MAXPERIOD;
            VPZi = VZi;			
		}

		VIIterations = vec_sub (VIIterations, VIOne);
 	// Cicla fino a quando ci sono iterazioni rimanenti    
	} while (! vec_all_eq (VIIterations, VIZero)); 
    //} while (--Iterations);
	// Scrive il risultato per ogni pixel nell'array di destinazione 
    vec_st (VIResult, 0, (unsigned int *) PixelVecBase); 

    return vec_extract (VIIterations, 0);
    //return Iterations;
}
