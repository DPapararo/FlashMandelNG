# FlashMandel
# 1.0 Makefile created by Edgar Schwan
# 1.1 cleanup old ppc referred files and added new altivec options GCC flags - 6.4.2018 dpapararo
# 1.2 now enabled some specific GCC PowerPC optimizations like fused multiply add and others, makefile tabs fixed - 26.5.2018 dpapararo
# 1.3 altivec code compiled and linked to all core modules
# 1.4 now mymath modules are optimized for altivec version using GCC altivec options switches - 18.6.2018 dpapararo
# 1.5 added CFLAGSAV option for best modularity and better reading - set DEBUGLEVEL=0 - 3.2.2019 dpapararo
# 1.6 readded arexx menu - 5.5.2019 eschwan
# 1.7 added new binary due handmade powerpc asm optimized main routine - 20.01.20 dpapararo
# 1.8 added spe binary compilation for TABOR - 26.01.20 dpapararo
# 2.5 added GMP library 31-03-20 dpapararo
# 2.6 removed MyMath and debug symbols from binaries 19-04-20 dpapararo
# 2.7  rearranged optimization options and other small changes 05-05-20 dpapararo
# 2.8 removed pre OS4 defines
# 2.9 made some reaction/locale direcory reorganizations
# 3.0 small enanchements
# 3.1 Now ARexx stuff has is own drawer, Altivec version has ARexx support disabled
# $Ver: 3.2 added FlashMandel_lite version without Reaction and Arexx support


# GNU gcc
#
CC = gcc
AS = as
CCSPE = ppc-amigaos-gcc-5.4.0

COPTSIZE = -mcpu=604e -mtune=604e -Os -mno-altivec -ffp-contract=fast -ffast-math -mstrict-align -mregnames
COPTSPEED = -mcpu=604e -mtune=604e -O3 -mno-altivec -ffp-contract=fast -ffast-math -mstrict-align -mregnames -DUSE_POWERPC_MATH  
COPTSPEEDAV = -mcpu=7400 -mtune=7400 -O3 -maltivec -mabi=altivec -ffp-contract=fast -ffast-math -mstrict-align -mno-vrsave -mregnames -DUSE_ALTIVEC_MATH #-DUSE_ALTIVEC_ASM 
COPTSPEEDSPE = -mcpu=8540 -mtune=8540 -O3 -mno-altivec -ffast-math -mstrict-align -mregnames -DUSE_C_MATH #-mfloat-gprs=double
DEBUG_CFLAGS = -DNDEBUG -g -gstabs -DMYDEBUG=1 -DDEBUGLEVEL=1
CFLAGS_IFF = -w -Wextra $(COPTSIZE) -mcrt=newlib -IHeaders/amigaos4/include -I. -IHeaders
CFLAGS = -w -Wextra $(COPTSPEED) -mcrt=newlib -IHeaders/amigaos4/include -I. -IHeaders -D__USE_INLINE__ -D__USE_BASETYPE__ -DCATCOMP_ARRAY -DFM_REACT_SUPPORT -DFM_AREXX_SUPPORT
CFLAGSAV = -w -Wextra $(COPTSPEEDAV) -mcrt=newlib -IHeaders/amigaos4/include -I. -IHeaders -D__USE_INLINE__ -D__USE_BASETYPE__ -DCATCOMP_ARRAY -DFM_REACT_SUPPORT
CFLAGSSPE = -w -Wextra $(COPTSPEEDSPE) -mcrt=newlib -IHeaders/amigaos4/include -I. -IHeaders -D__USE_INLINE__ -D__USE_BASETYPE__ -DCATCOMP_ARRAY -DFM_REACT_SUPPORT -DFM_AREXX_SUPPORT
CFLAGS_NOREACT_NOAREXX = -w -Wextra $(COPTSPEED) -mcrt=newlib -IHeaders/amigaos4/include -I. -IHeaders -D__USE_INLINE__ -D__USE_BASETYPE__ -DCATCOMP_ARRAY
CFLAGS_NOAREXX = -w -Wextra $(COPTSPEED) -mcrt=newlib -IHeaders/amigaos4/include -I. -IHeaders -D__USE_INLINE__ -D__USE_BASETYPE__ -DCATCOMP_ARRAY -DFM_REACT_SUPPORT
ASFLAGS = -m7400 -mbig -mno-solaris
ASALTIVEC = -m7400 -maltivec -mno-solaris

