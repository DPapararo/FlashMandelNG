####################################################################################
#  $Ver: 1.0 first release - Dino Papararo 3 May 2020
#
#  FUNCTION
#
#    MandelnAltivecPPC -- perform Z = Z^n + C iteration (with n power of 2).
#
#  SYNOPSIS
#
#    unsigned long MandelnAltivecPPC (long Iterations,long Power,float Cre,float Cim)
#
#  This function tests if a point belongs or not at Mandelbrot's set
#  Handmade optimized for PowerPC processors.
#
#  r0:Tmp r3:Iterations r4:Power r5:PLoop r6:PTot r7:LoadFloat r8:Tmp1 r9:Tmp2
#  f0:MaxDist f1:Cre f2:Cim
#  v0:vMaxDist v1:vCre v2:vCim
#  v3:vZr v4:vZi v5:vZr2/vTmp1 v6:vZi2/vTmp2
#  v7:vDist v8:vPzr v9:vPzi v10:vEpsilon v11:vZero v12:vOne
####################################################################################

    .file "mandelnaltivecppc.s"

    .section	".text"
    .align		2
	.globl 		MandelnAltivecPPC
	.type		MandelnAltivecPPC, @function

MandelnAltivecPPC:
	stwu    %r1,-64(%r1)        #save 64 bytes on stack
    stfs 	%f1,16(%r1)         #save Cre on stack
    stfs 	%f2,32(%r1)         #save Cim on stack
#    vspltisw %v12,1              #vOne = 1
	lis		%r7,.Radius@ha		#set high 16bits r7 as Radius and clear lower 16bits
    la 		%r7,.Radius@l(%r7)  #load dword from Radius into f0
    lvx 	%v0,0,%r7           #vMaxDist = Tmp1
#    vspltw  %v0,%v0,0           #popolate all vector chunks
    vxor    %v11,%v11,%v11      #vZero = 0
    li      %r8,16              #Tmp1 = Cre
    li      %r9,32              #Tmp2 = Cim
    lvewx   %v1,%r1,%r8         #vCre = Tmp1
    lvewx   %v2,%r1,%r9         #vCim = Tmp2
#    vspltw  %v1,%v1,0           #popolate all vector chunks
#    vspltw  %v2,%v2,0           #popolate all vector chunks
    lvewx   %v3,%r1,%r8         #vZr = Tmp1
    lvewx   %v4,%r1,%r9         #vZi = Tmp2
#    vspltw  %v3,%v3,0           #popolate all vector chunks
#    vspltw  %v4,%v4,0           #popolate all vector chunks

#    lis 	 %r7,.Epsilon@ha     #set high 16bits r7 as Radius and clear lower 16bits
#    la 	 %r7,.Epsilon@l(%r7) #load Epsilon into v10
#    lvewx     %v10,0,%r7        #vEpsilon = Tmp1
#    vspltw  %v10,%v10,0         #popolate all vector chunks

#   lvewx   %v8,%r1,%r8         #vPzr = vZr
#    vspltw  %v8,%v8,0           #popolate all vector chunks
#   xor	    %r5,%r5,%r5		    #PLoop = 0
#   lvewx   %v9,%r1,%r9         #vPzi = vZi
#    vspltw  %v9,%v9,0           #popolate all vector chunks

#   li	    %r6,2 		        #PTot = 2

.MainLoop:
    mtctr	%r4					#Load Power into counter

.PowerLoop1:
    vmaddfp %v6,%v4,%v4,%v11    #vZi2 = vZi * vZi + vZero
    vmaddfp %v4,%v4,%v3,%v11    #vZi *= vZr + vZero
    vmaddfp %v5,%v3,%v3,%v11    #vZr2 = vZr * vZr + vZero
    vaddfp	%v4,%v4,%v4         #vZi += vZi
    vsubfp  %v3,%v5,%v6         #vZr = vZr2 - vZi2
    bdnz 	.PowerLoop1         #if Power > 0 goto .PowerLoop1

	vaddfp 	%v7,%v5,%v6         #Dist = vZr2 + vZi2
	vcmpgtfp. %v7,%v7,%v0       #compare dist with Radius
    bne-    %cr6,.Exit          #if dist > radius goto .Exit
