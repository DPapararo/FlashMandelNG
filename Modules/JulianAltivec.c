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
   $Ver:3.1 readded int16 controls to speedup - Dino Papararo 28 jul 2023
*/

#pragma altivec_codegen on
#pragma altivec_model on
#pragma altivec_vrsave off

#include <altivec.h>
#include <exec/types.h>

uint32 JulianAltivec (uint32 * PixelVecBase, uint32 Iterations, int16 Power,
	       				float32 Cre, float32 Cim, float32 Cre1, float32 Cim1,
	       				float32 Cre2, float32 Cim2, float32 Cre3, float32 Cim3,
	       				float32 JKre, float32 JKim)
{
  int16 Exp, ll, lh, hl, hh;
  
  vector bool int VMask;
  vector float VZr, VZi, VZi2, VZr2, VDist;

//  vector unsigned int VecTmp;
//  vector unsigned int VecIter = ((vector unsigned int) { Iterations, Iterations, Iterations, Iterations });
//  const vector unsigned int VIZero = { 0L, 0L, 0L, 0L }; //vec_splat_u32 (0L);
//  const vector unsigned int VIOne = { 1L, 1L, 1L, 1L }; //vec_splat_u32 (1L);
  const vector float VJKre = ((vector float) { JKre, JKre, JKre, JKre });
  const vector float VCre = ((vector float) { Cre, Cre1, Cre2, Cre3 });
  const vector float VFZero = vec_xor (VFZero,VFZero);
  const vector float VJKim = ((vector float) { JKim, JKim, JKim, JKim });
  const vector float VCim = ((vector float) { Cim, Cim1, Cim2, Cim3 });
  const vector float VMaxDist = vec_ctf (vec_splat_u32 (4L), 0); // ((vector float) { 4.0, 4.0, 4.0, 4.0 });

    *(PixelVecBase + 0) = 0L;
	ll = 0;
    *(PixelVecBase + 1) = 0L;
	lh = 0;
    *(PixelVecBase + 2) = 0L;
	hl = 0;
    *(PixelVecBase + 3) = 0L;
	hh = 0;
	
    VZr = vec_or (VFZero, VCre); // faster than load
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
            *(PixelVecBase + 0) = Iterations;
            *(PixelVecBase + 1) = Iterations;
            *(PixelVecBase + 2) = Iterations;
            *(PixelVecBase + 3) = Iterations;
			
	  		break;
		}

        if (vec_any_gt (VDist, VMaxDist))
        {
			VMask = vec_cmpgt (VDist, VMaxDist);
                          
		   	if (!ll) if (VMask [0]) *(PixelVecBase + 0) = Iterations, ll = 1;
           	if (!lh) if (VMask [1]) *(PixelVecBase + 1) = Iterations, lh = 1;
           	if (!hl) if (VMask [2]) *(PixelVecBase + 2) = Iterations, hl = 1;
           	if (!hh) if (VMask [3]) *(PixelVecBase + 3) = Iterations, hh = 1;
			
//			if ((VMask [0]) && (VMask [1]) && (VMask [2]) && (VMask [3])) break;
			if ((ll) && (hh) && (hl) && (lh)) break;
         }
		 
      	VZr = vec_add (VZr, VJKre);
      	VZi = vec_add (VZi, VJKim);

    } while (--Iterations);
 
  	return Iterations;
}
