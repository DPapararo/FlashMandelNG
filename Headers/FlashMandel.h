/*
 *  FlashMandel.h $Ver 3.8 Dino Papararo 08 Nov 2023
 */

/* Modified for ARexx-support by E. Schwan 23.1.2002 */
/* added ppc g3/g4/g5 support by D. Papararo 30.3.2002 */
/* added PREVIEW_MSG definition for Orbit Window support 20.02.2003 */
/* Modified for AmigaOS4/GCC 15.09.2004, Edgar Schwan */
/* Added MINLIMIT2 and MINLIMIT4 #define values for better reading 18.01.2010 */
/* Removed as much as possible 68k related code 01.06.2018 dino papararo */
/* Added CMASK screen custom bitmap flag 03.06.2018 dpapararo */
/* optimized altivec routines 01.11.18 */
/* added prototypes for Altivec functions and updated email address 03 Feb 2019 dpapararo */
/* modified DEF_MONITOR values */
/* added GMP support in MandelChunk and changed ID_MAND 03-04-2020 */
/* added Histogram mask HMASK 01-01-2021 */
/* added Histogram function  17-01-2021 */
/* added support for SPE 02-01-2023 */
/* added Boundary trace algorithm 08-11-2023 */

#ifndef FLASHMANDEL_H
#define FLASHMANDEL_H

#include <exec/types.h>
#include <GMP/gmp.h>

#define Lib_Version 50L

#define VERSION  "FlashMandelNG 4.9 "
#define AUTHOR   "Dino Papararo"
#define COPYRIGHT_DATE "(c)1995-2023"
#define ADDRESS  "Via Manzoni, 184\n  80123 Napoli\n  Italy"
#define EMAIL    "E-Mail address:\n  Dino.Papararo@GMail.Com\n\n"

#define MAX_PRECISION_BITS 4096
#define MIN_PRECISION_BITS 32
#define DEF_PRECISION_BITS 128

#define DEF_MATH_DIGITS 39 /* DIGITS FOR 128 BIT PRECISION */
#define MAX_MATH_DIGITS 1235 /* DIGITS = LOG(X)(2^MAX_PRECISION_BITS) (WHERE X = BASE10) = 1235*/
#define MIN_MATH_DIGITS 10

#define MIN_ITERATIONS 64
#define DEF_ITERATIONS 4096
#define DEF_ITERATIONSSTR "4096"
#define MAX_ALLOWED_ITERATIONS 1073741824 /* (2^30) or 0X3FFFFFFF */

struct MandelChunk
{
    int32 LeftEdge,TopEdge,Width,Height;
    uint32 ModeID;
    int16 Depth, Power;
    uint32 PixelFormat,Iterations,Modulo,Flags;
    uint32 PrecisionBits,PrecisionDigits;
    float64 RMin,IMin,RMax,IMax;
    float64 JKre,JKim;
    mpf_t GRMin,GIMin,GRMax,GIMax;
    mpf_t GJKre,GJKim;
};

struct LoadSaveFMChunk
{
    int32 LeftEdge,TopEdge,Width,Height;
    uint32 ModeID;
    int16 Depth,Power;
    uint32 PixelFormat,Iterations,Modulo,Flags;
    uint32 PrecisionBits,PrecisionDigits;
    float64 RMin,IMin,RMax,IMax;
    float64 JKre,JKim;
    char GRMinSTR [MAX_MATH_DIGITS],GIMinSTR [MAX_MATH_DIGITS],GRMaxSTR [MAX_MATH_DIGITS],GIMaxSTR [MAX_MATH_DIGITS];
    char GJKreSTR [MAX_MATH_DIGITS],GJKimSTR [MAX_MATH_DIGITS];
};

struct UndoBuffer
{
    float64 AreaRange [4],JuliaConst [2]; /* rmin,rmax,imin,imax - jkre,jkim */
    uint32 Iterations,Flags;
    uint32 PrecisionBits,PrecisionDigits;
    int16 Depth,Power;
    mpf_t GRMin,GIMin,GRMax,GIMax;
    mpf_t GJKre,GJKim;
};