#    cmpwi   %cr5,%r3,0          #compare Iterations with 0
#    beq-    %cr5,.Exit          #if Iterations == 0 goto .Exit
    vaddfp 	%v4,%v4,%v2         #vZi += vCim
    vaddfp  %v3,%v3,%v1         #vZr += vCre

#	check periodicity method (1) reusing Zr2 and Zi2 as Temp vars
#    vsubfp  %v5,%v3,%v8          #Tmp1 = Zr - Pzr
#    vspltisw %v5,-1              #
#    vslw %v6,%v5,%v5             #
#    vandc %v5,%v5,%v6            # Tmp1 = |Tmp1|
#    vcmpgtfp %v5,%v5,%v10       #compare Tmp1 with Epsilon
#    vand 	 %v5,%v5,%v12
#    beq    %cr6,.NoPeriodicity   #if Pzr >= Epsilon goto .NoPeriodicity
#    vsubfp  %v6,%v4,%v9          #Tmp2 = Zi - Pzi
#    vspltisw %v6,-1              #
#    vslw %v5,%v6,%v6             #
#    vandc %v6,%v6,%v5            # Tmp2 = |Tmp2|
#    vcmpgtfp %v6,%v6,%v10       #compare Tmp2 with Epsilon
#    vand 	 %v6,%v6,%v12
#    bne    %cr6,.Periodicity    #if Tmp2 < Epsilon goto .Periodicity

#   check periodicity method (2)
#    fcmpu   %cr7,%f3,%f8        #compare Pzr with zr
#    bne     %cr7,.NoPeriodicity #if not equal goto .NoPeriodicity
#    fcmpu   %cr5,%f4,%f9        #compare Pzi with zi
#    beq     %cr5,.Periodicity   #if equal goto .Periodicity

#.NoPeriodicity:
#    cmpw    %cr6,%r5,%r6        #compare PLoop with PTot
    subic.   %r3,%r3,1           #Iterations--
#    addi    %r5,%r5,1           #PLoop++
#    blt     %cr6,.MainLoop      #if PLoop < PTot goto .MainLoop

#    vaddfp  %v8,%v11,%v3        #Pzr = zr
#    add     %r6,%r6,%r6         #PTot += PTot
#    vaddfp  %v9,%v11,%v4        #Pzi = zi
    bgt+    %cr0,.MainLoop           #goto .MainLoop

#.Periodicity:
#    xor    %r3,%r3,%r3          #Iterations = 0
#    li     %r3,254             #for debug

.Exit:
    addi %r1,%r1,64
    blr                        	#return

    .size	MandelnAltivecPPC,.-MandelnAltivecPPC
    .section	.rodata

    .align  4
.Radius:
	.long	1082130432
	.long	1082130432
	.long	1082130432
	.long	1082130432
#    .long	 1074790400          #4.0 radius
#    .long	 1074790400          #4.0 radius
#    .long	 1074790400          #4.0 radius
#    .long	 1074790400          #4.0 radius

    .align  4
.Epsilon:
    .long   925353388
    .long   925353388
    .long   925353388
    .long   925353388
#     .long   1020396463         #1e-15 high precision
#     .long   1020396463         #1e-15 high precision
#     .long   1020396463         #1e-15 high precision
#     .long   1020396463         #1e-15 high precision
#	 .long	 1013386997        	 #1e-17 very high precision
#    .long   1037794527          #1e-10 mid precision
#    .long   1055193269          #1e-5 lower precision
#    .long   1055193269          #1e-5 lower precision
#    .long   1055193269          #1e-5 lower precision
#    .long   1055193269          #1e-5 lower precision
