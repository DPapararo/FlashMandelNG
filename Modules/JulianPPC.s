#########################################################################################################
#  V2.1 handmade asm main routine - Dino Papararo 20 Jan 2020
#  V2.2 rescheduled some instruction - Dino Papararo 22 Jan 2020
#  V2.3 added periodicity checking - Dino Papararo 13 Apr 2020
#  Ver 2.4 small speedup changes - Dino Papararo 03 May 2020
#  Ver 2.5 modified Periodicity check, another speedup! - Dino Papararo 24 Dic 2020
#  Ver 2.6 initialized Zr2 and Zi2 to zero - Dino Papararo 11 Feb 2024
#  $Ver 2.7 speedup and fixes PeriodCheck - Dino Papararo 09 Nov 2024
#
#  FUNCTION
#
#    JulianPPC -- perform Z = (Z^(2^n)) + C iteration
#
#  SYNOPSIS
#
#    unsigned long JulianPPC (long Iterations,long Power,double Cre,double Cim, double JKre, double JKim)
#
#  This function tests if a point belongs or not at a Julia's set
#  Handmade optimized for PowerPC processors.
#
#  r3:Iterations r4:Power r5:PLoop r6:MaxPeriod r7:LoadRadius r8:LoadEpsilon
#  f0:MaxDist f1:Cre f2:Cim f3:JKre f4:JKim f5:Zr2/Tmp1 f6:Zi2x2/Tmp2
#  f7:Dist f8:Pzr f9:Pzi f10:Epsilon f11:Zr f12:Zi f13:ZrZi
#########################################################################################################

    .file "julianppc.s"

    .section ".text"
    .align   2
    .globl   JulianPPC
    .type    JulianPPC, @function

JulianPPC:
    lis     %r7,.Radius@ha          #set high 16bits r7 as Radius and clear lower 16bits
    fmr     %f11,%f1                #Zr = Cre
	lis		%r8,.Epsilon@ha			#set high 16bits r7 as Epsilon and clear lower 16bits
	fmr     %f12,%f2                #Zi = Cim	
	la		%r7,.Radius@l(%r7)		#Load low word Radius
    fmr     %f8,%f1                 #Pzr = Cre
    lfs		%f10,.Epsilon@l(%r8)	#Load Epsilon in f10
	fmr 	%f9,%f2					#Pzi = Cim
    li      %r6,25          		#MaxPeriod = 25 - periodicity check interval
	lfs		%f0,0(%r7)				#MaxDist = 4.0 - load Radius in MaxDist (f0)
	mr      %r5,%r6          		#PLoop = MaxPeriod

.MainLoop:
    mtctr  %r4                      #Load Power into counter

.PowerLoop1:
    fmul    %f5,%f11,%f11           #Zr2 = Zr * Zr
    fmul    %f6,%f12,%f12           #Zi2x2 = Zi * Zi
    fmul    %f13,%f12,%f11          #ZrZi = Zr * Zi
    fsub    %f11,%f5,%f6            #Zr = Zr2 - Zi2x2
    fadd    %f12,%f13,%f13          #Zi = ZrZi + ZrZi
    bdnz    .PowerLoop1             #if --Power > 0 goto .PowerLoop1

    fadd    %f7,%f5,%f6             #Dist = Zr2 + Zi2x2
    fcmpu   %cr5,%f7,%f0            #compare dist with MaxDist
    bgt-    %cr5,.Exit              #if dist > radius goto .Exit

    subic.  %r5,%r5,1				#PLoop--
	bgt+	%cr0,.NoPeriodicity     #if PLoop > 0 goto .NoPeriodicity (cr0 set by subic.)

#	check periodicity reusing Zr2 and Zi2x2 as Tmp1 and Tmp2 vars
   	fsub   %f5,%f3,%f8              #Tmp1 = Zr - Pzr
   	fabs   %f5,%f5                  #Tmp1 = abs |Tmp1|
   	fcmpu  %cr1,%f5,%f10            #compare Tmp1 with Epsilon
   	blt+   %cr1,.NoPeriodicity      #if Tmp1 >= Epsilon goto .NoPeriodicity
   	fsub   %f6,%f4,%f9              #Tmp2 = Zi - Pzi
   	fabs   %f6,%f6                  #Tmp2 = abs |Tmp2|
   	fcmpu  %cr2,%f6,%f10            #compare Tmp2 with Epsilon
   	blt-   %cr2,.PeriodicityExit    #if Tmp2 < Epsilon goto .PeriodicityExit

    fmr     %f8,%f11                #Pzr = Zr
    mr     	%r5,%r6                	#PLoop = MaxPeriod
    fmr     %f9,%f12                #Pzi = Zi

.NoPeriodicity:
    fadd    %f11,%f3,%f11           #Zr += JKre
    subic.  %r3,%r3,1               #Iterations--
    fadd    %f12,%f4,%f12           #Zi += JKim
    bgt+    %cr0,.MainLoop          #if Iterations > 0 goto .MainLoop (cr0 set by subic.)
	
.Exit:
    blr                             #return

.PeriodicityExit:
    xor    %r3,%r3,%r3              #Iterations = 0
#    li     %r3,254                 #for debug
    blr                             #return

    .size    JulianPPC,.-JulianPPC
   	.section	.rodata.cst4,"aM",@progbits,4
    .align  2
.Radius:
    .long   1082130432              #radius = 4.0
.Epsilon:
	.long	646978941  	 			#1e-15 high precision tolerance
	.ident	"Dino Papararo asm hand code"
	.gnu_attribute 4, 1