/* prototypes */
void Init_GMP (uint32);
void Clear_GMP (void);
void Reset_GMP (uint32);
void Init_MANDChunk_GMP (uint32);
void Clear_MANDChunk_GMP (void);
void Reset_MANDChunk_GMP (uint32);
void Clear_UNDOBuffer_GMP (uint32);
int32 SetPubScreenName (STRPTR);
uint32 CheckCPU (void);
uint32 Fail (STRPTR, uint32);
void FreeBitMapSafety (struct BitMap *);
int16 ClearZoomFrame (struct RastPort *);
void PutPointer (struct Window *,uint16 *,int16,int16,int16,int16,uint8);
int32 About (struct Window *);
void AdjustRatio (mpf_t *,mpf_t *,mpf_t *,mpf_t *,int16,int16,int16);
void ShowTime (struct Window *,STRPTR,int32);
uint32 IntegerGad (struct Window *,STRPTR,STRPTR,STRPTR,uint32);
int32 Choice (struct Window *,STRPTR,STRPTR);
void CloseDisplay (struct ILBMInfo *);
int32 MakeDisplay (struct ILBMInfo *);
int16 NewCoords (struct Window *,const int16,const int16,const int16,const int16);
int16 DrawFrame (struct Window *,const int16,const int16,const int16,const int16);
int16 Preview (struct Window *,uint8 *,uint8*,uint8 *,uint32 *,uint8 *,uint8 *,int16,int16);
int16 Orbit (struct Window *,int16,int16);
void DrawAxis (struct Window *,int16,int16);
int16 ShowOrbit (struct MandelChunk *,struct Window *,struct Window *);
int16 CheckBox (struct RastPort *, const int16, const int16, const int16, const int16);
int16 CheckBoxMem (struct MandelChunk *, uint32 *, const int16, const int16, const int16, const int16);
uint32 CalcMandelnOrbit (int16 *,uint32,int16,int16,int16,float64,float64);
uint32 CalcMandelnOrbit_GMP (int16 *,uint32,int16,int16,int16,mpf_t,mpf_t);
uint32 CalcJulianOrbit (int16 *,uint32,int16,int16,int16,float64,float64,float64,float64);
uint32 CalcJulianOrbit_GMP (int16 *,uint32,int16,int16,int16,mpf_t,mpf_t,mpf_t,mpf_t);
int16 ShowCoords (struct Window *);
int32 SaveCoords (struct Window *);
int32 RestoreCoords (struct Window *);
int16 FileRequest (struct Window *,STRPTR,STRPTR,int16,int16);
int16 FontRequest (struct Window *);
int16 SMRequest (struct ILBMInfo *);
void SetMenuStart (struct ILBMInfo *, int16 UndoBuffer);
void SetMenuStop (struct ILBMInfo *);
void DisplayRndMem (struct MandelChunk *,struct Window *,uint32 *,uint8 *);
void Histogram (struct MandelChunk *,struct Window *,uint8 *,uint32 *,uint32 *);
uint32 ProcessMenu (struct MandelChunk *,struct Window *,uint8 *,uint8 *,uint8 *,uint32 *,uint32 *,uint8 *,uint16);
int16 PickJuliaK (struct MandelChunk *,struct Window *,uint8 *,uint8 *,uint8*,uint32 *,uint32 *,uint8 *);
void Scan (struct MandelChunk *, struct RastPort *, uint32 *, uint32, uint32 , int16);
uint32 Load (struct MandelChunk *, struct RastPort *, uint32 *, uint32, int16);
void AddQueue (uint32, uint32);
int16 AllocateBoundary (uint32, uint32);
void DeallocateBoundary (void); 
void CheckMenu (struct Window *);
void ProcessMouse (struct Window *,int16,int16,int32);
uint32 HandleEvents (struct ILBMInfo *,struct MandelChunk *);
int32 WinDump (struct Window *);
int32 MainProg (struct ILBMInfo *,struct MandelChunk *);
int16 PasteBitMap (struct BitMap *,struct Window *,uint16,uint16,uint16,uint16);
struct Screen *OpenIdScreen (struct ILBMInfo *,int16,int16,int16,uint32);
struct Window *OpenDisplay (struct ILBMInfo *,int16,int16,int16,uint32);
struct BitMap *CopyBitMap (struct Window *,uint16,uint16,uint16,uint16);

