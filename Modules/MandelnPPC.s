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
#  r3:Iterations r4:Power r5:PLoop r6:MaxPeriod r7:LoadFloat
#  f0:MaxDist f1:Cre f2:Cim f3:Zr f4:Zi f5:Zr2/Tmp1 f6:Zi2/Tmp2
#  f7:Dist f8:Pzr f9:Pzi f10:Epsilon
####################################################################################

    .file "mandelnppc.s"

    .section ".text"
    .align   2
    .globl   MandelnPPC
    .type    MandelnPPC, @function

MandelnPPC:
    fmr     %f4,%f2                 #Zi = Cim
    lis     %r7,.Radius@ha          #set high 16bits r7 as Radius and clear lower 16bits
    fmr     %f3,%f1                 #Zr = Cre
    li      %r6,20                  #MaxPeriod = 20
    lfs     %f0,.Radius@l(%r7)      #load Radius in f0
    fmr     %f8,%f3                 #Pzr = zr
#    lis     %r7,.Epsilon@ha        #set high 16bits r7 as Epsilon and clear lower 16bits
    xor     %r5,%r5,%r5             #PLoop = 0
    fmr     %f9,%f4                 #Pzi = zi
#    lfd     %f10,.Epsilon@l(%r7)    #load Epsilon into f10
 
.MainLoop:
    mtctr   %r4                     #Load Power into counter
.PowerLoop1:
    fmul    %f6,%f4,%f4             #Zi2 = Zi * Zi
    fmul    %f5,%f3,%f3             #Zr2 = Zr * Zr
    fmul    %f4,%f4,%f3             #Zi *= Zr
    fsub    %f3,%f5,%f6             #Zr = Zr2 - Zi2
    fadd    %f4,%f4,%f4             #Zi += Zi
    bdnz    .PowerLoop1             #if --Power > 0 goto .PowerLoop1
  
    fadd    %f7,%f5,%f6             #Dist = Zr2 + Zi2
    fcmpu   %cr5,%f7,%f0            #compare dist with Radius
    fadd    %f3,%f3,%f1             #Zr += Cre
    bgt-    %cr5,.Exit              #if dist > radius goto .Exit
    cmpwi   %cr6,%r3,0              #compare Iterations with 0
    fadd    %f4,%f4,%f2             #Zi += Cim
    beq-    %cr6,.Exit              #if Iterations == 0 goto .Exit

#   check periodicity method (1) reusing Zr2 and Zi2 as Temp vars
#   fsub   %f5,%f3,%f8              #Tmp1 = Zr - Pzr
#   fabs   %f5,%f5                  #Tmp1 = abs |Tmp1|
#   fcmpu  %cr7,%f5,%f10            #compare Tmp1 with Epsilon
#   bge+   %cr7,.NoPeriodicity      #if Pzr >= Epsilon goto .NoPeriodicity
#   fsub   %f6,%f4,%f9              #Tmp2 = Zi - Pzi
#   fabs   %f6,%f6                  #Tmp2 = abs |Tmp2|
#   fcmpu  %cr5,%f6,%f10            #compare Tmp2 with Epsilon
#   blt-   %cr5,.PeriodicityExit    #if Tmp2 < Epsilon goto .PeriodicityExit

#   check periodicity method (2)
    fcmpu   %cr7,%f3,%f8            #compare Pzr with zr
    bne+    %cr7,.NoPeriodicity     #if not equal goto .NoPeriodicity
    fcmpu   %cr5,%f4,%f9            #compare Pzi with zi
    beq-    %cr5,.PeriodicityExit   #if equal goto .PeriodicityExit

.NoPeriodicity:
    cmpw    %cr6,%r5,%r6            #compare PLoop with MaxPeriod
    subic.  %r3,%r3,1               #Iterations--
    addi    %r5,%r5,1               #PLoop++
    blt+    %cr6,.MainLoop          #if PLoop < MaxPeriod goto .MainLoop

    fmr     %f8,%f3                 #Pzr = zr
    xor     %r5,%r5,%r5             #PLoop = 0
    fmr     %f9,%f4                 #Pzi = zi
    bgt+    %cr0,.MainLoop          #if Iterations > 0 goto .MainLoop (cr0 set by subic.)

.Exit:
    blr                             #return

.PeriodicityExit:
    xor    %r3,%r3,%r3              #Iterations = 0
#    li     %r3,254                  #for debug
    blr                             #return

    .size   MandelnPPC,.-MandelnPPC
    .section	.rodata.cst4,"aM",@progbits,4
    .align  2
.Radius:
    .long   1082130432              #radius = 4.0
#    .long	1074790400
#	.long	0     

#	.section	.rodata.cst8,"aM",@progbits,8   
#    .align  3
#.Epsilon:
#	.long	1020396463              #1e-15 high precision
#	.long	-1629006314
    
#    .long   1013386997             #1e-17 very high precision
#    .long   1020396463             #1e-15 high precision
#    .long   1037794527             #1e-10 mid precision
#    .long   1055193269             #1e-5 lower precision

