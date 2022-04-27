####################################################################################
#  V2.1 handmade asm main routine - Dino Papararo 20 Jan 2020
#  V2.2 rescheduled some instruction - Dino Papararo 22 Jan 2020
#  V2.3 added periodicity checking - Dino Papararo 13 Apr 2020
#  Ver 2.4 small speedup changes - Dino Papararo 03 May 2020
#  $Ver 2.5 modified Periodicity check, another speedup! - Dino Papararo 24 Dic 2020
#
#  FUNCTION
#
#    MandelnPPC -- perform Z = (Z^(2^n)) + C iteration
#
#  SYNOPSIS
#
#    unsigned long MandelnPPC (long Iterations,long Power,double Cre,double Cim)
#
#  This function tests if a point belongs or not at Mandelbrot's set
#  Handmade optimized for PowerPC processors.
#
#  r3:Iterations r4:Power r5:*Dist r6:PLoop r7:MaxPeriod r8:LoadFloat
#  f0:MaxDist f1:Cre f2:Cim f11:Zr f12:Zi f5:Zr2/Tmp1 f6:Zi2/Tmp2
#  f7:Dist f8:Pzr f9:Pzi f10:Epsilon
####################################################################################

    .file "mandelnppc.s"

    .section ".text"
    .align   2
    .globl   MandelnPPC
    .type    MandelnPPC, @function

MandelnPPC:
    fmr     %f12,%f2                 #Zi = Cim
    lis     %r8,.Radius@ha          #set high 16bits r8 as Radius and clear lower 16bits
    fmr     %f11,%f1                 #Zr = Cre
    li      %r7,20                  #MaxPeriod = 20
    lfd     %f0,.Radius@l(%r8)      #load dword from Radius into f0
#    lis     %r8,.Epsilon@ha         #set high 16bits r8 as Epsilon and clear lower 16bits
    fmr     %f8,%f11                 #Pzr = zr
    xor     %r6,%r6,%r6             #PLoop = 0
    fmr     %f9,%f12                 #Pzi = zi
#    lfd     %f10,.Epsilon@l(%r8)    #load Epsilon into f10

.MainLoop:
    mtctr   %r4                     #Load Power into counter

.PowerLoop1:
    fmul    %f6,%f12,%f12             #Zi2 = Zi * Zi
    fmul    %f5,%f11,%f11             #Zr2 = Zr * Zr
    fmul    %f12,%f12,%f11             #Zi *= Zr
    fsub    %f11,%f5,%f6             #Zr = Zr2 - Zi2
    fadd    %f12,%f12,%f12             #Zi += Zi
    bdnz    .PowerLoop1             #if --Power > 0 goto .PowerLoop1

    fadd    %f7,%f5,%f6             #Dist = Zr2 + Zi2
    fcmpu   %cr5,%f7,%f0            #compare dist with Radius
    fadd    %f11,%f11,%f1             #Zr += Cre
    bgt-    %cr5,.Exit              #if dist > radius goto .Exit
    cmpwi   %cr6,%r3,0              #compare Iterations with 0
    fadd    %f12,%f12,%f2             #Zi += Cim
    beq-    %cr6,.Exit              #if Iterations == 0 goto .Exit

#   check periodicity method (1) reusing Zr2 and Zi2 as Temp vars
#   fsub   %f5,%f11,%f8              #Tmp1 = Zr - Pzr
#   fabs   %f5,%f5                  #Tmp1 = abs |Tmp1|
#   fcmpu  %cr7,%f5,%f10            #compare Tmp1 with Epsilon
#   bge+   %cr7,.NoPeriodicity      #if Pzr >= Epsilon goto .NoPeriodicity
#   fsub   %f6,%f12,%f9              #Tmp2 = Zi - Pzi
#   fabs   %f6,%f6                  #Tmp2 = abs |Tmp2|
#   fcmpu  %cr5,%f6,%f10            #compare Tmp2 with Epsilon
#   blt-   %cr5,.PeriodicityExit    #if Tmp2 < Epsilon goto .PeriodicityExit

#   check periodicity method (2)
    fcmpu   %cr7,%f11,%f8            #compare Pzr with zr
    bne+    %cr7,.NoPeriodicity     #if not equal goto .NoPeriodicity
    fcmpu   %cr5,%f12,%f9            #compare Pzi with zi
    beq-    %cr5,.PeriodicityExit   #if equal goto .PeriodicityExit

.NoPeriodicity:
    cmpw    %cr6,%r6,%r7            #compare PLoop with MaxPeriod
    subic.  %r3,%r3,1               #Iterations--
    addi    %r6,%r6,1               #PLoop++
    blt+    %cr6,.MainLoop          #if PLoop < MaxPeriod goto .MainLoop

    fmr     %f8,%f11                 #Pzr = zr
    xor     %r6,%r6,%r6             #PLoop = 0
    fmr     %f9,%f12                 #Pzi = zi
    bgt+    .MainLoop               #if Iterations > 0 goto .MainLoop

.Exit:
    fmr     %r5,%f7
    blr                             #return

.PeriodicityExit:
    xor    %r3,%r3,%r3              #Iterations = 0
#    li     %r3,254                  #for debug
    blr                             #return

    .size   MandelnPPC,.-MandelnPPC
    .section    .rodata
    .align  2
.Radius:
    .long   1074790400              #radius = 4.0
#    .align  2
#.Epsilon:
#    .long   1013386997             #1e-17 very high precision
#    .long   1020396463             #1e-15 high precision
#    .long   1037794527             #1e-10 mid precision
#    .long   1055193269             #1e-5 lower precision