# -DNDEBUG -DCATCOMP_NUMBERS -DCATCOMP_STRINGS -DFM_REACT_SUPPORT -DFM_AREXX_SUPPORT 
#AFLAGS = -m604e --defsym CATCOMP_ARRAY=1 $(DEBUG_CFLAGS)
# -m7400 -DFM_AREXX_SUPPORT  -DUSE_C_MATH -ffp-contract=fast
RM = C:Delete FORCE

# Dirs
O = ObjOS4
H = Headers
MOD = Modules
IFFMOD = $(MOD)/Iffmodules
GMPMOD = $(MOD)/GMP
AREXXMOD = $(MOD)/ARexx
REACTMOD = $(MOD)/React
REACTMOD_NOAREXX = $(MOD)/React_NoArexx
REACTLOCMOD = $(MOD)/Locale
AUTOINIT = $(MOD)/Autoinit

# Object-Files
MOD_O = $(O)/CalcJuliaOrbit.o $(O)/CalcMandelOrbit.o $(O)/ColorWindow.o $(O)/Gfx.o $(O)/Julian.o $(O)/Mandeln.o $(O)/JulianPPC.o $(O)/MandelnPPC.o $(O)/TurboRender.o $(O)/GfxRender.o
MOD_O_ALTIVEC = $(O)/CalcJuliaOrbit_av.o $(O)/CalcMandelOrbit_av.o $(O)/ColorWindow.o $(O)/Gfx.o $(O)/Julian.o $(O)/Mandeln.o $(O)/JulianAltivec.o $(O)/MandelnAltivec.o $(O)/TurboRender_av.o $(O)/GfxRender_av.o $(O)/MandelnAltivecPPC.o
MOD_O_SPE = $(O)/CalcJuliaOrbit_spe.o $(O)/CalcMandelOrbit_spe.o $(O)/ColorWindow.o $(O)/Gfx.o $(O)/JulianSpe.o $(O)/MandelnSpe.o $(O)/TurboRender_spe.o $(O)/GfxRender_spe.o
MOD_O_LITE = $(O)/CalcJuliaOrbit.o $(O)/CalcMandelOrbit.o $(O)/ColorWindow.o $(O)/Gfx.o $(O)/Julian.o $(O)/Mandeln.o $(O)/JulianPPC.o $(O)/MandelnPPC.o $(O)/TurboRender.o $(O)/GfxRender.o
IFFMOD_O = $(O)/FMIlbm.o $(O)/iffpstrings.o  
GMP_MOD_O = $(O)/Julian_GMP.o $(O)/Mandeln_GMP.o $(O)/CalcJulianOrbit_GMP.o $(O)/CalcMandelnOrbit_GMP.o $(O)/Render_GMP.o
AREXXMOD_O = $(O)/FM_ARexx_Misc.o $(O)/FM_ARexx_React.o $(O)/FM_ARexx_React2.o
REACTMOD_O = $(O)/FM_ReactionBasics.o $(O)/FM_PalettePref_React.o $(O)/FM_ConfirmReq_React.o $(O)/FM_CoordReq_React.o $(O)/FM_InfoReq_React.o $(O)/FM_IntegerReq_React.o $(O)/FM_SysInfoReq_React.o 
REACTMOD_NOAREXX_O = $(O)/FM_ReactionBasics_noarexx.o $(O)/FM_PalettePref_React_noarexx.o $(O)/FM_ConfirmReq_React_noarexx.o $(O)/FM_CoordReq_React_noarexx.o $(O)/FM_InfoReq_React_noarexx.o $(O)/FM_IntegerReq_React_noarexx.o $(O)/FM_SysInfoReq_React_noarexx.o 
AUTOINIT_O = #$(O)/autoinit_palette_base.o #$(O)/autoinit_palette_main.o
 
