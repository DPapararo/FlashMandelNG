/*
   Mandelbrot Fixed point main loop, C function for OS4 version

   $Ver 1.0 Dino Papararo 15 Nov 2024
*/

#include <math.h>
#include <exec/types.h>

#define MAXPERIOD 25              // Periodicity check interval
#define TOLERANCE (1)             // Tolerance in 4.28 fixed-point format (~0.00000000372529)
#define MAXDIST (4 << 28)         // 4.0 in 4.28 fixed-point format

// Convert float64 constant to 4.28 fixed-point
static inline int64 Flt2Fix (float64 x) 
{
    int64 conversion = (int64) (x * (1 << 28));
    return conversion;
}

// Multiply two 4.28 fixed-point values without using 64-bit integers
static inline int64 fixed_mul (int32 a, int32 b) 
{
    int64 product = (int64) a * (int64) b;  
    return (product >> 28);
}

// Absolute difference between two 2.30 fixed-point values
static inline int64 fixed_abs_diff (int64 a, int64 b) 
{
    return (a > b) ? (a - b) : (b - a);
}

uint32 MandelnFixed (uint32 Iterations, int16 Power, float64 Cre, float64 Cim) 
{
    int16 Exp, PLoop;
    int64 Zr, Zi, Zr2, Zi2x2, ZrZi, PZr, PZi;

    // Convert Cre and Cim from float64 to 4.28 fixed-point
    int64 Cre_fixed = Flt2Fix (Cre);
    int64 Cim_fixed = Flt2Fix (Cim);

    // Initialize variables
    PZr = Cre_fixed;
    PZi = Cim_fixed;
    PLoop = MAXPERIOD;
    Zr = Cre_fixed;
    Zi = Cim_fixed;

    do 
    {
        for (Exp = Power; Exp > 0; Exp--) 
        {
            Zr2 = fixed_mul (Zr, Zr);      // Zr^2 in fixed-point
            Zi2x2 = fixed_mul (Zi, Zi);   // Zi^2 in fixed-point
            ZrZi = fixed_mul (Zr, Zi);    // Zr * Zi in fixed-point
            Zr = Zr2 - Zi2x2;            // Zr^2 - Zi^2
            Zi = ZrZi + ZrZi;            // 2 * Zr * Zi
        }

        // Check for escape condition
        if ((Zr2 + Zi2x2) > MAXDIST) break;

        // Periodicity check
        if (PLoop-- <= 0) 
        {
            if ((fixed_abs_diff (Zr, PZr) < TOLERANCE) && (fixed_abs_diff (Zi, PZi) < TOLERANCE))
            {
                Iterations = 0L; // return 0L or 254L for debug
                break;                
            }

            PZr = Zr;
            PLoop = MAXPERIOD;
            PZi = Zi;
        }

        // Add the constant Cre and Cim
        Zr += Cre_fixed;
        Zi += Cim_fixed;
        
    } while (--Iterations);

    return Iterations;
}