void CalcFractal (struct MandelChunk *,struct Window *,uint8 *,uint8 *,uint8 *,uint8 *,uint32 *,uint32 *,uint32 *);
void CalcFractalMem (struct MandelChunk *, struct Window *, uint32 *, uint32 *, uint32 *);
void CalcFractal_GMP (struct MandelChunk *, struct Window *, uint8 *, uint8 *, uint8 *, uint8 *, uint32 *, uint32 *, uint32 *);
void CalcFractalMem_GMP (struct MandelChunk *, struct Window *, uint32 *, uint32 *);

#ifdef USE_ALTIVEC_MATH
uint32 MandelnAltivec (uint32 *,uint32,int16,float32,float32,float32,float32,float32,float32,float32,float32);
uint32 JulianAltivec (uint32 *,uint32,int16,float32,float32,float32,float32,float32,float32,float32,float32,float32,float32);
#elif USE_POWERPC_MATH
uint32 MandelnPPC (uint32,int16,float64,float64);
uint32 JulianPPC (uint32,int16,float64,float64,float64,float64);
#elif USE_C_MATH
uint32 Mandeln (uint32,int16,float64,float64);
uint32 Julian (uint32,int16,float64,float64,float64,float64);
#elif USE_SPE_MATH
uint32 MandelnSPE (uint32,int16,float64,float64);
uint32 JulianSPE (uint32,int16,float64,float64,float64,float64);
#endif /* USE_ALTIVEC_MATH */

uint32 DrawFractal (struct MandelChunk *,struct Window *,uint8 *,uint8 *,uint8 *,uint8 *,uint32 *,uint32 *,int16);
int16 ModifyPalette (struct Window *,int16,int16,uint32 *);
int16 ScalePalette (struct Window *,uint32 *,uint32,uint32);
int16 Fade (struct Window *,uint8 *,uint8 *,uint32 *,uint32,uint32,int16);
int16 Cycle (struct Window *,uint32,int16);
int32 QueryMandFile (struct ILBMInfo *,struct LoadSaveFMChunk *,STRPTR);
int32 QueryMandPic (struct ILBMInfo *,struct LoadSaveFMChunk *,STRPTR);
int32 LoadMandPic (struct ILBMInfo *,STRPTR);
int32 SaveMandPic (struct ILBMInfo *,struct LoadSaveFMChunk *,STRPTR,STRPTR,STRPTR);
int32 LoadPalette (struct ILBMInfo *,STRPTR);
int32 SavePalette (struct ILBMInfo *,struct Chunk *,struct Chunk *,STRPTR);

#define ID_FMNG MAKE_ID ('F','M','N','G')

#define MINLIMIT  2
#define MINLIMIT2 3
#define MINLIMIT3 4

#define RESERVED_PENS 4L

#define CPU_603  0x1000
#define CPU_603e 0x2000
#define CPU_604  0x4000
#define CPU_604e 0x8000
#define CPU_620  0x10000
#define CPU_G3   0x20000
#define CPU_G4   0x40000
#define CPU_G5   0x80000

#define MIN_LEVELUNDO 5L
#define MAX_LEVELUNDO 1000L
#define DEF_LEVELUNDO 50L