OBJ = $(O)/FlashMandel.o $(MOD_O) $(GMP_MOD_O) $(IFFMOD_O) $(AREXXMOD_O) $(REACTMOD_O) 
  # $(AUTOINIT_O)
  # $(O)/aos4_debug.o
  # $(REACTLOCMOD)/FM_Reaction.o
OBJ_NOREACT_NOAREXX = $(O)/FlashMandel_lite.o $(MOD_O_LITE) $(GMP_MOD_O) $(IFFMOD_O)
  # $(AUTOINIT_O)
  # $(O)/aos4_debug.o
  # $(REACTLOCMOD)/FM_Reaction.o  
OBJ_ALTIVEC = $(O)/FlashMandel_av.o $(MOD_O_ALTIVEC) $(GMP_MOD_O) $(IFFMOD_O) $(REACTMOD_NOAREXX_O)
  # $(AUTOINIT_O)
  # $(O)/aos4_debug.o
  # $(REACTLOCMOD)/FM_Reaction.o
OBJ_SPE = $(O)/FlashMandel_spe.o $(MOD_O_SPE) $(GMP_MOD_O) $(IFFMOD_O) $(AREXXMOD_O) $(REACTMOD_O)
  # $(AUTOINIT_O)
  # $(O)/aos4_debug.o
  # $(REACTLOCMOD)/FM_Reaction.o


# Header-Files
AREXXMOD_H = $(H)/FM_ARexx_Misc.h $(H)/FM_ARexx_React.h
REACTMOD_H = $(H)/FM_ReactionBasics.h $(H)/FM_PalettePref_React.h $(H)/FM_ConfirmReq_React.h $(H)/FM_CoordReq_React.h $(H)/FM_InfoReq_React.h $(H)/FM_IntegerReq_React.h $(H)/FM_SysInfoReq_React.h
REACTLOCMOD_H = $(REACTLOCMOD)/FM_Reaction.h $(REACTLOCMOD)/FM_ReactionCD.h

# to compile for altivec, spe or lite remove the '#' in the following line
all: FlashMandelNG_OS4 FlashMandelNG_OS4_altivec FlashMandelNG_OS4_lite #FlashMandelNG_OS4_spe

clean:
	$(RM) $(O)/#?.o
	$(RM) FlashMandelNG

FlashMandelNG_OS4_lite: $(OBJ_NOREACT_NOAREXX)
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -o FlashMandelNG_OS4_lite $(OBJ_NOREACT_NOAREXX) -lgmp-604e -lm -lamiga -lauto
#strip -s FlashMandelNG_OS4

FlashMandelNG_OS4: $(OBJ)
	$(CC) $(CFLAGS) -o FlashMandelNG_OS4 $(OBJ) -lgmp-604e -lm -lamiga -lauto
#strip -s FlashMandelNG_OS4

FlashMandelNG_OS4_altivec: $(OBJ_ALTIVEC)
	$(CC) $(CFLAGSAV) -o FlashMandelNG_OS4_altivec $(OBJ_ALTIVEC) -lgmp-7450-altivec -lm -lamiga -lauto
#strip -s FlashMandelNG_OS4

FlashMandelNG_OS4_spe: $(OBJ_SPE)
	$(CCSPE) $(CFLAGSSPE) -o FlashMandelNG_OS4_spe $(OBJ_SPE) -lgmp-generic -lm -lamiga -lauto
#strip -s FlashMandelNG_OS4

#$(O)/aos4_debug.o : aos4_debug.c aos4_debug.h
#	 $(CC) $(CFLAGS) -c aos4_debug.c -o $(O)/aos4_debug.o

$(O)/FlashMandel.o : FlashMandel.c $(H)/FlashMandel.h $(REACTMOD_H) $(REACTLOCMOD_H) $(AREXXMOD_H)
	$(CC) $(CFLAGS) -c FlashMandel.c -o $(O)/FlashMandel.o

$(O)/FlashMandel_av.o : FlashMandel.c $(H)/FlashMandel.h $(REACTMOD_H) $(REACTLOCMOD_H)
	$(CC) $(CFLAGSAV) -c FlashMandel.c -o $(O)/FlashMandel_av.o

