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
   Ver:2.8 removed external Pixel vector - Dino Papararo 16 Jan 2021
   $Ver:2.9 Rearranged some instructions to gain some cpu cycles - Dino Papararo 02 Dic 2021
*/

//#pragma altivec_codegen on
//#pragma altivec_model on
//#pragma altivec_vrsave off

#include <altivec.h>
#include <exec/types.h>

#define VF_ALL(x) ((vector float) {x, x, x, x})
#define MAXPERIOD 20        // periodicity check interval

/* inline vector float vec_neg_zero (void)
{
    vector unsigned result = vec_splat_u32 (-1);
    return (vector float) vec_sl (result,result);
} */

uint32 MandelnAltivec (uint32 *PixelVecBase, uint32 Iterations, int16 Power,
                        float32 Cre, float32 Cim, float32 Cre1, float32 Cim1,
                        float32 Cre2, float32 Cim2, float32 Cre3, float32 Cim3)
{
  int16 Exp, ll, lh, hl, hh;
  uint32 PLoop;
  vector bool int VMask, VPMask1, VPMask2;
  vector float VZr, VZi, VZi2, VZr2, VPZr, VPZi, VDist;

//  const vector float32 VEpsilon = VF_ALL (1e-5); // arbitrary epsilon precision check set to 1e-5 due single precision float32s
//  const vector float VFZero = vec_neg_zero (); // -0.0 notation is preferred to manage IEEE edge cases in fmadd instruction
//  const vector float VFZero = VF_ALL (-0.0f); // -0.0 notation is preferred to manage IEEE edge cases in fmadd instruction
//  const vector float VMaxDist = VF_ALL (4.0f);

  const vector float VCre = (vector float) { Cre, Cre1, Cre2, Cre3 };
  const vector float VFZero = vec_xor (VFZero,VFZero);
  const vector float VCim = (vector float) { Cim, Cim1, Cim2, Cim3 };
  const vector float VMaxDist = vec_ctf (vec_splat_u32 (4),0);
  const vector unsigned int VOne = vec_splat_u32 (1);


    PLoop = 0;
    VZr = vec_or (VFZero,VCre); // faster than load
    ll = 0;
    VZi = vec_or (VFZero,VCim);
    hl = 0;
    VPZr = vec_or (VFZero,VCre);
    lh = 0;
    VPZi = vec_or (VFZero,VCim);
    hh = 0;

    do
    {
        for (Exp = Power; Exp > 0; Exp--)            
        {
//          VZi2 = vec_madd (VZi, VZi, VFZero);
//          VZr2 = vec_madd (VZr, VZr, VFZero);
//          VZi = vec_add (VZr, VZi);
//          VZi = vec_madd (VZi, VZi, VFZero);
//          VZr = vec_sub (VZr2, VZi2);
//          VZi = vec_sub (VZi, VZr2);
//          VZi = vec_sub (vec_sub (VZi, VZr2), VZi2);    

            VZi2 = vec_madd (VZi, VZi, VFZero);
            VZr2 = vec_madd (VZr, VZr, VFZero);
            VZi = vec_madd (VZi, VZr, VFZero);
            VZr = vec_sub (VZr2, VZi2);
            VZi = vec_add (VZi, VZi);
        }

        VDist = vec_add (VZr2, VZi2);

        if (vec_all_gt (VDist, VMaxDist))
        {
            if (!ll) *(PixelVecBase + 0) = Iterations;
            if (!lh) *(PixelVecBase + 1) = Iterations;
            if (!hl) *(PixelVecBase + 2) = Iterations;
            if (!hh) *(PixelVecBase + 3) = Iterations;

            return Iterations;
        }

        if (vec_any_gt (VDist, VMaxDist))
        {
            VMask = vec_cmpgt (VDist, VMaxDist);

            if (!ll)
            {
                if (VMask[0])
                {
                    *(PixelVecBase + 0) = Iterations;
                    ll = 1;
                }
            }
            
            if (!lh)
            {
                if (VMask[1])
                {
                    *(PixelVecBase + 1) = Iterations;
                    lh = 1;
                }
            }
      
            if (!hl)
            {
                if (VMask[2])
                {
                    *(PixelVecBase + 2) = Iterations;
                    hl = 1;
                }
            }
      
            if (!hh)
            {
                if (VMask[3])
                {
                    *(PixelVecBase + 3) = Iterations;
                    hh = 1;
                }
            }

            if (ll)
                {if (hh)
                    {if (hl)
                        {if (lh)
                            return Iterations;}}}
        }

        VZr = vec_add (VZr, VCre);
        VZi = vec_add (VZi, VCim);

        /* periodicity check method 1 - VZr2 and VZi2 can be reused as tmp vars */
/*
        VZr2 = vec_sub (VZr,VPZr);
        VZr2 = vec_abs (VZr2);

        if (vec_all_lt (VZr2,VEpsilon))
        {
            VZi2 = vec_sub (VZi,VPZi);
            VZi2 = vec_abs (VZi2);
            
            if (vec_all_lt (VZi2,VEpsilon))
            {
                if (!ll) PixelVecBase[0] = 254L;
                if (!lh) PixelVecBase[1] = 254L;
                if (!hl) PixelVecBase[2] = 254L;
                if (!hh) PixelVecBase[3] = 254L;

                return Iterations;
            }
        }

        if (vec_any_lt (VZr2,VEpsilon))
        {
            VZi2 = vec_sub (VZi,VPZi);
            VZi2 = vec_abs (VZi2);

            if (vec_any_lt (VZi2,VEpsilon))
            {
                VPMask1 = vec_cmplt (VZr2,VEpsilon);
                VPMask2 = vec_cmplt (VZi2,VEpsilon);
                VMask = vec_and (VPMask1,VPMask2);

                if (!ll) {if (VMask[0]) {PixelVecBase[0] = 254L; ll = 1;}}
                if (!lh) {if (VMask[1]) {PixelVecBase[1] = 254L; lh = 1;}}
                if (!hl) {if (VMask[2]) {PixelVecBase[2] = 254L; hl = 1;}}
                if (!hh) {if (VMask[3]) {PixelVecBase[3] = 254L; hh = 1;}}

                if (ll) if (hh) if (hl) if (lh) return 0L;
            }
        }
*/
/* periodicity simpler method 2 */

        if (vec_all_eq (VZr, VPZr))
        {
            if (vec_all_eq (VZi, VPZi))
            {
                if (!ll) *(PixelVecBase + 0) = 0L;
                if (!lh) *(PixelVecBase + 1) = 0L;
                if (!hl) *(PixelVecBase + 2) = 0L;
                if (!hh) *(PixelVecBase + 3) = 0L;

                return Iterations;
            }
        }

        if (vec_any_eq (VZr, VPZr))
        {
            if (vec_any_eq (VZi, VPZi))
            {
                VPMask1 = vec_cmpeq (VZr, VPZr);
                VPMask2 = vec_cmpeq (VZi, VPZi);
                VMask = vec_and (VPMask1, VPMask2);

                if (!ll)
                {
                    if (VMask[0])
                    {
                        *(PixelVecBase + 0) = 0L;
                        ll = 1;
                    }
                }
            
                if (!lh)
                {
                    if (VMask[1])
                    {
                        *(PixelVecBase + 1) = 0L;
                        lh = 1;
                    }
                }
        
                if (!hl)
                {
                    if (VMask[2])
                    {
                        *(PixelVecBase + 2) = 0L;
                        hl = 1;
                    }
                }
        
                if (!hh)
                {
                    if (VMask[3])
                    {
                        *(PixelVecBase + 3) = 0L;
                        hh = 1;
                    }
                }

                if (ll)
                    {if (hh)
                        {if (hl)
                            {if (lh)
                                return 0L;}}}
            }
        }

        if (PLoop++ >= MAXPERIOD)
        {
            VPZr = vec_or (VFZero, VZr);
            PLoop = 0;
            VPZi = vec_or (VFZero, VZi);
        }
    } while (--Iterations);

    if (!ll) *(PixelVecBase + 0) = 0L;
    if (!lh) *(PixelVecBase + 1) = 0L;
    if (!hl) *(PixelVecBase + 2) = 0L;
    if (!hh) *(PixelVecBase + 3) = 0L;

    return 0L;
}
