	.file	"Mandeln.c"
	.section	".text"
	.align 2
	.globl Mandeln
	.type	Mandeln, @function
Mandeln:
	cmpwi %cr5,%r4,0
	fmr %f12,%f1
	lis %r8,.LC1@ha
	fmr %f0,%f2
	la %r8,.LC1@l(%r8)
	li %r10,0
	lis %r7,.LC2@ha
.L8:
	ble- %cr5,.L2
	addi %r9,%r4,-1
	rlwinm %r9,%r9,0,0xffff
	addi %r9,%r9,1
	mtctr %r9
.L3:
	fmul %f10,%f0,%f0
	fmul %f11,%f12,%f12
	fmul %f0,%f12,%f0
	fsub %f12,%f11,%f10
	fadd %f0,%f0,%f0
	bdnz .L3
.L2:
	fadd %f9,%f11,%f10
	lfs %f8,0(%r8)
	fcmpu %cr7,%f9,%f8
	bgtlr- %cr7
	fadd %f12,%f12,%f1
	lfd %f8,.LC2@l(%r7)
	fadd %f0,%f0,%f2
	fsub %f9,%f12,%f7
	fabs %f9,%f9
	fcmpu %cr7,%f9,%f8
	bnl- %cr7,.L5
	fsub %f9,%f0,%f6
	fabs %f9,%f9
	fcmpu %cr7,%f9,%f8
	blt- %cr7,.L10
.L5:
	cmplwi %cr7,%r10,19
	bgt- %cr7,.L11
	addi %r10,%r10,1
.L7:
	addic. %r3,%r3,-1
	bne+ %cr0,.L8
.L10:
	li %r3,0
	blr
.L11:
	fmr %f6,%f0
	li %r10,0
	fmr %f7,%f12
	b .L7
	.size	Mandeln, .-Mandeln
	.section	.rodata.cst4,"aM",@progbits,4
	.align 2
.LC1:
	.long	1082130432
	.section	.rodata.cst8,"aM",@progbits,8
	.align 3
.LC2:
	.long	1020396463
	.long	-1629006314
	.ident	"GCC: (adtools build 8.4.0) 8.4.0"
	.gnu_attribute 4, 1