$(O)/FlashMandel_spe.o : FlashMandel.c $(H)/FlashMandel.h $(REACTMOD_H) $(REACTLOCMOD_H) $(AREXXMOD_H)
	$(CCSPE) $(CFLAGSSPE) -c FlashMandel.c -o $(O)/FlashMandel_spe.o

$(O)/FlashMandel_lite.o : FlashMandel.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -c FlashMandel.c -o $(O)/FlashMandel_lite.o
 
$(O)/MandelnPPC.o : $(MOD)/MandelnPPC.s
	$(AS) $(ASFLAGS) $(MOD)/MandelnPPC.s -o $(O)/MandelnPPC.o

$(O)/JulianPPC.o : $(MOD)/JulianPPC.s
	$(AS) $(ASFLAGS) $(MOD)/JulianPPC.s -o $(O)/JulianPPC.o

$(O)/MandelnAltivecPPC.o : $(MOD)/MandelnAltivecPPC.s
	$(AS) $(ASFLAGS) $(ASALTIVEC) $(MOD)/MandelnAltivecPPC.s -o $(O)/MandelnAltivecPPC.o

$(O)/CalcJuliaOrbit.o : $(MOD)/CalcJuliaOrbit.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CC) $(CFLAGS) -c $(MOD)/CalcJuliaOrbit.c -o $(O)/CalcJuliaOrbit.o
	
$(O)/CalcJuliaOrbit_av.o : $(MOD)/CalcJuliaOrbit.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CC) $(CFLAGSAV) -c $(MOD)/CalcJuliaOrbit.c -o $(O)/CalcJuliaOrbit_av.o	

$(O)/CalcJuliaOrbit_spe.o : $(MOD)/CalcJuliaOrbit.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CCSPE) $(CFLAGSSPE) -c $(MOD)/CalcJuliaOrbit.c -o $(O)/CalcJuliaOrbit_spe.o

$(O)/CalcMandelOrbit.o : $(MOD)/CalcMandelOrbit.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CC) $(CFLAGS) -c $(MOD)/CalcMandelOrbit.c -o $(O)/CalcMandelOrbit.o

$(O)/CalcMandelOrbit_av.o : $(MOD)/CalcMandelOrbit.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CC) $(CFLAGSAV) -c $(MOD)/CalcMandelOrbit.c -o $(O)/CalcMandelOrbit_av.o	

$(O)/CalcMandelOrbit_spe.o : $(MOD)/CalcMandelOrbit.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CCSPE) $(CFLAGSSPE) -c $(MOD)/CalcMandelOrbit.c -o $(O)/CalcMandelOrbit_spe.o
 
$(O)/ColorWindow.o : $(MOD)/ColorWindow.c $(H)/FlashMandel.h $(H)/CompilerSpecific.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H)
	$(CC) $(CFLAGS) -c $(MOD)/ColorWindow.c -o $(O)/ColorWindow.o

$(O)/Gfx.o : $(MOD)/Gfx.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(MOD)/Gfx.c -o $(O)/Gfx.o

$(O)/Julian.o : $(MOD)/Julian.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(MOD)/Julian.c -o $(O)/Julian.o

$(O)/JulianAltivec.o : $(MOD)/JulianAltivec.c $(H)/FlashMandel.h
	$(CC) $(CFLAGSAV) -c $(MOD)/JulianAltivec.c -o $(O)/JulianAltivec.o
  
$(O)/JulianSpe.o : $(MOD)/Julian.c $(H)/FlashMandel.h
	$(CCSPE) $(CFLAGSSPE) -c $(MOD)/Julian.c -o $(O)/JulianSpe.o
 
$(O)/Mandeln.o : $(MOD)/Mandeln.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(MOD)/Mandeln.c -o $(O)/Mandeln.o

$(O)/MandelnAltivec.o : $(MOD)/MandelnAltivec.c $(H)/FlashMandel.h
	$(CC) $(CFLAGSAV) -c $(MOD)/MandelnAltivec.c -o $(O)/MandelnAltivec.o