#define INIT_DEF_RMIN      -2.5f
#define INIT_DEF_RMAX      +1.5f
#define INIT_DEF_IMIN      -1.125f
#define INIT_DEF_IMAX      +1.125f
#define INIT_DEF_RMINSTR   "-2.5"
#define INIT_DEF_RMAXSTR   "+1.5"
#define INIT_DEF_IMINSTR   "-1.125"
#define INIT_DEF_IMAXSTR   "+1.125"
#define INIT_DEF_JKRE      -0.72f
#define INIT_DEF_JKIM      -0.26f
#define INIT_DEF_JKRESTR   "-0.72"
#define INIT_DEF_JKIMSTR   "-0.26"

//#define DEF_MONITOR      0x40D20003    // CGX support
//#define DEF_MONITORSTR  "0x40D20003"   // CGX support
//#define DEF_MONITOR      0x50041000      // UAE & P96 support 1280x720
//#define DEF_MONITORSTR  "0x50041000"     // UAE & P96 support 1280x720
#define DEF_MONITOR       ~0
#define DEF_MONITORSTR    "~0"

#define DEF_USERNAMESTR "Amiga rulez!"

#define DEF_FONTNAMESTR "topaz.font"
#define DEF_FONTSIZE 8
#define MAX_FONTSIZE 24
#define MIN_FONTSIZE 8

#define MARGIN 25 /* margin in pixels */
#define MIN_WIDTH 800
#define MAX_WIDTH 16368
#define DEF_WIDTH 1280

#define MIN_HEIGHT 600
#define MAX_HEIGHT 16384
#define DEF_HEIGHT 720

#define MIN_DEPTH 8
#define MID_DEPTH 16
#define MAX_DEPTH 24 // A8R8G8B8 Truecolor support
#define DEF_DEPTH 8

#define MIN_PRIORITY -5
#define MAX_PRIORITY 5
#define DEF_PRIORITY -3

#define MAX_FILELEN 30
#define MAX_DIRLEN  230
#define MAX_PATHLEN 260
#define BARLEN      120  /* more for reaction-support !! */

#define DEF_DELAY 5
#define MIN_DELAY 0
#define MAX_DELAY 200

#define TOBLACK   1
#define FROMBLACK 0

#define RAW_ESC     0X045
#define RAW_UPARROW 0X04C
#define RAW_DNARROW 0X04D
#define RAW_RHARROW 0X04E
#define RAW_LFARROW 0X04F
#define RAW_TAB     0X042
#define RAW_HELP    0X05F
#define RAW_1       0X001
#define RAW_2       0X002
#define RAW_3       0X003
#define RAW_4       0X004
#define RAW_5       0X005
#define RAW_6       0X006
#define RAW_7       0X007
#define RAW_8       0X008
#define RAW_9       0X009
#define RAW_0       0X00A
#define RAW_SPACE   0X040
#define RAW_ENTER   0X043
#define RAW_RETURN  0X044
#define RAW_c       0X011
#define RAW_i       0X017
#define RAW_o       0X018

#define VAN_ESC     0x01B

#define BLINKTIMES 5L

#define ONESEC (50L * 1L)
#define TWOSECS (50L * 2L) /* 2 secs */

#define SHIFTRIGHT FALSE
#define SHIFTLEFT TRUE

#define WINDOW_X_OFFSET 55
#define WINDOW_Y_OFFSET 65

#define INITIALZOOM  18

#define CLEAR_POINTER 0
#define BUSY_POINTER 1
#define ZOOM_POINTER 2

#define ZPW 15
#define ZPH 15
#define ZPXO -8
#define ZPYO -7

#define MAXCHARS 20

#define ACCEPT (1)
#define RATIO (2)
#define RESET (3)
#define CANCEL (4)
#define KEEP (5)

