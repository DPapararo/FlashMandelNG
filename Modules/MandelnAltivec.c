/*
   MandelnAltivec main loop, C function for OS4 version
   $Ver 1.5 Dino Papararo 03 Feb 2019
   all altivec functions are tested with a powermac g5 quad and xcode 3 with asciimandelbrot test program :-)
   Ver 1.6 Dino Papararo 16 Jan 2020 normalized types, small changes to loops
   Ver 2.1 reworked code to look like asm handmade code present in mandelnppc.s - Dino Papararo 20 Jan 2020
   Ver 2.2 added periodicity check - Dino Papararo 19 Apr 2020
   Ver 2.3 small improvements - Dino Papararo 05 May 2020
   Ver 2.4 speeded up altivec until 3x, now are processed 4 pixels per time - Dino Papararo 14 May 2020
   Ver 2.5 using intrinsics for vector assignments - Dino Papararo 15 May 2020
   Ver 2.6 improved periodicity, implemented simpler and faster method - Dino Papararo 23 Dic 2020
   Ver 2.7 rearranged datatypes - Dino Papararo 5 Jan 2021
   Ver 2.8 removed external Pixel vector - Dino Papararo 16 Jan 2021
   Ver 2.9 Rearranged some instructions to gain some cpu cycles - Dino Papararo 02 Dic 2021
   Ver:3.0 Fixed vec_cmpgt and removed periodicity check and int16 controls - Dino Papararo 04 Jun 2023
   Ver:3.1 readded int16 controls to speedup - Dino Papararo 28 jul 2023
   $Ver:3.3 values from/to memory are loaded/stored at star/end od calculatione - Dino Papararo 05 nov 2023   
*/

#pragma altivec_codegen on
#pragma altivec_model on
#pragma altivec_vrsave off

#include <altivec.h>
#include <exec/types.h>

uint32 MandelnAltivec (uint32 *PixelVecBase, uint32 Iterations, int16 Power,
                        float32 Cre, float32 Cim, float32 Cre1, float32 Cim1,
                        float32 Cre2, float32 Cim2, float32 Cre3, float32 Cim3)
{
  register int16 Exp, ll = 0, lh = 0, hl = 0, hh = 0;

  vector bool int VMask;
  const vector bool int VMask_ll = ((vector bool int) { 0xffffffffffffffff, 0L, 0L, 0L });
  const vector bool int VMask_lh = ((vector bool int) { 0L, 0xffffffffffffffff, 0L, 0L });
  const vector bool int VMask_hl = ((vector bool int) { 0L, 0L, 0xffffffffffffffff, 0L });
  const vector bool int VMask_hh = ((vector bool int) { 0L, 0L, 0L, 0xffffffffffffffff });

  vector float VZr, VZi, VZi2, VZr2, VDist;
 
  const vector float VCre  = ((vector float) { Cre, Cre1, Cre2, Cre3 });
  const vector float VCim = ((vector float) { Cim, Cim1, Cim2, Cim3 });
  const vector float VMaxDist = vec_ctf (vec_splat_u32 (4L), 0); /* ((vector float) { 4.0, 4.0, 4.0, 4.0 }); */
  const vector float VFZero  = vec_xor (VFZero, VFZero); 
     
  const vector unsigned int VIOne = vec_splat_u32 (1L); 

  vector unsigned int VIIterations = ((vector unsigned int) { Iterations, Iterations, Iterations, Iterations }); 
  vector unsigned int VIResult = vec_splat_u32 (0L);

    VZr = vec_or (VFZero, VCre); /* faster than load */
    VZi = vec_or (VFZero, VCim);
	    
    do
    {
      	for (Exp = Power; Exp > 0; Exp--)
		{
			VZi2 = vec_madd (VZi, VZi, VFZero);
	  		VZr2 = vec_madd (VZr, VZr, VFZero);
	  		VZi = vec_madd (VZi, VZr, VFZero);
	  		VZr = vec_sub (VZr2, VZi2);
	  		VZi = vec_add (VZi, VZi);
		}

      	VDist = vec_add (VZr2, VZi2);

      	if (vec_all_gt (VDist, VMaxDist))
		{
			VIResult = VIIterations;
	  		break;
		}

        if (vec_any_gt (VDist, VMaxDist)) /* start fine tuning for each quartet of pixel */
        {  
			VMask = vec_cmpgt (VDist, VMaxDist);

			if ((ll) && (hh) && (hl) && (lh)) break;

			if (! ll) { if (vec_all_eq (vec_and (VMask, VMask_ll), VMask_ll)) ll = 1; }
			if (! lh) { if (vec_all_eq (vec_and (VMask, VMask_lh), VMask_lh)) lh = 1; }
			if (! hl) { if (vec_all_eq (vec_and (VMask, VMask_hl), VMask_hl)) hl = 1; }
			if (! hh) { if (vec_all_eq (vec_and (VMask, VMask_hh), VMask_hh)) hh = 1; }

			VIResult = vec_sel (VIResult, VIIterations, VMask);
        }  
		
        VZr = vec_add (VZr, VCre);
        VZi = vec_add (VZi, VCim);
    
		VIIterations = vec_sub (VIIterations, VIOne);
		
	} while (--Iterations);

	vec_st (VIResult, 0, (unsigned int *) PixelVecBase);

    return Iterations;
}