$(O)/MandelnSpe.o : $(MOD)/Mandeln.c $(H)/FlashMandel.h
	$(CCSPE) $(CFLAGSSPE) -c $(MOD)/Mandeln.c -o $(O)/MandelnSpe.o

$(O)/TurboRender.o : $(MOD)/TurboRender.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(MOD)/TurboRender.c -o $(O)/TurboRender.o

$(O)/TurboRender_av.o : $(MOD)/TurboRender.c $(H)/FlashMandel.h
	$(CC) $(CFLAGSAV) -c $(MOD)/TurboRender.c -o $(O)/TurboRender_av.o

$(O)/TurboRender_spe.o : $(MOD)/TurboRender.c $(H)/FlashMandel.h
	$(CCSPE) $(CFLAGSSPE) -c $(MOD)/TurboRender.c -o $(O)/TurboRender_spe.o

$(O)/GfxRender.o : $(MOD)/GfxRender.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(MOD)/GfxRender.c -o $(O)/GfxRender.o

$(O)/GfxRender_av.o : $(MOD)/GfxRender.c $(H)/FlashMandel.h
	$(CC) $(CFLAGSAV) -c $(MOD)/GfxRender.c -o $(O)/GfxRender_av.o

$(O)/GfxRender_spe.o : $(MOD)/GfxRender.c $(H)/FlashMandel.h
	$(CCSPE) $(CFLAGSSPE) -c $(MOD)/GfxRender.c -o $(O)/GfxRender_spe.o
 
#IFF
$(O)/FMIlbm.o : $(IFFMOD)/FMIlbm.c
	$(CC) $(CFLAGS_IFF) -c $(IFFMOD)/FMIlbm.c -o $(O)/FMIlbm.o

$(O)/iffpstrings.o : $(IFFMOD)/iffpstrings.c
	$(CC) $(CFLAGS_IFF) -c $(IFFMOD)/iffpstrings.c -o $(O)/iffpstrings.o

#GMP
$(O)/Julian_GMP.o : $(GMPMOD)/Julian_GMP.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -c $(GMPMOD)/Julian_GMP.c -o $(O)/Julian_GMP.o

$(O)/Mandeln_GMP.o : $(GMPMOD)/Mandeln_GMP.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -c $(GMPMOD)/Mandeln_GMP.c -o $(O)/Mandeln_GMP.o

$(O)/CalcJulianOrbit_GMP.o : $(GMPMOD)/CalcJulianOrbit_GMP.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -c $(GMPMOD)/CalcJulianOrbit_GMP.c -o $(O)/CalcJulianOrbit_GMP.o

$(O)/CalcMandelnOrbit_GMP.o : $(GMPMOD)/CalcMandelnOrbit_GMP.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -c $(GMPMOD)/CalcMandelnOrbit_GMP.c -o $(O)/CalcMandelnOrbit_GMP.o

$(O)/Render_GMP.o : $(GMPMOD)/Render_GMP.c $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOREACT_NOAREXX) -c $(GMPMOD)/Render_GMP.c -o $(O)/Render_GMP.o
 
#REACTION
$(O)/FM_ARexx_Misc.o : $(AREXXMOD)/FM_ARexx_Misc.c $(H)/FlashMandel.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H) $(AREXXMOD_H)
	$(CC) $(CFLAGS) -c $(AREXXMOD)/FM_ARexx_Misc.c -o $(O)/FM_ARexx_Misc.o

$(O)/FM_ARexx_React.o : $(AREXXMOD)/FM_ARexx_React.c $(H)/FlashMandel.h $(H)/Compilerspecific.h $(REACTMOD_H) $(REACTLOCMOD_H) $(AREXXMOD_H)
	$(CC) $(CFLAGS) -c $(AREXXMOD)/FM_ARexx_React.c -o $(O)/FM_ARexx_React.o

$(O)/FM_ARexx_React2.o : $(AREXXMOD)/FM_ARexx_React2.c $(H)/FlashMandel.h $(H)/FM_ReactionBasics.h $(REACTLOCMOD_H) $(AREXXMOD_H)
	$(CC) $(CFLAGS) -c $(AREXXMOD)/FM_ARexx_React2.c -o $(O)/FM_ARexx_React2.o