#define WMASK  0x1      // Window mask
#define FMASK  0x2
#define VMASK  0x4      // VisualInfo mask
#define SMASK  0x8      // Scren mask
#define BMASK  0x10     // Bitmap mask
#define ZMASK  0x20     // Brawborder mask
#define TMASK  0x40     // Title mask
#define MMASK  0x80     // Menu mask
#define LMASK  0x100    // PIXMEM mask
#define PMASK  0x200    // GFXMEM mask
#define UMASK  0x400
#define CMASK  0x800    // BRBitmap mask
#define RMASK  0x1000   // RNDMEM mask
#define DMASK  0x2000   // RGBMEM mask
#define AMASK  0x4000   // ARGBMEM mask

#define PROPERTYFLAGS (DIPF_IS_WB|DIPF_IS_RTG)
#define PROPERTYMASK (DIPF_IS_DUALPF|DIPF_IS_PF2PRI|DIPF_IS_HAM|DIPF_IS_EXTRAHALFBRITE|DIPF_IS_PAL)

#define LOADPICTURE_MSG 0x1
#define SAVEPICTURE_MSG 0x2
#define DUMP_MSG        0x4
#define REDRAW_MSG      0x8
#define UNDO_MSG        0x10
#define DRAW_MSG        0x20
#define PREVIEW_MSG     0x40
#define NEWDISPLAY_MSG  0x80
#define STOP_MSG        0x100
#define EXIT_MSG        0x200
#define COORDS_MSG      0x400
#define ITER_MSG        0x800
#define ABOUT_MSG       0x1000
#define TITLE_MSG       0x2000
#define TIME_MSG        0x4000
#define PALETTE_MSG     0x8000
#define CYCLERIGHT_MSG  0x10000
#define CYCLELEFT_MSG   0x20000
#define DELAY_MSG       0x40000
#define COLOR_MSG       0x80000
#define LOADPALETTE_MSG 0x100000
#define SAVEPALETTE_MSG 0x200000
#define FONTREQ_MSG     0x400000
#define SHOWGUIDE_MSG   0x800000
#define SYSINFO_MSG     0x1000000
#define AREXX_MSG       0x2000000  /* new for arexx-support */
#define ORBIT_MSG       0x4000000
#define PREC_MSG        0x8000000

#define PICTURES_DRAWER 0
#define PALETTES_DRAWER 1

#define IDCMP_STANDARD IDCMP_CLOSEWINDOW|IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_MENUPICK|IDCMP_EXTENDEDMOUSE
#define WFLG_STANDARD  WFLG_ACTIVATE|WFLG_BACKDROP|WFLG_GIMMEZEROZERO|WFLG_NOCAREREFRESH|WFLG_SMART_REFRESH|WFLG_BORDERLESS|WFLG_REPORTMOUSE|WFLG_NEWLOOKMENUS|WFLG_NW_EXTENDED
#define MODE_ID_MASK   (LACE|HIRES|HAM|EXTRA_HALFBRITE)

#define PRINTERDEVICE "printer.device"
#define FMCATALOGNAME "FlashMandelNG.catalog"

#define DATE __DATE__

#define HISTOGRAM_BIT (1L << 9)
#define TITLE_BIT     (1L << 10)
#define BRUTE_BIT     (1L << 11)
#define TILING_BIT    (1L << 12)
#define BOUNDARY_BIT  (1L << 13)
#define HIGHPREC_BIT  (1L << 14)
#define FOUR_BIT      (1L << 15)
#define LINEAR_BIT    (1L << 16)
#define REPEATED_BIT  (1L << 17)
#define LOG_BIT       (1L << 18)
#define SQUARE_BIT    (1L << 19)
#define ONE_BIT       (1L << 20)
#define TWO_BIT       (1L << 21)
#define THREE_BIT     (1L << 22)
#define FIXED_BIT     (1L << 23)
#define REAL_BIT      (1L << 24)
#define MC68K_BIT     (1L << 25)
#define PPC_BIT       (1L << 26)
#define JULIA4_BIT    (1L << 27)
#define JULIA_BIT     (1L << 28)
#define MANDEL_BIT    (1L << 29)
#define MANDEL4_BIT   (1L << 30)
#define TURBO_BIT     (1L << 31)

#endif /* FLASHMANDEL_H */