$(O)/FM_ConfirmReq_React.o : $(REACTMOD)/FM_ConfirmReq_React.c $(H)/FM_ConfirmReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_ConfirmReq_React.c -o $(O)/FM_ConfirmReq_React.o

$(O)/FM_CoordReq_React.o : $(REACTMOD)/FM_CoordReq_React.c $(H)/FM_CoordReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_CoordReq_React.c -o $(O)/FM_CoordReq_React.o

$(O)/FM_InfoReq_React.o : $(REACTMOD)/FM_InfoReq_React.c $(H)/FM_InfoReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_InfoReq_React.c -o $(O)/FM_InfoReq_React.o

$(O)/FM_IntegerReq_React.o : $(REACTMOD)/FM_IntegerReq_React.c $(H)/FM_IntegerReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_IntegerReq_React.c -o $(O)/FM_IntegerReq_React.o

$(O)/FM_PalettePref_React.o : $(REACTMOD)/FM_PalettePref_React.c $(H)/FM_PalettePref_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_PalettePref_React.c -o $(O)/FM_PalettePref_React.o

$(O)/FM_ReactionBasics.o : $(REACTMOD)/FM_ReactionBasics.c $(H)/FM_ReactionBasics.h  $(REACTLOCMOD_H) $(H)/FlashMandel.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_ReactionBasics.c -o $(O)/FM_ReactionBasics.o

$(O)/FM_SysInfoReq_React.o : $(REACTMOD)/FM_SysInfoReq_React.c $(H)/FM_SysInfoReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS) -c $(REACTMOD)/FM_SysInfoReq_React.c -o $(O)/FM_SysInfoReq_React.o

#AUTOINIT
$(O)/autoinit_palette_base.o : $(REACTMOD)/autoinit_palette_base.c
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/autoinit_palette_base.c -o $(O)/autoinit_palette_base.o

$(O)/autoinit_palette_main.o : $(REACTMOD)/autoinit_palette_main.c
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/autoinit_palette_main.c -o $(O)/autoinit_palette_main.o

#REACTION_NOAREXX	
$(O)/FM_ConfirmReq_React_noarexx.o : $(REACTMOD)/FM_ConfirmReq_React.c $(H)/FM_ConfirmReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_ConfirmReq_React.c -o $(O)/FM_ConfirmReq_React_noarexx.o

$(O)/FM_CoordReq_React_noarexx.o : $(REACTMOD)/FM_CoordReq_React.c $(H)/FM_CoordReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_CoordReq_React.c -o $(O)/FM_CoordReq_React_noarexx.o

$(O)/FM_InfoReq_React_noarexx.o : $(REACTMOD)/FM_InfoReq_React.c $(H)/FM_InfoReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_InfoReq_React.c -o $(O)/FM_InfoReq_React_noarexx.o

$(O)/FM_IntegerReq_React_noarexx.o : $(REACTMOD)/FM_IntegerReq_React.c $(H)/FM_IntegerReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_IntegerReq_React.c -o $(O)/FM_IntegerReq_React_noarexx.o

$(O)/FM_PalettePref_React_noarexx.o : $(REACTMOD)/FM_PalettePref_React.c $(H)/FM_PalettePref_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_PalettePref_React.c -o $(O)/FM_PalettePref_React_noarexx.o

$(O)/FM_ReactionBasics_noarexx.o : $(REACTMOD)/FM_ReactionBasics.c $(H)/FM_ReactionBasics.h  $(REACTLOCMOD_H) $(H)/FlashMandel.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_ReactionBasics.c -o $(O)/FM_ReactionBasics_noarexx.o

$(O)/FM_SysInfoReq_React_noarexx.o : $(REACTMOD)/FM_SysInfoReq_React.c $(H)/FM_SysInfoReq_React.h $(REACTLOCMOD_H) $(H)/FM_ReactionBasics.h
	$(CC) $(CFLAGS_NOAREXX) -c $(REACTMOD)/FM_SysInfoReq_React.c -o $(O)/FM_SysInfoReq_React_noarexx.o