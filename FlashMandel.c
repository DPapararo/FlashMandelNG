/******************************************************************************************************************
**  Coded by Dino Papararo
**
**  Modified for reaction, locale andarexx-support by Edgar Schwan, great work! 23-Jan-2002
**  Fixed a lot of bad string manipulation issues and added multilevel undo function by Dino Papararo 04-Sep-2003
**  Added support for MyMath-Library, removed fixedpoint-math - Edgar Schwan, great work again! 10.09.2003
**  Removed code related to a non AmigaOS4 Version and 68k cpus
**  Modified for AmigaOS4/GCC 02.10.2004, Edgar Schwan
**  MANDChunk is now apointer 26.02.2005, Edgar Schwan
**  Added real time preview when choosing Julia mode 14.11.2006, Dino Papararo
**  Added orbital plot function 21.03.2009, Dino Papararo
**  Bugfixes and a lot speed improvements, default dir now is "Progdir:" V1.3 19.02.2018 Dino Papararo
**  V1.4 Removed STORMC related code compiler defaults to GCC 26.may.2018 dino papararo
**  V1.5 new Tooltypes reading code
**           systeminfo requester fixed subst MEMF_FAST with MEMF_PUBLIC and MC68K_BIT with PPC_BIT flags
**           allocvec subst MEMF_FAST with MEMF_PRIVATE
**           fixed PikJuliaK windows preview 01.jun.2018 dino papararo
**  V1.6 used new OS4 specific AllocVec and AllocBitmap functions and flags
**       pedantic check for init/free memory and bitmap structures 03.06.2018 dpapararo
**  V1.7 fixed an arexx path issue - edgar schwan
**       fixed amigaguide path, now is possible open docs inside program - dpapararo 18-06-2018
**  V1.8 optim ized altivec calculations for speed, one instruction and two vars less - dpapararo 01-11-2018
**  V1.9 fixed Julia altivec function call - 03-02-2019 dpapararo
**  V2.0 now arexx menu is come back - 05-05-2019 edgar schwan
**  V2.1 cleanup all pre-AGA code and other fixes - 20-01-20 dpapararo
**  V2.2 very deep bugfix and cleanup code, now developed and tested under Pegasos II OS4.1fe - 12-03-20 dpapararo
**  V2.5 removed MyMath internal routines and added GMP Library multiprecision support - dpapararo
**       modified rendering recursion with some speedup - dpapararo
**  V2.6 Fixed Load and Save picures, removed all debug symbols - dpapararo
**  V2.7 Fixed screenmodes list, now only RTG modes are accepted.
**       Fixed PubScreenName now more than two instances of FlashMandel are allowed. 05-05-20 dpapararo
**  V2.8 Now Altivec version processes 4 pixels per time! 14-05-20 dpapararo
**  V2.9 Removed "compilerspecific.h" dependencies - small fixes
**  V3.0 Reimplemented reaction GUI (Edgar Schwan), added ITERATIONS ToolType  and removed various serious bugs
**       COLORMAPPING reworked, find ReadPixelColor bug and reverted to ReadPixel
**       now Mainwindow is GimmeZeroZero (Dino Papararo) 03-01-2021
**  V3.1 Implemented Histogram coloring algorithm
**       bugfixed RectangleDraw limits, Alivec main pixel vector, Chunkypixeks modulo and others minor issues
**       Cleaned code following C Best Practices, i.e. avoiding glodal vars and other stuff
**  V3.2 Implemented 24bit screens rendering 24-02-2021
**       fixed Histogram coloring algorithm
**       bugfixed recursive drawing functions 
**       many other fixes and changes 27-03-2021
**  V3.3 Implemented load and save pictures for 24bit screens
**  V3.4 Now load/save ilbm uses datatypes.library
**       Added a "*.fmng" file to save coordinates
**  V3.5 Fixed program startup and palette requester 
**  V3.6 Enabled GFX renderings also for 24bit screens
**  V3.7 Enabled middle mouse button for Zoom picture, rearranged all GetIMsg routines, 
**       fixed zoom frame on load error, now .fmng files are not listed in load picture 24-11-21 DPapararo
**  V3.8 Added mouse wheel frame zoom granularity control
**  V3.9 Bugfixed Recalculation and Undo function, other small fixes and enanchments
**       Enabled Java mode/IEEE for atvivec version 24-12-2021 DPapararo
**  V4.0 Removed Altivec Java/IEEE mode because useless for our purposes.
**		 Various bugs fixed, enanchements and code cleanups.
**  V4.1 Fixed Modulo in WritePixelArray and other small cleanups.
**
**	V4.2 Fixed Copy and Paste bitmap functions
**
**  V4.3 Introduced new tooltypes, reworked recursivity until 3x3 pixel blocks.
**		 Fixed Orbitwindow memalloc, small code cleanups.
**
**  V4.4 Added scrolling and zoon in/out feature with keyboard
**		
******************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#ifdef __ALTIVEC__
#include <altivec.h>
#endif

#include <exec/types.h>
#include <exec/exec.h>
#include <exec/exectags.h>
#include <intuition/gadgetclass.h>
#include <intuition/menuclass.h>
#include <graphics/scale.h>
#include <workbench/workbench.h>
#include <reaction/reaction_macros.h>
#include <utility/utility.h>
#include <libraries/asl.h>
#include <devices/printer.h>
#include <classes/window.h>
#include <classes/arexx.h>
#include <rexx/rxslib.h>
#include <rexx/errors.h>
#define NO_PROTOS
#include <iffp/ilbmapp.h>
#undef NO_PROTOS
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/window.h>
#include <proto/diskfont.h>
#include <proto/gadtools.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/asl.h>
#include <proto/wb.h>
#define __NOLIBBASE__
#include <proto/rexxsyslib.h>
#include <intuition/intuition.h>
#include <graphics/GfxBase.h>
#include <graphics/GfxMacros.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#undef  __NOLIBBASE__

#include <GMP/gmp.h>
// #include <GMP/mpfr.h>
// #include <GMP/mpf2mpfr.h>

#include "Headers/FlashMandel.h"
#include "Headers/FM_ReactionBasics.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "Headers/FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include "Headers/FM_InfoReq_React.h"

#ifdef FM_REACT_SUPPORT
#include "Headers/FM_ConfirmReq_React.h"
#include "Headers/FM_PalettePref_React.h"
#include "Headers/FM_InfoReq_React.h"
#include "Headers/FM_IntegerReq_React.h"
#include "Headers/FM_SysInfoReq_React.h"
#include "Headers/FM_CoordReq_React.h"
#endif /* FM_REACT_SUPPORT */

#ifdef FM_AREXX_SUPPORT
#include "Headers/FM_ARexx_React.h"
#include "Headers/FM_ARexx_Misc.h"
#endif /* FM_AREXX_SUPPORT */

#define VERSTAG "\0$VER: FlashMandelNG V4.4 (25.04.2022) Dino Papararo - Edgar Schwan"
#define GUIDE "SYS:Utilities/Multiview Docs/FlashMandelNG.guide"

/* PALETTE PEN COLORS FOR GUI PENS */
#define BLACK      (0)
#define WHITE      (1)
#define LIGHT_GREY (2)
#define DARK_GREY  (3)

// don't needed for auto mode -lmauto gcc switch
/*
struct Library *GadToolsBase;
struct GadToolsIFace *IGadTools;
struct Library *UtilityBase;
struct UtilityIFace *IUtility;
*/

struct Catalog *CatalogPtr = NULL;
struct UndoBuffer *UNDOBuffer = NULL;
#ifdef FM_AREXX_SUPPORT
struct Library *RexxSysBase = NULL;
#ifdef __amigaos4__
struct RexxSysIFace *IRexxSys = NULL;
#endif /* !__amigaos4__ */
extern Object *AREXXOBJ;
extern uint32 AREXXSIGNAL;
extern struct List AREXXEVENTLIST;
#endif /* FM_AREXX_SUPPORT */

STRPTR USED VER = (STRPTR) VERSTAG;
STRPTR USED CMD_Guide = (STRPTR) GUIDE;
STRPTR MyDir = NULL;
char FMSCREENNAME [MAXPUBSCREENNAME + 1] = "FlashMandel.1";
char HIGHPREC_STR[30] = {0};
char MYFILE[MAX_FILELEN], PICTURESDIR[MAX_DIRLEN], PALETTESDIR[MAX_DIRLEN], MYPATH[MAX_PATHLEN], BAR_STRING[BARLEN];
char USERNAME_STRING[MAX_FILELEN], COPYRIGHT_STRING[BARLEN], MYFONT[MAX_FILELEN], OLDFONTNAME_STR[MAX_FILELEN];
uint16 oldstatus = 0;
int32 res = NULL;
uint32 receivedsig, wsignal, lastsignal;
int8 allocsignal = 0;
int16 ForceAbort = FALSE;
int16 MX1 = 0, MY1 = 0, MX2 = 0, MY2 = 0, W = 0, H = 0;    /* changed for ARexx-support */
int16 __attribute__ ((aligned (16))) ZOOMLINE[5 * 2], RETURNVALUE = 0, UNDOCOUNTER = 0, LEVELUNDO;
int32 PRIORITY = DEF_PRIORITY, __oslibversion = Lib_Version;
uint32 MASK = TMASK, DELAY = DEF_DELAY, ELAPSEDTIME = NULL;
float64 DEF_RMIN, DEF_RMAX, DEF_IMIN, DEF_IMAX, DEF_JKRE, DEF_JKIM;
uint8 *PIXMEM = NULL, *GFXMEM = NULL, *ARGBMEM = NULL;
uint32 *PALETTE = NULL, *RNDMEM = NULL;
CPTR *VINFO = NULL;
// APTR LOCK = NULL;
CONST_STRPTR __attribute__ ((aligned (16))) CPUPPC_STR[50];
CONST_STRPTR __attribute__ ((aligned (16))) VERPPC_STR[50];
CONST_STRPTR __attribute__ ((aligned (16))) VECPPC_STR[50];
mpf_t gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2, gcre3,
      gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp, gdist, gmaxdist,
      gincremreal, gincremimag, gpzr, gpzi;

void (*C_POINT) (struct MandelChunk *, struct RastPort *, uint32 *, const int16, const int16);
void (*H_LINE) (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);
void (*V_LINE) (struct MandelChunk *, struct RastPort *, uint8 *, uint32 *, const int16, const int16, const int16);

uint32 LinearRemap (const float64, const float64, const float64, const float64, const float64);
uint32 LogRemap (const float64, const float64, const float64, const float64, const float64);
uint32 RepeatedRemap (const float64, const float64, const float64, const float64, const float64);
uint32 SquareRootRema (const float64, const float64, const float64, const float64, const float64);
uint32 OneRemap (const float64, const float64, const float64, const float64, const float64);
uint32 TwoRemap (const float64, const float64, const float64, const float64, const float64);
uint32 ThreeRemap (const float64, const float64, const float64, const float64, const float64);
uint32 FourRemap (const float64, const float64, const float64, const float64, const float64);

int16 __attribute__ ((saveds)) SMFilterFunc (REG (a0, struct Hook *), REG (a2, struct ScreenModeRequester *), REG (a1, uint32));

struct LoadSaveFMChunk *LSFMChunk = NULL;
struct MandelChunk *MANDChunk = NULL;

struct Hook SMFILTERHOOK = { NULL, NULL, (void *) SMFilterFunc, NULL };
struct Border MYBORDER = { 0, 0, 0, 0, COMPLEMENT, 5, &ZOOMLINE, 0 };
struct TextAttr MYFONTSTRUCT = { DEF_FONTNAMESTR, DEF_FONTSIZE, FS_NORMAL, FPF_DISKFONT|FPF_DESIGNED };
struct NewGadget TEXTGAD = { 0, 0, 0, 0, 0, &MYFONTSTRUCT, 0, 0, 0, 0 };
struct NewGadget BUTTONGAD = { 0, 0, 0, 0, 0, &MYFONTSTRUCT, 0, 0, 0, 0 };
struct NewGadget CHECKBOXGAD = { 0, 0, 0, 0, 0, &MYFONTSTRUCT, 0, 0, 0, 0 };
struct BitScaleArgs BSA = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, 0, 0, NULL, NULL };
struct Chunk FLASHMANDEL_CHUNK = { NULL, ID_ILBM, ID_FMNG, sizeof (struct LoadSaveFMChunk), &LSFMChunk };
struct Chunk COPYRIGHT_CHUNK = { NULL, ID_ILBM, ID_Copyright, sizeof (COPYRIGHT_STRING), COPYRIGHT_STRING };
struct Chunk USERNAME_CHUNK = { NULL, ID_ILBM, ID_AUTH, sizeof (USERNAME_STRING), USERNAME_STRING };

/* PALETTE 8BIT DEFAULT TABLE */
uint32 __attribute__ ((aligned (16))) PALETTE256[] = {
       	256 << 16 + 0,
       	0X00000000, 0X00000000, 0X00000000, 0XFFFFFFFF, 0XFFFFFFFF,
       	0XFFFFFFFF, 0XAAAAAAAA, 0XAAAAAAAA, 0XAAAAAAAA, 0X66666666,
       	0X66666666, 0X66666666, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
       	0XE9E9E9E9, 0XEBEBEBEB, 0XEFEFEFEF, 0XD4D4D4D4, 0XD7D7D7D7,
       	0XDFDFDFDF, 0XBFBFBFBF, 0XC3C3C3C3, 0XCFCFCFCF, 0XAAAAAAAA,
       	0XAFAFAFAF, 0XBFBFBFBF, 0X95959595, 0X9B9B9B9B, 0XAFAFAFAF,
       	0X80808080, 0X87878787, 0X9F9F9F9F, 0X6B6B6B6B, 0X73737373,
       	0X8F8F8F8F, 0X56565656, 0X5F5F5F5F, 0X7F7F7F7F, 0X41414141,
       	0X4B4B4B4B, 0X6F6F6F6F, 0X2C2C2C2C, 0X37373737, 0X5F5F5F5F,
       	0X17171717, 0X23232323, 0X4F4F4F4F, 0X02020202, 0X0F0F0F0F,
       	0X3F3F3F3F, 0X02020202, 0X16161616, 0X4B4B4B4B, 0X02020202,
       	0X1D1D1D1D, 0X57575757, 0X02020202, 0X24242424, 0X63636363,
       	0X02020202, 0X2B2B2B2B, 0X6F6F6F6F, 0X02020202, 0X32323232,
       	0X7B7B7B7B, 0X02020202, 0X39393939, 0X87878787, 0X02020202,
       	0X40404040, 0X93939393, 0X02020202, 0X47474747, 0X9F9F9F9F,
        0X02020202, 0X4E4E4E4E, 0XABABABAB, 0X02020202, 0X55555555,
        0XB7B7B7B7, 0X02020202, 0X5C5C5C5C, 0XC3C3C3C3, 0X02020202,
        0X63636363, 0XCFCFCFCF, 0X02020202, 0X6A6A6A6A, 0XDBDBDBDB,
        0X02020202, 0X71717171, 0XE7E7E7E7, 0X02020202, 0X78787878,
        0XF3F3F3F3, 0X02020202, 0X7F7F7F7F, 0XFFFFFFFF, 0X11111111,
        0X7F7F7F7F, 0XF0F0F0F0, 0X21212121, 0X7F7F7F7F, 0XE0E0E0E0,
        0X31313131, 0X7F7F7F7F, 0XD0D0D0D0, 0X41414141, 0X7F7F7F7F,
        0XC0C0C0C0, 0X51515151, 0X7F7F7F7F, 0XB0B0B0B0, 0X61616161,
        0X7F7F7F7F, 0XA0A0A0A0, 0X71717171, 0X7F7F7F7F, 0X90909090,
        0X81818181, 0X7F7F7F7F, 0X80808080, 0X90909090, 0X7F7F7F7F,
        0X70707070, 0XA0A0A0A0, 0X7F7F7F7F, 0X60606060, 0XB0B0B0B0,
        0X7F7F7F7F, 0X50505050, 0XC0C0C0C0, 0X7F7F7F7F, 0X40404040,
        0XD0D0D0D0, 0X7F7F7F7F, 0X30303030, 0XE0E0E0E0, 0X7F7F7F7F,
        0X20202020, 0XF0F0F0F0, 0X7F7F7F7F, 0X10101010, 0XFFFFFFFF,
        0X7F7F7F7F, 0X00000000, 0XF8F8F8F8, 0X7B7B7B7B, 0X00000000,
        0XF0F0F0F0, 0X78787878, 0X00000000, 0XE9E9E9E9, 0X74747474,
        0X00000000, 0XE1E1E1E1, 0X71717171, 0X00000000, 0XDADADADA,
        0X6D6D6D6D, 0X00000000, 0XD2D2D2D2, 0X6A6A6A6A, 0X00000000,
        0XCBCBCBCB, 0X66666666, 0X00000000, 0XC3C3C3C3, 0X63636363,
        0X00000000, 0XBCBCBCBC, 0X5F5F5F5F, 0X00000000, 0XB4B4B4B4,
        0X5C5C5C5C, 0X00000000, 0XADADADAD, 0X58585858, 0X00000000,
        0XA5A5A5A5, 0X55555555, 0X00000000, 0X9E9E9E9E, 0X51515151,
        0X00000000, 0X96969696, 0X4E4E4E4E, 0X00000000, 0X8F8F8F8F,
        0X4A4A4A4A, 0X00000000, 0X87878787, 0X47474747, 0X00000000,
        0X7F7F7F7F, 0X43434343, 0X00000000, 0X78787878, 0X40404040,
        0X00000000, 0X70707070, 0X3C3C3C3C, 0X00000000, 0X69696969,
        0X39393939, 0X00000000, 0X61616161, 0X35353535, 0X00000000,
        0X5A5A5A5A, 0X32323232, 0X00000000, 0X52525252, 0X2E2E2E2E,
        0X00000000, 0X4B4B4B4B, 0X2B2B2B2B, 0X00000000, 0X43434343,
        0X27272727, 0X00000000, 0X3C3C3C3C, 0X24242424, 0X00000000,
        0X34343434, 0X20202020, 0X00000000, 0X2D2D2D2D, 0X1D1D1D1D,
        0X00000000, 0X25252525, 0X19191919, 0X00000000, 0X1E1E1E1E,
        0X16161616, 0X00000000, 0X16161616, 0X12121212, 0X00000000,
        0X0F0F0F0F, 0X0F0F0F0F, 0X00000000, 0X1E1E1E1E, 0X1E1E1E1E,
        0X00000000, 0X2D2D2D2D, 0X2D2D2D2D, 0X00000000, 0X3C3C3C3C,
        0X3C3C3C3C, 0X00000000, 0X4B4B4B4B, 0X4B4B4B4B, 0X00000000,
        0X5A5A5A5A, 0X5A5A5A5A, 0X00000000, 0X69696969, 0X69696969,
        0X00000000, 0X78787878, 0X78787878, 0X00000000, 0X87878787,
        0X87878787, 0X00000000, 0X96969696, 0X96969696, 0X00000000,
        0XA5A5A5A5, 0XA5A5A5A5, 0X00000000, 0XB4B4B4B4, 0XB4B4B4B4,
        0X00000000, 0XC3C3C3C3, 0XC3C3C3C3, 0X00000000, 0XD2D2D2D2,
        0XD2D2D2D2, 0X00000000, 0XE1E1E1E1, 0XE1E1E1E1, 0X00000000,
        0XF0F0F0F0, 0XF0F0F0F0, 0X00000000, 0XFFFFFFFF, 0XFFFFFFFF,
        0X00000000, 0XEFEFEFEF, 0XFFFFFFFF, 0X00000000, 0XDFDFDFDF,
        0XFFFFFFFF, 0X00000000, 0XCFCFCFCF, 0XFFFFFFFF, 0X00000000,
        0XBFBFBFBF, 0XFFFFFFFF, 0X00000000, 0XAFAFAFAF, 0XFFFFFFFF,
        0X00000000, 0X9F9F9F9F, 0XFFFFFFFF, 0X00000000, 0X8F8F8F8F,
        0XFFFFFFFF, 0X00000000, 0X7F7F7F7F, 0XFFFFFFFF, 0X00000000,
        0X6F6F6F6F, 0XFFFFFFFF, 0X00000000, 0X5F5F5F5F, 0XFFFFFFFF,
        0X00000000, 0X4F4F4F4F, 0XFFFFFFFF, 0X00000000, 0X3F3F3F3F,
        0XFFFFFFFF, 0X00000000, 0X2F2F2F2F, 0XFFFFFFFF, 0X00000000,
        0X1F1F1F1F, 0XFFFFFFFF, 0X00000000, 0X0F0F0F0F, 0XFFFFFFFF,
        0X00000000, 0X00000000, 0XFFFFFFFF, 0X00000000, 0X00000000,
        0XF7F7F7F7, 0X00000000, 0X00000000, 0XF0F0F0F0, 0X00000000,
        0X00000000, 0XE8E8E8E8, 0X00000000, 0X00000000, 0XE1E1E1E1,
        0X00000000, 0X00000000, 0XD9D9D9D9, 0X00000000, 0X00000000,
        0XD2D2D2D2, 0X00000000, 0X00000000, 0XCACACACA, 0X00000000,
        0X00000000, 0XC3C3C3C3, 0X00000000, 0X00000000, 0XBBBBBBBB,
        0X00000000, 0X00000000, 0XB4B4B4B4, 0X00000000, 0X00000000,
        0XACACACAC, 0X00000000, 0X00000000, 0XA5A5A5A5, 0X00000000,
        0X00000000, 0X9D9D9D9D, 0X00000000, 0X00000000, 0X96969696,
        0X00000000, 0X00000000, 0X8E8E8E8E, 0X00000000, 0X00000000,
        0X87878787, 0X00000000, 0X00000000, 0X7F7F7F7F, 0X00000000,
        0X00000000, 0X78787878, 0X00000000, 0X00000000, 0X70707070,
        0X00000000, 0X00000000, 0X69696969, 0X00000000, 0X00000000,
        0X61616161, 0X00000000, 0X00000000, 0X5A5A5A5A, 0X00000000,
        0X00000000, 0X52525252, 0X00000000, 0X00000000, 0X4B4B4B4B,
        0X00000000, 0X00000000, 0X43434343, 0X00000000, 0X00000000,
        0X3C3C3C3C, 0X00000000, 0X00000000, 0X34343434, 0X00000000,
        0X00000000, 0X2D2D2D2D, 0X00000000, 0X00000000, 0X25252525,
        0X00000000, 0X00000000, 0X1E1E1E1E, 0X00000000, 0X00000000,
        0X16161616, 0X00000000, 0X00000000, 0X0F0F0F0F, 0X00000000,
        0X07070707, 0X0E0E0E0E, 0X07070707, 0X0F0F0F0F, 0X0E0E0E0E,
        0X0F0F0F0F, 0X17171717, 0X0D0D0D0D, 0X17171717, 0X1F1F1F1F,
        0X0D0D0D0D, 0X1F1F1F1F, 0X26262626, 0X0C0C0C0C, 0X26262626,
        0X2E2E2E2E, 0X0C0C0C0C, 0X2E2E2E2E, 0X36363636, 0X0B0B0B0B,
        0X36363636, 0X3E3E3E3E, 0X0B0B0B0B, 0X3E3E3E3E, 0X45454545,
        0X0A0A0A0A, 0X45454545, 0X4D4D4D4D, 0X0A0A0A0A, 0X4D4D4D4D,
        0X55555555, 0X0A0A0A0A, 0X55555555, 0X5D5D5D5D, 0X09090909,
        0X5D5D5D5D, 0X64646464, 0X09090909, 0X64646464, 0X6C6C6C6C,
        0X08080808, 0X6C6C6C6C, 0X74747474, 0X08080808, 0X74747474,
        0X7C7C7C7C, 0X07070707, 0X7C7C7C7C, 0X83838383, 0X07070707,
        0X83838383, 0X8B8B8B8B, 0X06060606, 0X8B8B8B8B, 0X93939393,
        0X06060606, 0X93939393, 0X9B9B9B9B, 0X05050505, 0X9B9B9B9B,
        0XA2A2A2A2, 0X05050505, 0XA2A2A2A2, 0XAAAAAAAA, 0X05050505,
        0XAAAAAAAA, 0XB2B2B2B2, 0X04040404, 0XB2B2B2B2, 0XBABABABA,
        0X04040404, 0XBABABABA, 0XC1C1C1C1, 0X03030303, 0XC1C1C1C1,
        0XC9C9C9C9, 0X03030303, 0XC9C9C9C9, 0XD1D1D1D1, 0X02020202,
        0XD1D1D1D1, 0XD9D9D9D9, 0X02020202, 0XD9D9D9D9, 0XE0E0E0E0,
        0X01010101, 0XE0E0E0E0, 0XE8E8E8E8, 0X01010101, 0XE8E8E8E8,
        0XF0F0F0F0, 0X00000000, 0XF0F0F0F0, 0XF8F8F8F8, 0X00000000,
        0XF8F8F8F8, 0XFFFFFFFF, 0X00000000, 0XFFFFFFFF, 0XFFFFFFFF,
        0X00000000, 0XEFEFEFEF, 0XFFFFFFFF, 0X00000000, 0XDFDFDFDF,
        0XFFFFFFFF, 0X00000000, 0XCFCFCFCF, 0XFFFFFFFF, 0X00000000,
        0XBFBFBFBF, 0XFFFFFFFF, 0X00000000, 0XAFAFAFAF, 0XFFFFFFFF,
        0X00000000, 0X9F9F9F9F, 0XFFFFFFFF, 0X00000000, 0X8F8F8F8F,
        0XFFFFFFFF, 0X00000000, 0X7F7F7F7F, 0XFFFFFFFF, 0X00000000,
        0X6F6F6F6F, 0XFFFFFFFF, 0X00000000, 0X5F5F5F5F, 0XFFFFFFFF,
        0X00000000, 0X4F4F4F4F, 0XFFFFFFFF, 0X00000000, 0X3F3F3F3F,
        0XFFFFFFFF, 0X00000000, 0X2F2F2F2F, 0XFFFFFFFF, 0X00000000,
        0X1F1F1F1F, 0XFFFFFFFF, 0X00000000, 0X0F0F0F0F, 0XFFFFFFFF,
        0X00000000, 0X00000000, 0XEFEFEFEF, 0X00000000, 0X00000000,
        0XDFDFDFDF, 0X00000000, 0X00000000, 0XCFCFCFCF, 0X00000000,
        0X00000000, 0XBFBFBFBF, 0X00000000, 0X00000000, 0XAFAFAFAF,
        0X00000000, 0X00000000, 0X9F9F9F9F, 0X00000000, 0X00000000,
        0X8F8F8F8F, 0X00000000, 0X00000000, 0X7F7F7F7F, 0X00000000,
        0X00000000, 0X6F6F6F6F, 0X00000000, 0X00000000, 0X5F5F5F5F,
        0X00000000, 0X00000000, 0X4F4F4F4F, 0X00000000, 0X00000000,
        0X3F3F3F3F, 0X00000000, 0X00000000, 0X2F2F2F2F, 0X00000000,
        0X00000000, 0X1F1F1F1F, 0X00000000, 0X00000000, 0X0F0F0F0F,
        0X00000000, 0X00000000, 0X00000000, 0X0F0F0F0F, 0X0F0F0F0F,
        0X00000000, 0X1F1F1F1F, 0X1F1F1F1F, 0X00000000, 0X2F2F2F2F,
        0X2F2F2F2F, 0X00000000, 0X3F3F3F3F, 0X3F3F3F3F, 0X00000000,
        0X4F4F4F4F, 0X4F4F4F4F, 0X00000000, 0X5F5F5F5F, 0X5F5F5F5F,
        0X00000000, 0X6F6F6F6F, 0X6F6F6F6F, 0X00000000, 0X7F7F7F7F,
        0X7F7F7F7F, 0X00000000, 0X8F8F8F8F, 0X8F8F8F8F, 0X00000000,
        0X9F9F9F9F, 0X9F9F9F9F, 0X00000000, 0XAFAFAFAF, 0XAFAFAFAF,
        0X00000000, 0XBFBFBFBF, 0XBFBFBFBF, 0X00000000, 0XCFCFCFCF,
        0XCFCFCFCF, 0X00000000, 0XDFDFDFDF, 0XDFDFDFDF, 0X00000000,
        0XEFEFEFEF, 0XEFEFEFEF, 0X00000000, 0XFFFFFFFF, 0XFFFFFFFF,
        0X00000000, 0XEFEFEFEF, 0XFFFFFFFF, 0X00000000, 0XDFDFDFDF,
        0XFFFFFFFF, 0X00000000, 0XCFCFCFCF, 0XFFFFFFFF, 0X00000000,
        0XBFBFBFBF, 0XFFFFFFFF, 0X00000000, 0XAFAFAFAF, 0XFFFFFFFF,
        0X00000000, 0X9F9F9F9F, 0XFFFFFFFF, 0X00000000, 0X8F8F8F8F,
        0XFFFFFFFF, 0X00000000, 0X7F7F7F7F, 0XFFFFFFFF, 0X00000000,
        0X6F6F6F6F, 0XFFFFFFFF, 0X00000000, 0X5F5F5F5F, 0XFFFFFFFF,
        0X00000000, 0X4F4F4F4F, 0XFFFFFFFF, 0X00000000, 0X3F3F3F3F,
        0XFFFFFFFF, 0X00000000, 0X2F2F2F2F, 0XFFFFFFFF, 0X00000000,
        0X1F1F1F1F, 0XFFFFFFFF, 0X00000000, 0X0F0F0F0F, 0XFFFFFFFF,
        0X00000000, 0X00000000, 0XFFFFFFFF, 0X00000000, 0X00000000,
        0XEFEFEFEF, 0X00000000, 0X00000000, 0XDFDFDFDF, 0X00000000,
        0X00000000, 0XCFCFCFCF, 0X00000000, 0X00000000, 0XBFBFBFBF,
        0X00000000, 0X00000000, 0XAFAFAFAF, 0X00000000, 0X00000000,
        0X9F9F9F9F, 0X00000000, 0X00000000, 0X8F8F8F8F, 0X00000000,
       	0X00000000, 0X7F7F7F7F, 0X00000000, 0X00000000, 0X6F6F6F6F,
       	0X00000000, 0X00000000, 0X5F5F5F5F, 0X00000000, 0X00000000,
       	0X4F4F4F4F, 0X00000000, 0X00000000, 0X3F3F3F3F, 0X00000000,
       	0X00000000, 0X2F2F2F2F, 0X00000000, 0X00000000, 0X1F1F1F1F,
       	0X00000000, 0X00000000, 0X0F0F0F0F, NULL
     };

// ZOOM POINTER SPRITE
uint16 __attribute__ ((aligned (16))) ZOOMPOINTER[] = {
       	0x0000, 0x0000,     /* reserved, must be NULL */
       	0x0100, 0x0000, 0x0100, 0x0000, 0x0000, 0x0100, 0x0000, 0x0100, 
       	0x0100, 0x0100, 0x0100, 0x0100, 0x0000, 0x0000, 0xCC66, 0x3C78, 
       	0x0000, 0x0000, 0x0100, 0x0100, 0x0100, 0x0100, 0x0000, 0x0100, 
       	0x0000, 0x0100, 0x0100, 0x0000, 0x0100, 0x0000, 0x0000, 0x0000,
       	0x0000, 0x0000      /* reserved, must be NULL */ };

/* uint16 __attribute__ ((aligned (16))) WaitPointer[] = {
       	0x0000, 0x0000,     // reserved, must be NULL
       	0x0400, 0x07C0, 0x0000, 0x07C0, 0x0100, 0x0380, 0x0000, 0x07E0,
       	0x07C0, 0x1FF8, 0x1FF0, 0x3FEC, 0x3FF8, 0x7FDE, 0x3FF8, 0x7FBE,
       	0x7FFC, 0xFF7F, 0x7EFC, 0xFFFF, 0x7FFC, 0xFFFF, 0x3FF8, 0x7FFE,
       	0x3FF8, 0x7FFE, 0x1FF0, 0x3FFC, 0x07C0, 0x1FF8, 0x0000, 0x07E0,
       	0x0000, 0x0000      // reserved, must be NULL}; */

uint16 __attribute__ ((aligned (16))) PENS[] = { WHITE, BLACK, WHITE, LIGHT_GREY, BLACK, LIGHT_GREY, WHITE, DARK_GREY, WHITE, WHITE, DARK_GREY, BLACK, (uint16) ~ 0 };

/* PEN COLORS DESCRIPTIONS
01 WHITE      DETAILPEN         testo (v34)
02 BLACK      BLOCKPEN          sfondo (v34)
03 WHITE      TEXTPEN           text on background
04 LIGHT_GREY SHINEPEN          bright edge on 3d objects
05 BLACK      SHADOWPEN         dark edge on 3d objects
06 LIGHT_GREY FILLPEN           active-window/selected-gadget fill
07 WHITE      FILLTEXTPEN       text over FILLPEN
08 DARK_GREY  BACKGROUNDPEN     always color 0
09 WHITE      HIGHLIGHTTEXTPEN  special color text, on background
10 WHITE      BARDETAILPEN      text/detail in screen-bar/menus
11 DARK_GREY  BARBLOCKPEN       screen-bar/menus fill
12 BLACK      BARTRIMPEN        trim under screenbar
*/

struct NewMenu FLASHMANDELMENU[] = {
		NM_TITLE, NULL, 0, 0, 0, (APTR) TXT_Project,
 	    NM_ITEM, NULL, "A", NM_ITEMDISABLED, 0, (APTR) TXT_About,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "N", NM_ITEMDISABLED, 0, (APTR) TXT_SystemInfo,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "H", NM_ITEMDISABLED, 0, (APTR) TXT_Help,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "L", NM_ITEMDISABLED, 0, (APTR) TXT_LoadPicture,
       	NM_ITEM, NULL, "S", NM_ITEMDISABLED, 0, (APTR) TXT_SavePicture,
       	NM_ITEM, NULL, "Y", NM_ITEMDISABLED, 0, (APTR) TXT_LoadPalette,
       	NM_ITEM, NULL, "E", NM_ITEMDISABLED, 0, (APTR) TXT_SavePalette,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "D", NM_ITEMDISABLED, 0, (APTR) TXT_Print,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "Q", NM_ITEMDISABLED, 0, (APTR) TXT_Quit,
		/***************************************************************************************/
       	NM_TITLE, NULL, 0, 0, 0, (APTR) TXT_Options,
       	NM_ITEM, NULL, 0, NM_ITEMDISABLED, 0, (APTR) TXT_Title,
       	NM_SUB, NULL, "O", CHECKIT | MENUTOGGLE | CHECKED, 0,
       	(APTR) TXT_TitleBar,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
       	NM_SUB, NULL, "T", 0, 0, (APTR) TXT_LastTime,
       	NM_ITEM, NULL, "C", NM_ITEMDISABLED, 0, (APTR) TXT_Limits,
       	NM_ITEM, NULL, 0, NM_ITEMDISABLED, 0, (APTR) TXT_Iterations,
       	NM_SUB, "256", 0, CHECKIT, ~(1 << 0), 0,
       	NM_SUB, "320", 0, CHECKIT | CHECKED, ~(1 << 1), 0,
       	NM_SUB, "512", 0, CHECKIT, ~(1 << 2), 0,
       	NM_SUB, "1024", 0, CHECKIT, ~(1 << 3), 0,
       	NM_SUB, "2048", 0, CHECKIT, ~(1 << 4), 0,
       	NM_SUB, "4096", 0, CHECKIT, ~(1 << 5), 0,
       	NM_SUB, "8192", 0, CHECKIT, ~(1 << 6), 0,
       	NM_SUB, "16384", 0, CHECKIT, ~(1 << 7), 0,
       	NM_SUB, "32768", 0, CHECKIT, ~(1 << 8), 0,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
       	NM_SUB, NULL, "I", CHECKIT, ~(1 << 10), (APTR) TXT_Custom,
       	NM_ITEM, NULL, 0, NM_ITEMDISABLED, 0, (APTR) TXT_ZPower,
       	NM_SUB, "2?", 0, CHECKIT, ~(1 << 0), 0,
       	NM_SUB, "4?", 0, CHECKIT, ~(1 << 1), 0,
       	NM_SUB, "8?", 0, CHECKIT, ~(1 << 2), 0,
       	NM_SUB, "16?", 0, CHECKIT, ~(1 << 3), 0,
       	NM_SUB, "32?", 0, CHECKIT, ~(1 << 4), 0,
       	NM_SUB, "64?", 0, CHECKIT, ~(1 << 5), 0,
       	NM_SUB, "128?", 0, CHECKIT, ~(1 << 6), 0,
       	NM_SUB, "256?", 0, CHECKIT, ~(1 << 7), 0,
       	NM_SUB, "512?", 0, CHECKIT, ~(1 << 8), 0,
       	NM_SUB, "1024?", 0, CHECKIT, ~(1 << 9), 0,
       	NM_SUB, "2048?", 0, CHECKIT, ~(1 << 10), 0,
       	NM_ITEM, NULL, 0, 0, 0, (APTR) TXT_Priority,
       	NM_SUB, "-5", "%", CHECKIT, ~(1 << 0), 0,
       	NM_SUB, "-4", "$", CHECKIT, ~(1 << 1), 0,
       	NM_SUB, "-3", "?", CHECKIT, ~(1 << 2), 0,
       	NM_SUB, "-2", "\"", CHECKIT, ~(1 << 3), 0,
       	NM_SUB, "-1", "!", CHECKIT, ~(1 << 4), 0,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
       	NM_SUB, " 0", "0", CHECKIT, ~(1 << 6), 0,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
       	NM_SUB, "+1", "1", CHECKIT, ~(1 << 8), 0,
       	NM_SUB, "+2", "2", CHECKIT, ~(1 << 9), 0,
       	NM_SUB, "+3", "3", CHECKIT, ~(1 << 10), 0,
       	NM_SUB, "+4", "4", CHECKIT, ~(1 << 11), 0,
       	NM_SUB, "+5", "5", CHECKIT, ~(1 << 12), 0,
       	NM_ITEM, NULL, 0, 0, 0, (APTR) TXT_ColorRemap,
       	NM_SUB, NULL, "6", CHECKIT, ~(1 << 0), (APTR) TXT_Linear,
       	NM_SUB, "Log (x)", "7", CHECKIT, ~(1 << 1), 0,
       	NM_SUB, NULL, "8", CHECKIT, ~(1 << 2), (APTR) TXT_Repeated,
       	NM_SUB, "Sqrt (x)", "9", CHECKIT, ~(1 << 3), 0,
       	NM_SUB, "x?", "&", CHECKIT, ~(1 << 4), 0,
       	NM_SUB, "Sqrt (x?)", "/", CHECKIT, ~(1 << 5), 0,
       	NM_SUB, "Sqrt(Log(x?))", "(", CHECKIT, ~(1 << 6), 0,
       	NM_SUB, "GUI", ")", CHECKIT, ~(1 << 7), 0,
       	NM_ITEM, NULL, 0, 0, 0, (APTR) TXT_FractalType,
       	NM_SUB, NULL, "J", CHECKIT, ~(1 << 0), (APTR) TXT_Julia,
       	NM_SUB, NULL, "M", CHECKIT, ~(1 << 1), (APTR) TXT_Mandelbrot,
       	NM_ITEM, NULL, 0, NM_ITEMDISABLED, 0, (APTR) TXT_RenderingMode,
       	NM_SUB, "Turbo", "+", CHECKIT | MENUTOGGLE, 0, 0,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
      	NM_SUB, "Histogram", "G", CHECKIT | MENUTOGGLE, 0, 0, // 1,7,2
       	NM_ITEM, NULL, 0, 0, 0, (APTR) TXT_MathMode,
       	NM_SUB, NULL, 0, CHECKIT | CHECKED, ~(1 << 0),
       	(APTR) TXT_MathMode_Fast,
       	NM_SUB, NULL, 0, CHECKIT, ~(1 << 1), (APTR) TXT_MathMode_HighPrec,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
       	NM_SUB, NULL, 0, 0, 0, (APTR) TXT_Math_ChangePrec,
		/***************************************************************************************/
       	NM_TITLE, NULL, 0, 0, 0, (APTR) TXT_Calculate,
       	NM_ITEM, NULL, "W", NM_ITEMDISABLED, 0, (APTR) TXT_Preview,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "R", NM_ITEMDISABLED, 0, (APTR) TXT_Recalculate,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "U", NM_ITEMDISABLED, 0, (APTR) TXT_Undo,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "Z", NM_ITEMDISABLED, 0, (APTR) TXT_Zoom,
       	NM_ITEM, NULL, "X", 0, 0, (APTR) TXT_Stop,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "B", 0, 0, (APTR) TXT_Orbit,
		/***************************************************************************************/
       	NM_TITLE, NULL, 0, 0, 0, (APTR) TXT_Video,
       	NM_ITEM, NULL, 0, NM_ITEMDISABLED, 0, (APTR) TXT_Cycle,
       	NM_SUB, NULL, ">", 0, 0, (APTR) TXT_Forward,
       	NM_SUB, NULL, "<", 0, 0, (APTR) TXT_Backward,
       	NM_SUB, NM_BARLABEL, 0, 0, 0, 0,
       	NM_SUB, NULL, ".", 0, 0, (APTR) TXT_Delay,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "P", NM_ITEMDISABLED, 0, (APTR) TXT_Palette,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "V", NM_ITEMDISABLED, 0, (APTR) TXT_ScreenMode,
       	NM_ITEM, NM_BARLABEL, 0, 0, 0, 0,
       	NM_ITEM, NULL, "F", NM_ITEMDISABLED, 0, (APTR) TXT_FontSettings,
		/***************************************************************************************/
#ifdef 	FM_AREXX_SUPPORT
		/*	NM_TITLE,"ARexx",0,0,0,0,
       	NM_ITEM,"Launch...",0,0,0,0, */
#endif /* FM_AREXX_SUPPORT */
       	NM_END, 0, 0, 0, 0, 0
     	};

struct Menu *MAINMENU = NULL;
struct BitMap *MYBITMAP = NULL;
struct TextFont *NEWFONT = NULL;
struct ILBMInfo MYILBM = { 0 };

int32 __attribute__ ((aligned (16))) IlbmProps[] = { ID_ILBM, ID_BMHD, ID_ILBM, ID_CMAP, ID_ILBM, ID_CAMG, ID_ILBM, ID_FMNG, ID_ILBM, ID_AUTH, ID_ILBM, ID_Copyright, TAG_DONE };
int32 __attribute__ ((aligned (16))) IlbmCollects[] = { TAG_DONE };
int32 __attribute__ ((aligned (16))) IlbmStops[] = { ID_ILBM, ID_BODY, TAG_DONE };

// #ifdef USE_ALTIVEC_MATH
uint32 __attribute__ ((aligned (32))) PIXELVECTOR[4] = {0L, 0L, 0L, 0L}; /* Pixels colors vector */
// #endif

/* DisplayError() */
uint32 DisplayError (struct Window *Win, int32 id, uint32 ErrorLevel)
{
  struct EasyStruct es = { 	sizeof (struct EasyStruct), /* es_StructSize */
    						0, /* es_Flags */
    						CATSTR (TXT_FMErrorTitle),	/* es_Title */
    						"%s", /* es_TextFormat */
    						CATSTR (TXT_OK), /* es_GadgetFormat */ };
						
  uint32 idcmp = IDCMP_RAWKEY;

	if (Win) EasyRequest (Win, &es, &idcmp, CATSTR (id));
	else Printf ("FlashMandel error: %s\n", CATSTR (id));
	
  	RETURNVALUE = ErrorLevel;
  	return (ErrorLevel);
}

/* GetDefStr() */
STRPTR GetDefStr (int32 id)
{
  STRPTR string = NULL;
  struct FM_CatCompArrayType *array = (struct FM_CatCompArrayType *) &FM_CatCompArray;

	if (LocaleBase)
    {
      	while (array->cca_ID != LAST_CAT_ENTRY)
		{
	  		if (array->cca_ID == id)
	    	{
	      		string = array->cca_Str;
	      		break;
	    	}
	  
	  		array++;			
		} 
    }

  	return (string);
}

/* LocalizeMenu() */
void LocalizeMenu (struct NewMenu *nm)
{
  struct NewMenu *item;
  STRPTR str = NULL;

  	if (! LocaleBase) return;

  	for (item = nm; item->nm_Type != NM_END; item++)
    {
      	if (item->nm_UserData)
		{
	  		str = NULL;
	  		
			if ((uint32) item->nm_UserData == (uint32) TXT_MathMode_HighPrec)
	    	{
	      		const STRPTR format = (CATSTR ((uint32) item->nm_UserData));
	      		SNPrintf (HIGHPREC_STR, sizeof (HIGHPREC_STR), format, MANDChunk->PrecisionBits);
	      		str = HIGHPREC_STR;
	    	}
	  
	  		else
	    	{
	      		str = CATSTR ((uint32) item->nm_UserData);
	    	}
	  
	  		item->nm_Label = (STRPTR) str;
		}
    }
}

/* UpdatePrecMenuItem() */
void UpdatePrecMenuItem (int16 p, struct Menu *menu, struct ILBMInfo *ilbm)
{
  	if (! LocaleBase) return;

  	struct MenuItem *item = ItemAddress (menu, (uint32) FULLMENUNUM (1, 8, 1));
  	const STRPTR format = CATSTR ((uint32) GTMENUITEM_USERDATA (item));
  	
	ModifyIDCMP (ilbm->win, NULL);
  	ClearMenuStrip (ilbm->win);
  	SNPrintf (HIGHPREC_STR, sizeof (HIGHPREC_STR), format, p);
  	ResetMenuStrip (ilbm->win, menu);
  	ModifyIDCMP (ilbm->win, IDCMP_STANDARD);
}

/* CloseDownDisplay() */
void CloseDownDisplay (struct Screen *screen)
{
  uint32 status, received;

#ifdef FM_REACT_SUPPORT
  	FreeReaction ();
#endif
  	if ((status = PubScreenStatus (screen, PSNF_PRIVATE)) & 1) return;
  	
	else
    {	/* Close as soon as possible */
      	DisplayError (NULL, TXT_ERR_CantMakeScreenPrivate, 0);
      	
		while (1)
		{
	  		received = Wait (lastsignal);
	  		if ((status = PubScreenStatus (screen, PSNF_PRIVATE)) & 1) return;
	  		DisplayError (NULL, TXT_ERR_CantMakeScreenPrivate, 0);
		}
    }
}

/* main startup */
int main (int Argc, char **Argv)
{
  int32 ReturnCode = 0;
  STRPTR StopString;
  STRPTR *IconToolTypes = NULL;
  struct ScreenModeRequester *SMReq = NULL;

	/* if (UtilityBase = OpenLibrary("utility.library", 0))
    {
        if (IUtility = (struct IUtility *) GetInterface(UtilityBase, "main", 1, NULL))
        {
            if (GadToolsBase = OpenLibrary("gadtools.library", 0))
            {
                if (!(IGadTools = (struct IGadTools *) GetInterface(GadToolsBase, "main", 1, NULL)))
                {
                    RETURNVALUE = RETURN_FAIL; goto exit_os4;
                }
            }       
            else {RETURNVALUE = RETURN_FAIL; goto exit_os4;}
        }
        else {RETURNVALUE = RETURN_FAIL; goto exit_os4;}
    }
    else {RETURNVALUE = RETURN_FAIL; goto exit_os4;} */

	/* use automatic compiler open close resources */
	/* initialization code */

	if (CheckCPU () != VECTORTYPE_ALTIVEC)
   	{
#ifdef __ALTIVEC__
          DisplayError (NULL, TXT_ERR_NoChips, 20L);
          ReturnCode = RETURN_FAIL;
          goto cleanup;
#endif
    }  	
	
	/* java/ieee mode on */
	/*	const vector unsigned int VZero = vec_splat_u32 (0);
    vec_mtvscr (VZero); */

    if ((allocsignal = AllocSignal (-1)) == -1)
    {
      ReturnCode = RETURN_FAIL;
      goto cleanup;
    }

  	/* new for reaction-support */
    lastsignal = 1 << allocsignal;

#ifdef FM_AREXX_SUPPORT
    if ((RexxSysBase = OpenLibrary ("rexxsyslib.library", 36L)) == NULL)
    {
          ReturnCode = RETURN_FAIL;
          goto cleanup;
    }

    if ((IRexxSys = (struct IRexxSys *) GetInterface (RexxSysBase, "main", 1, NULL)) == NULL)
    {
          ReturnCode = RETURN_FAIL;
          goto cleanup;
    }

    NewList (&AREXXEVENTLIST);
    AREXXEVENTLIST.lh_Type = NT_USER;
#endif /* FM_AREXX_SUPPORT */

    if ((MANDChunk = (struct MandelChunk *) AllocVecTags (sizeof (struct MandelChunk),
                        									AVT_Type, MEMF_PRIVATE,	AVT_Contiguous, TRUE, 
															AVT_Lock, TRUE, AVT_Alignment, 16,
                        									AVT_ClearWithValue, 0, TAG_DONE)) == NULL)
    {
          ReturnCode = RETURN_FAIL;
          goto cleanup;
    }

    if ((LSFMChunk = (struct LoadSaveFMChunk *) AllocVecTags (sizeof (struct LoadSaveFMChunk), 
																AVT_Type, MEMF_PRIVATE, AVT_Contiguous, TRUE, 
																AVT_Lock, TRUE,	AVT_Alignment, 16, 
																AVT_ClearWithValue, 0, TAG_DONE)) == NULL)
    {
          ReturnCode = RETURN_FAIL;
          goto cleanup;
    }

    CatalogPtr = OpenCatalog (NULL, FMCATALOGNAME, OC_PreferExternal, TRUE, TAG_DONE);
    LocalizeMenu ((struct NewMenu *) &FLASHMANDELMENU); /* open catalog file or use default language */

    FLASHMANDEL_CHUNK.ch_Data = LSFMChunk;
    MANDChunk->ModeID = INVALID_ID;
    MANDChunk->LeftEdge = 0;
    MANDChunk->TopEdge = 0;
    MANDChunk->Width = DEF_WIDTH;
    MANDChunk->Height = DEF_HEIGHT;
    MANDChunk->Depth = DEF_DEPTH;
    MANDChunk->Iterations = DEF_ITERATIONS;
    MANDChunk->PixelFormat = PIXF_CLUT; // default to 8bit pixelformat (PIXF_CLUT 8bit - PIXF_A8R8G8B8 24bit) 
    MANDChunk->Modulo = DEF_WIDTH;
    MANDChunk->Flags |= (PPC_BIT | LINEAR_BIT | MANDEL_BIT | REAL_BIT);
    MANDChunk->Power = 1; /* classic mandelbrot formula Z=Z^2+C */
    MANDChunk->PrecisionBits = DEF_PRECISION_BITS;
    MANDChunk->RMin = INIT_DEF_RMIN;
    MANDChunk->RMax = INIT_DEF_RMAX;
    MANDChunk->IMin = INIT_DEF_IMIN;
    MANDChunk->IMax = INIT_DEF_IMAX;
    MANDChunk->JKre = INIT_DEF_JKRE;
    MANDChunk->JKim = INIT_DEF_JKIM;

    MYILBM.camg = INVALID_ID;
    MYILBM.Bmhd.w = DEF_WIDTH;
    MYILBM.Bmhd.h = DEF_HEIGHT;
    MYILBM.Bmhd.nPlanes = DEF_DEPTH;
    MYILBM.ParseInfo.propchks = IlbmProps;
    MYILBM.ParseInfo.collectchks = IlbmCollects;
    MYILBM.ParseInfo.stopchks = IlbmStops;
    MYILBM.Bmhd.pageWidth = 0;
    MYILBM.Bmhd.pageHeight = 0;
    MYILBM.stype = CUSTOMSCREEN | SCREENQUIET | CUSTOMBITMAP;
    MYILBM.TBState = TMASK & MASK;
    MYILBM.ucliptype = OSCAN_TEXT;
    MYILBM.brbitmap = NULL;
    MYILBM.EHB = FALSE;
    MYILBM.Autoscroll = FALSE;
    MYILBM.IFFPFlags = NULL;

    Strlcpy (USERNAME_STRING, DEF_USERNAMESTR, sizeof (USERNAME_STRING));  
    Strlcpy (OLDFONTNAME_STR, DEF_FONTNAMESTR, sizeof (OLDFONTNAME_STR));
    Strlcpy (MYFONT, DEF_FONTNAMESTR, sizeof (MYFONT));
    MYFONTSTRUCT.ta_Name = MYFONT;   
    MYFONTSTRUCT.ta_YSize = DEF_FONTSIZE;
    MYFONTSTRUCT.ta_Flags |= FPF_DESIGNED; /* new for reaction-support */
    PRIORITY = DEF_PRIORITY;
    LEVELUNDO = DEF_LEVELUNDO;

    Strlcat (COPYRIGHT_STRING, VERSION, sizeof (COPYRIGHT_STRING));
    Strlcat (COPYRIGHT_STRING, "by ", sizeof (COPYRIGHT_STRING));
    Strlcat (COPYRIGHT_STRING, AUTHOR, sizeof (COPYRIGHT_STRING));
    Strlcat (COPYRIGHT_STRING, " ", sizeof (COPYRIGHT_STRING));      
    Strlcat (COPYRIGHT_STRING, COPYRIGHT_DATE, sizeof (COPYRIGHT_STRING));

    if (Argc) goto clistart; /* CLI launch */
    if (! (IconToolTypes = ArgArrayInit (Argc, Argv))) goto clistart; /* IconToolTypes not allocated */

    MYILBM.Bmhd.w = ArgInt (IconToolTypes, "SCREENWIDTH", DEF_WIDTH);
    MYILBM.Bmhd.w = MIN (MAX_WIDTH, MAX (MYILBM.Bmhd.w, MIN_WIDTH));
    MYILBM.Bmhd.h = ArgInt (IconToolTypes, "SCREENHEIGHT", DEF_HEIGHT);
    MYILBM.Bmhd.h = MIN (MAX_HEIGHT, MAX (MYILBM.Bmhd.h, MIN_HEIGHT));
    MYILBM.Bmhd.nPlanes = ArgInt (IconToolTypes, "SCREENDEPTH", DEF_DEPTH);
    MYILBM.Bmhd.nPlanes = MIN (MAX_DEPTH, MAX (MYILBM.Bmhd.nPlanes, MIN_DEPTH));

#ifndef NDEBUG
	//  printf("SCREENWIDTH: %ld\n", MYILBM.Bmhd.w); //DEBUG
	//  printf("SCREENHEIGHT: %ld\n", MYILBM.Bmhd.h); //DEBUG
	//  printf("SCREENDEPTH: %ld\n", MYILBM.Bmhd.nPlanes); //DEBUG
#endif /* !NDEBUG */

    sscanf (ArgString (IconToolTypes, "SCREENMODE", DEF_MONITORSTR), "%lx", &MYILBM.camg);

#ifndef NDEBUG
	//  printf("SCREENMODE: 0x%08X\n", (unsigned int) MYILBM.camg); //DEBUG
#endif /* !NDEBUG */

    MANDChunk->RMin = strtod (ArgString (IconToolTypes, "REALMIN", INIT_DEF_RMINSTR), &StopString);
    if ((MANDChunk->RMin == 0.0) && (errno == ERANGE)) MANDChunk->RMin = INIT_DEF_RMIN;
    MANDChunk->RMax = strtod (ArgString (IconToolTypes, "REALMAX", INIT_DEF_RMAXSTR), &StopString);
    if ((MANDChunk->RMax == 0.0) && (errno == ERANGE)) MANDChunk->RMax = INIT_DEF_RMAX;
    MANDChunk->IMin = strtod (ArgString (IconToolTypes, "IMAGMIN", INIT_DEF_IMINSTR), &StopString);
    if ((MANDChunk->IMin == 0.0) && (errno == ERANGE)) MANDChunk->IMin = INIT_DEF_IMIN;
    MANDChunk->IMax = strtod (ArgString (IconToolTypes, "IMAGMAX", INIT_DEF_IMAXSTR), &StopString);
    if ((MANDChunk->IMax == 0.0) && (errno == ERANGE)) MANDChunk->IMax = INIT_DEF_IMAX;

    if ((MANDChunk->RMin >= MANDChunk->RMax) || (MANDChunk->IMin >= MANDChunk->IMax))
	{
        MANDChunk->RMin = INIT_DEF_RMIN;
        MANDChunk->RMax = INIT_DEF_RMAX;
        MANDChunk->IMin = INIT_DEF_IMIN;
        MANDChunk->IMax = INIT_DEF_IMAX;
    }

    MANDChunk->Iterations = strtoul (ArgString (IconToolTypes, "ITERATIONS", DEF_ITERATIONSSTR), &StopString, 10);
    if (!(MANDChunk->Iterations)) MANDChunk->Iterations = DEF_ITERATIONS;

    MANDChunk->JKre = strtod (ArgString (IconToolTypes, "JULIACONSTREAL", INIT_DEF_JKRESTR), &StopString);
    if ((MANDChunk->JKre == 0.0) && (errno == ERANGE)) MANDChunk->JKre = INIT_DEF_JKRE;

    MANDChunk->JKim = strtod (ArgString (IconToolTypes, "JULIACONSTIMAG", INIT_DEF_JKIMSTR), &StopString);
	if ((MANDChunk->JKim == 0.0) && (errno == ERANGE)) MANDChunk->JKim = INIT_DEF_JKIM;

    if (ArgInt (IconToolTypes, "STARTWITHJULIA", FALSE))
    {
        MANDChunk->Flags &= ~MANDEL_BIT;
        MANDChunk->Flags |= JULIA_BIT;
    }

    if (ArgInt (IconToolTypes, "TURBORENDER", FALSE)) MANDChunk->Flags |= TURBO_BIT;
    if (ArgInt (IconToolTypes, "HISTOGRAMRENDER", FALSE) && (MANDChunk->Flags & TURBO_BIT) && (MYILBM.Bmhd.nPlanes == MIN_DEPTH)) MANDChunk->Flags |= HISTOGRAM_BIT;

	switch (ArgInt (IconToolTypes, "POWER", FALSE))
  	{
      case 2:
          MANDChunk->Power = 1;
      break;
	  
      case 4:
          MANDChunk->Power = 2;
      break;
	  
      case 8:
          MANDChunk->Power = 3;
      break;
	  
      case 16:
          MANDChunk->Power = 4;
      break;
	  
      case 32:
          MANDChunk->Power = 5;
      break;
	  
      case 64:
          MANDChunk->Power = 6;
      break;
	  
      case 128:
          MANDChunk->Power = 7;
      break;
	  
      case 256:
          MANDChunk->Power = 8;
      break;
	  
      case 512:
          MANDChunk->Power = 9;
      break;
	  
      case 1024:
          MANDChunk->Power = 10;
      break;
	  
      case 2048:
          MANDChunk->Power = 11;
      break;
	  
      default:
          MANDChunk->Power = 1;
      break;
  	}

	switch (ArgInt (IconToolTypes, "COLORSREMAP", FALSE))
  	{
      case 0:
          MANDChunk->Flags |= LINEAR_BIT;
      break;
	  
      case 1:
        MANDChunk->Flags |= LOG_BIT;
      break;
	  
      case 2:
          MANDChunk->Flags |= REPEATED_BIT;
      break;
	  
      case 3:
          MANDChunk->Flags |= SQUARE_BIT;
      break;
	  
      case 4:
          MANDChunk->Flags |= ONE_BIT;
      break;
	  
      case 5:
          MANDChunk->Flags |= TWO_BIT;
      break;
	  
      case 6:
          MANDChunk->Flags |= THREE_BIT;
      break;
	  
      case 7:
          MANDChunk->Flags |= FOUR_BIT;
      break;
	  
      default:
          MANDChunk->Flags |= LINEAR_BIT;
      break;
  	}

    Strlcpy (USERNAME_STRING, ArgString (IconToolTypes, "USERNAME", DEF_USERNAMESTR), sizeof (USERNAME_STRING));
	Strlcpy (MYFONT, ArgString (IconToolTypes, "FONTNAME", DEF_FONTNAMESTR), sizeof (MYFONT));
    MYFONTSTRUCT.ta_Name = MYFONT;
    MYFONTSTRUCT.ta_YSize = ArgInt (IconToolTypes, "FONTSIZE", DEF_FONTSIZE);
    MYFONTSTRUCT.ta_YSize = MIN (MAX_FONTSIZE, MAX (MYFONTSTRUCT.ta_YSize, MIN_FONTSIZE));
    PRIORITY = ArgInt (IconToolTypes, "STARTPRI", DEF_PRIORITY);
    PRIORITY = MIN (MAX_PRIORITY, MAX (PRIORITY, MIN_PRIORITY));

    switch (ArgInt (IconToolTypes, "MATHMODE", TRUE))
  	{
      case 1:
          MANDChunk->Flags &= ~HIGHPREC_BIT;
      break;
	  
      case 2:
          MANDChunk->Flags |= HIGHPREC_BIT;
      break;
	  
      default:
          MANDChunk->Flags &= ~HIGHPREC_BIT;
      break;
  	}

    MANDChunk->PrecisionBits = ArgInt (IconToolTypes, "PRECISIONBITS", DEF_PRECISION_BITS);
    MANDChunk->PrecisionBits = MIN (MAX_PRECISION_BITS, MAX (MANDChunk->PrecisionBits, MIN_PRECISION_BITS));
    LEVELUNDO = (uint32) ArgInt (IconToolTypes, "UNDOLEVEL", DEF_LEVELUNDO);
    LEVELUNDO = MIN (MAX_LEVELUNDO, MAX (LEVELUNDO, MIN_LEVELUNDO));

clistart:
    if (!(UNDOBuffer = (struct UndoBuffer *) AllocVecTags ((sizeof (struct UndoBuffer) * LEVELUNDO), AVT_Type, MEMF_PRIVATE, 
															AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
															AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE)))
  	{
    	ReturnCode = RETURN_FAIL;
        goto cleanup;
  	}

    DEF_RMIN = MANDChunk->RMin;
    DEF_RMAX = MANDChunk->RMax;
    DEF_IMIN = MANDChunk->IMin;
    DEF_IMAX = MANDChunk->IMax;
    DEF_JKRE = MANDChunk->JKre;
    DEF_JKIM = MANDChunk->JKim;

    PALETTE = &PALETTE256;

    MYILBM.camg = BestModeID (BIDTAG_NominalWidth, MYILBM.Bmhd.w, BIDTAG_DesiredWidth, MYILBM.Bmhd.w, 
								BIDTAG_NominalHeight, MYILBM.Bmhd.h, BIDTAG_DesiredHeight, MYILBM.Bmhd.h, 
								BIDTAG_Depth, MYILBM.Bmhd.nPlanes, BIDTAG_DIPFMustNotHave, (DIPF_IS_DUALPF|DIPF_IS_PF2PRI|DIPF_IS_HAM|DIPF_IS_EXTRAHALFBRITE|DIPF_IS_PAL), 
								TAG_DONE);

    
	if ((MYILBM.camg != INVALID_ID) && ArgInt (IconToolTypes, "SMREQUESTER", TRUE))
	{
		if (SMReq = AllocAslRequest (ASL_ScreenModeRequest, NULL))
  		{
        	if (AslRequestTags (SMReq,
								ASLSM_TitleText, "FlashMandel ScreenMode Requester",
								ASLSM_InitialDisplayID, MYILBM.camg,
								ASLSM_InitialDisplayWidth, MYILBM.Bmhd.w,
				            	ASLSM_InitialDisplayHeight, MYILBM.Bmhd.h,
				            	ASLSM_InitialDisplayDepth, MYILBM.Bmhd.nPlanes, 
				            	ASLSM_InitialInfoOpened, FALSE, 
				            	ASLSM_DoWidth, FALSE, 
				            	ASLSM_DoHeight, FALSE, 
				            	ASLSM_DoDepth, FALSE,
				            	ASLSM_DoOverscanType, FALSE,           
				            	ASLSM_PropertyFlags, DIPF_IS_RTG|DIPF_IS_WB,
				            	ASLSM_PropertyMask, DIPF_IS_DUALPF|DIPF_IS_PF2PRI|DIPF_IS_HAM|DIPF_IS_EXTRAHALFBRITE|DIPF_IS_PAL,                       
				            	ASLSM_MinWidth, MIN_WIDTH, 
				            	ASLSM_MinHeight, MIN_HEIGHT, 
				            	ASLSM_MinDepth, MIN_DEPTH, 
				            	ASLSM_MaxDepth, MAX_DEPTH,
				            	ASLSM_FilterFunc, &SMFILTERHOOK,
				            	TAG_DONE))
      		{
            	MYILBM.camg = SMReq->sm_DisplayID;
            	MYILBM.Bmhd.w = SMReq->sm_DisplayWidth;
            	MYILBM.Bmhd.h = SMReq->sm_DisplayHeight;
            	MYILBM.Bmhd.nPlanes = SMReq->sm_DisplayDepth;
      		}

       		else
      		{
            	FreeAslRequest (SMReq);
            	ReturnCode = RETURN_FAIL;
            	goto cleanup;
      		}

        	FreeAslRequest (SMReq);
  		}
	}	

	// if (MYILBM.Bmhd.nPlanes == MAX_DEPTH) MANDChunk->Flags |= TURBO_BIT;    /* force use of turbo render for 24bit screens */
    if (MYILBM.camg == INVALID_ID)
  	{
        DisplayError (NULL, TXT_ERR_ModeNotAvailable, 20L);
        ReturnCode = RETURN_FAIL;
        goto cleanup;
  	}

    if (! MakeDisplay (&MYILBM))
  	{
        DisplayError (NULL, TXT_ERR_MakeDisplay, 20L);
        CloseDisplay (&MYILBM);
        ReturnCode = RETURN_FAIL;		
        goto cleanup;
  	}

    Forbid ();
    SetTaskPri (FindTask (NULL), PRIORITY);
    Permit ();

    MANDChunk->ModeID = MYILBM.camg;
    MANDChunk->Width = MYILBM.win->GZZWidth;
    MANDChunk->Height = MYILBM.win->GZZHeight;
	MANDChunk->Depth = MYILBM.Bmhd.nPlanes;
	MANDChunk->Modulo = MYILBM.win->GZZWidth;
    MANDChunk->PixelFormat = GetBitMapAttr (MYILBM.win->RPort->BitMap, BMA_PIXELFORMAT);
//	LOCK = LockBitMapTags (MYILBM.win->RPort->BitMap,LBM_BytesPerRow,&MANDChunk->Modulo,LBM_PixelFormat,&MANDChunk->PixelFormat,TAG_DONE);
//	UnlockBitMap (LOCK);
    MANDChunk->PrecisionDigits = (int16) ceil (log10 (pow (2, MANDChunk->PrecisionBits)));

	/* initialize MandChunk and global GMP vars */
    Init_MANDChunk_GMP (MAX_PRECISION_BITS); /*MANDChunk->PrecisionBits */
    Init_GMP (MANDChunk->PrecisionBits);

	/* convert real and imag coordinates into GMP number */
    mpf_set_d (MANDChunk->GIMin, MANDChunk->IMin);
    mpf_set_d (MANDChunk->GRMin, MANDChunk->RMin);
    mpf_set_d (MANDChunk->GIMax, MANDChunk->IMax);
    mpf_set_d (MANDChunk->GRMax, MANDChunk->RMax);
    mpf_set_d (MANDChunk->GJKre, MANDChunk->JKre);
    mpf_set_d (MANDChunk->GJKim, MANDChunk->JKim);

    PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
    ELAPSEDTIME = DrawFractal (MANDChunk, MYILBM.win, ARGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, FALSE);
    ClearPointer (MYILBM.win);

    SetMenuStart (&MYILBM, UNDOCOUNTER);
    ShowTime (MYILBM.win, CATSTR (TXT_RenderTime), ELAPSEDTIME);

#ifdef FM_AREXX_SUPPORT
    CreateARexxMenu (NULL);
    SetARexxMenu (&MYILBM);
	if (ArgInt (IconToolTypes, "PLAYSOUND", TRUE)) LaunchIt (&MYILBM, "startup.rexx", "arexx");
#endif /* FM_AREXX_SUPPORT */

    ShowTitle (MYILBM.win->WScreen, FALSE);
    ModifyIDCMP (MYILBM.win, NULL);
    ClearMenuStrip (MYILBM.win);

#ifdef FM_REACT_SUPPORT
    Do_InfoRequest (MYILBM.win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
    About (MYILBM.win);
#endif /* FM_REACT_SUPPORT */

    ResetMenuStrip (MYILBM.win, MAINMENU);
    ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
    if (TMASK & MASK) ShowTitle (MYILBM.win->WScreen, TRUE);

    ReturnCode = MainProg (&MYILBM, MANDChunk);

#ifdef FM_AREXX_SUPPORT
    RemoveARexxMenu (&MYILBM);
    FreeARexxMenu ();
#endif /* FM_AREXX_SUPPORT */

	/* Free global GMP vars */
    Clear_MANDChunk_GMP ();
    Clear_UNDOBuffer_GMP (UNDOCOUNTER); /* if there's something inside Undo buffer free it */
    Clear_GMP ();
	/* close screen */
    Fade (MYILBM.win, ARGBMEM, PALETTE, 50L, 1L, TOBLACK);
    CloseDisplay (&MYILBM);
	/* free all memory allocated */
cleanup:

    if (NEWFONT) CloseFont (NEWFONT);
    if (IconToolTypes) ArgArrayDone ();
    if (CatalogPtr) CloseCatalog (CatalogPtr); /* new for reaction-support */
    if (UNDOBuffer) FreeVec (UNDOBuffer);
    if (RNDMEM) FreeVec (RNDMEM);
    if (MANDChunk) FreeVec (MANDChunk);
    if (LSFMChunk) FreeVec (LSFMChunk);
#ifdef FM_AREXX_SUPPORT
    if (IRexxSys) DropInterface ((struct Interface *) IRexxSys);
    if (RexxSysBase) CloseLibrary (RexxSysBase);
#endif /* FM_AREXX_SUPPORT */
    if (allocsignal) FreeSignal (allocsignal);
	
  	//if (IGadTools) DropInterface((struct Interface *) IGadTools);
  	//if (GadToolsBase) CloseLibrary(GadToolsBase);
  	//if (IUtility) DropInterface((struct Interface *) IUtility);
  	//if (UtilityBase) CloseLibrary(UtilityBase);
  	//IExec->DropInterface((struct Interface*) IAsl);
    //IExec->CloseLibrary(AslBase);
	//use auto close libraries 
  	
#ifdef __ALTIVEC__
	/*java/ieee mode off*/
	/* const vector unsigned int VOne = vec_splat_u32 (1);
	vec_mtvscr (VOne); */ 
#endif /* __ALTIVEC__ */
	    
	exit (ReturnCode);
}

/* mainprog() */
int32 MainProg (struct ILBMInfo *Ilbm, struct MandelChunk *MandelInfo)
{
    while (! res)
    {
        while (HandleEvents (Ilbm, MandelInfo) & NEWDISPLAY_MSG)
        {
            if (SMRequest (Ilbm))
            {
                ClearZoomFrame (Ilbm->wrp);
 
                if (MandelInfo->Depth <= Ilbm->Bmhd.nPlanes)
                {
                    MYBITMAP = CopyBitMap (Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) Ilbm->win->GZZWidth, (uint16) Ilbm->win->GZZHeight);
                }

                else 
				{
					MYBITMAP = NULL;
					MASK &= ~BMASK;
				}

                Fade (Ilbm->win, ARGBMEM, PALETTE, 25L, 1L, TOBLACK);
                CloseDisplay (Ilbm);

                if (! MakeDisplay (Ilbm))
                {
                    if (MASK & BMASK)
                    {
                        if (MYBITMAP)
                        {
                            FreeBitMapSafety (MYBITMAP);
                            MYBITMAP = NULL;
                            MASK &= ~BMASK;
                        }
                    }

                    DisplayError (NULL, TXT_ERR_MakeDisplay, 20L);
                    CloseDisplay (Ilbm);
                    RETURNVALUE = 20;
                    goto ExitMainProg;
                }

                MandelInfo->ModeID = Ilbm->camg;
                MandelInfo->Width = Ilbm->win->GZZWidth;
                MandelInfo->Height = Ilbm->win->GZZHeight;
				MandelInfo->Depth = Ilbm->Bmhd.nPlanes;
				MandelInfo->Modulo = Ilbm->win->GZZWidth;
//				MandelInfo->Depth = GetBitMapAttr (Ilbm->win->RPort->BitMap, BMA_DEPTH);
                MandelInfo->PixelFormat = GetBitMapAttr (Ilbm->win->RPort->BitMap, BMA_PIXELFORMAT);
//				LOCK = LockBitMapTags (Ilbm->win->RPort->BitMap,LBM_BytesPerRow,&MandelInfo->Modulo,LBM_PixelFormat,&MandelInfo->PixelFormat,TAG_DONE);
//				UnlockBitMap (LOCK);

  				if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, FALSE);

                if (MYBITMAP) PasteBitMap (MYBITMAP, Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) GetBitMapAttr (MYBITMAP, BMA_WIDTH), (uint16) GetBitMapAttr (MYBITMAP, BMA_HEIGHT));

				// if (Ilbm->Bmhd.nPlanes == MAX_DEPTH)    MandelInfo->Flags |= TURBO_BIT;    /* force use of turbo render for 24bit screens */

#ifdef FM_REACT_SUPPORT
                res = Do_RenderRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
                res = Choice (Ilbm->win, CATSTR (TITLE_RenderReq), CATSTR (NewScr_TXT_Question));
#endif /* FM_REACT_SUPPORT */

  				if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);

                if (res)
                {
                    SetMenuStop (Ilbm);
                    PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                    ELAPSEDTIME =  DrawFractal (MandelInfo, Ilbm->win, ARGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, TRUE);
                    SetMenuStart (Ilbm, UNDOCOUNTER);
                    ShowTime (Ilbm->win, CATSTR (TXT_RenderTime), ELAPSEDTIME);
                }

                else SetMenuStart (Ilbm, UNDOCOUNTER);
            }
        }
		
		if (ForceAbort == FALSE)
		{
#ifdef FM_REACT_SUPPORT
	  		res = Do_ExitRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
	  		res = Choice (Ilbm->win, CATSTR (TITLE_ExitReq), CATSTR (Prt_TXT_AreYouSure));
#endif /* FM_REACT_SUPPORT */
		}

      	else res = TRUE;		
	}

 	ClearZoomFrame (Ilbm->wrp);

ExitMainProg:
	return (RETURNVALUE);
}

/***********************************************************************************************************/
void Init_MANDChunk_GMP (uint32 precision_bits)
{
	mpf_set_default_prec (precision_bits);

    mpf_inits (MANDChunk->GIMin, MANDChunk->GIMax,
               MANDChunk->GRMin, MANDChunk->GRMax,
               MANDChunk->GJKre, MANDChunk->GJKim, 0);
}

void Clear_MANDChunk_GMP (void)
{
	mpf_clears (MANDChunk->GIMin, MANDChunk->GIMax, MANDChunk->GRMin,
                MANDChunk->GRMax, MANDChunk->GJKre, MANDChunk->GJKim, 0);
}

void Reset_MANDChunk_GMP (uint32 precision_bits)
{
	mp_exp_t exp = 0;

    if (mpf_get_prec (gtmp) != precision_bits)
    {
		/* MPF_GET_STR BUGGY GMP FUNCTION - GOING WITH GMP_SNPRINTF */
		//      strcpy (LSFMChunk->GRMinSTR,mpf_get_str (/*&LSFMChunk->GRMinSTR*/ 0,&exp,10,0,MANDChunk->GRMin));
		//      strcpy (LSFMChunk->GIMinSTR,mpf_get_str (/*&LSFMChunk->GIMinSTR*/ 0,&exp,10,0,MANDChunk->GIMin));
		//      strcpy (LSFMChunk->GRMaxSTR,mpf_get_str (/*&LSFMChunk->GRMaxSTR*/ 0,&exp,10,0,MANDChunk->GRMax));
		//      strcpy (LSFMChunk->GIMaxSTR,mpf_get_str (/*&LSFMChunk->GIMaxSTR*/ 0,&exp,10,0,MANDChunk->GIMax));
		//      strcpy (LSFMChunk->GJKreSTR,mpf_get_str (/*&LSFMChunk->GJKreSTR*/ 0,&exp,10,0,MANDChunk->GJKre));
		//      strcpy (LSFMChunk->GJKimSTR,mpf_get_str (/*&LSFMChunk->GJKimSTR*/ 0,&exp,10,0,MANDChunk->GJKim));
        gmp_snprintf (LSFMChunk->GRMinSTR, precision_bits, "%.1235Ff", MANDChunk->GRMin);
        gmp_snprintf (LSFMChunk->GRMaxSTR, precision_bits, "%.1235Ff", MANDChunk->GRMax);
        gmp_snprintf (LSFMChunk->GIMinSTR, precision_bits, "%.1235Ff", MANDChunk->GIMin);
        gmp_snprintf (LSFMChunk->GIMaxSTR, precision_bits, "%.1235Ff", MANDChunk->GIMax);
        gmp_snprintf (LSFMChunk->GJKreSTR, precision_bits, "%.1235Ff", MANDChunk->GJKre);
        gmp_snprintf (LSFMChunk->GJKimSTR, precision_bits, "%.1235Ff", MANDChunk->GJKim);

        Clear_MANDChunk_GMP ();
        Init_MANDChunk_GMP (precision_bits);

		// sscanf is another possibility to save values
		/* gmp_sscanf (LSFMChunk->GRMinSTR,"%Ff",&MANDChunk->GRMin);
        gmp_sscanf (LSFMChunk->GIMinSTR,"%Ff",&MANDChunk->GIMin);
        gmp_sscanf (LSFMChunk->GRMaxSTR,"%Ff",&MANDChunk->GRMax);
        gmp_sscanf (LSFMChunk->GIMaxSTR,"%Ff",&MANDChunk->GIMax);
        gmp_sscanf (LSFMChunk->GJKreSTR,"%Ff",&MANDChunk->GJKre);
        gmp_sscanf (LSFMChunk->GJKimSTR,"%Ff",&MANDChunk->GJKim); */

        mpf_set_str (MANDChunk->GRMin, &LSFMChunk->GRMinSTR, 10);
        mpf_set_str (MANDChunk->GIMin, &LSFMChunk->GIMinSTR, 10);
        mpf_set_str (MANDChunk->GRMax, &LSFMChunk->GRMaxSTR, 10);
        mpf_set_str (MANDChunk->GIMax, &LSFMChunk->GIMaxSTR, 10);
        mpf_set_str (MANDChunk->GJKre, &LSFMChunk->GJKreSTR, 10);
        mpf_set_str (MANDChunk->GJKim, &LSFMChunk->GJKimSTR, 10);
    }
}

void Clear_UNDOBuffer_GMP (uint32 counter)
{
    while (counter > 0)
    {
        counter--;

        mpf_clears (UNDOBuffer[counter].GRMin, UNDOBuffer[counter].GRMax,
                    UNDOBuffer[counter].GIMin, UNDOBuffer[counter].GIMax,
                    UNDOBuffer[counter].GJKre, UNDOBuffer[counter].GJKim, 0);
    }
}

void Init_GMP (uint32 precision_bits)
{
    mpf_set_default_prec (precision_bits);
    mpf_inits (gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2,
               gcre3, gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp,
               gdist, gmaxdist, gincremreal, gincremimag, gpzr, gpzi, 0);
    mpf_set_ui (gmaxdist, 4);
}

void Clear_GMP (void)
{
	mpf_clears (gzr, gzi, gzr2, gzi2, gcre, gcim, gcre1, gcim1, gcre2, gcim2,
                gcre3, gcim3, gjkre, gjkim, grmin, gimin, grmax, gimax, gtmp,
                gdist, gmaxdist, gincremreal, gincremimag, gpzr, gpzi, 0);
}

void Reset_GMP (uint32 precision_bits)
{
    if (mpf_get_prec (gtmp) != precision_bits)
    {
        Clear_GMP ();
        Init_GMP (precision_bits);
    }
}

/* PutPointer() */
void PutPointer (struct Window *Win, uint16 * PointerImage, int16 Width, int16 Height, int16 XOrigin, int16 YOrigin, uint8 Type)
{
    switch (Type)
    {
        case CLEAR_POINTER:
            SetWindowPointer (Win, TAG_DONE);
        break;
		
        case BUSY_POINTER:
              SetWindowPointer (Win, WA_BusyPointer, TRUE, TAG_DONE);
        break;
		  
        case ZOOM_POINTER:
            SetPointer (Win, PointerImage, Height, Width, XOrigin, YOrigin);
        break;
    }
}

/* CheckCPU() */
uint32 CheckCPU (void)
{
  STRPTR txt_none = CATSTR (TXT_None);
  STRPTR txt_builtin = CATSTR (TXT_Unknown);
  STRPTR txt_unknown = CATSTR (TXT_Unknown);
  uint32 vectorunit;
  CONST_STRPTR modelstring;
  CONST_STRPTR versionstring;

    /* VECTORTYPE_NONE = 0 - VECTORTYPE_ALTIVEC = 1 - VECTORTYPE_VMX = 2 */
	
    GetCPUInfoTags (GCIT_ModelString, &modelstring, GCIT_VersionString, &versionstring, GCIT_VectorUnit, &vectorunit, TAG_DONE);
    Strlcpy (CPUPPC_STR, modelstring, sizeof (CPUPPC_STR));    
	Strlcpy (VERPPC_STR, versionstring, sizeof (VERPPC_STR));
	
	switch (vectorunit)
	{
		case VECTORTYPE_NONE:	
			Strlcpy (VECPPC_STR, txt_none, sizeof (VECPPC_STR));
		break;
		
		case VECTORTYPE_ALTIVEC:
			Strlcpy (VECPPC_STR, "Altivec", sizeof (VECPPC_STR));
		break;
		
		/* case VECTORTYPE_VMX:
			Strlcpy (VECPPC_STR, "VMX", Strlen ("VMX") + 1);
		break; */
		
		default:
			Strlcpy (VECPPC_STR, "Unknown", sizeof (VECPPC_STR));
		break;	
	}
	
    return (vectorunit);
}

/* SetPubScreenName() */
int32 SetPubScreenName (STRPTR PubScreenName)
{
  struct Screen *Scr = NULL;
  int32 Suffix = 2;
  char FirstPubScreenName [MAXPUBSCREENNAME+1], CurrentPubScreenName [MAXPUBSCREENNAME+1];
  STRPTR TmpName;
  
	TmpName = NextPubScreen (Scr, CurrentPubScreenName);           
    Strlcpy (FirstPubScreenName, TmpName, sizeof (FirstPubScreenName));

    while (TmpName)
    {
    	if (Stricmp (TmpName, PubScreenName) == 0) // members are equal
        {
        	SNPrintf (PubScreenName, sizeof (FirstPubScreenName), "FlashMandel.%ld", Suffix++);
        }
		
		if (!(Scr = LockPubScreen (TmpName))) break;
        TmpName = NextPubScreen (Scr, CurrentPubScreenName);
        UnlockPubScreen (NULL, Scr);

        if (Stricmp (TmpName, FirstPubScreenName) == 0) break;			
    }  

    return (Suffix-1);
}

int16 ClearZoomFrame (struct RastPort *RPort)
{
  int16 Cleared = FALSE;
  
	if (ZMASK & MASK)
    {
    	DrawBorder (RPort, &MYBORDER, 0, 0);
        MASK &= ~ZMASK;
		Cleared = TRUE;
    }
	
	return (Cleared);
}	

/* OpenDisplay() */
struct Window *OpenDisplay (struct ILBMInfo *Ilbm, int16 Width, int16 Height, int16 Depth, uint32 ModeID)
{
  struct Screen *Scr = NULL;
  struct Window *Win = NULL;

    if (Scr = OpenIdScreen (Ilbm, Width, Height, Depth, ModeID))
    {
    	if (Win = OpenWindowTags (NULL, WA_Left, Scr->LeftEdge, WA_Top, Scr->TopEdge, 
									WA_Width, Scr->Width, WA_Height, Scr->Height, 
									WA_ScreenTitle, COPYRIGHT_STRING, WA_CustomScreen, Scr,
              						WA_IDCMP, IDCMP_STANDARD, WA_Flags,	WFLG_STANDARD, 
									WA_MouseQueue, 1L, WA_BusyPointer, TRUE, 
									WA_NewLookMenus, TRUE, WA_DropShadows, TRUE, TAG_DONE))
    	{
      		Ilbm->scr = Scr;
      		Ilbm->win = Win;
			// Ilbm->vp  = &(Scr->ViewPort);
      		Ilbm->vp = ViewPortAddress (Win);
      		Ilbm->srp = &(Scr->RastPort);
      		Ilbm->wrp = Win->RPort;
      		Ilbm->UserFlags = 0; // Ilbm compression -> TRUE == cmpByteRun1 - FALSE == cmpNone
      		Ilbm->UserData = NULL;

      		goto ExitOpenDisplay;
    	}
    }

  	else
    {
      	if (CMASK & MASK)
    	{
      		if (Ilbm->brbitmap)
        	{
          		FreeBitMapSafety (Ilbm->brbitmap);
          		Ilbm->brbitmap = NULL;
          		MASK &= ~CMASK;
        	}
    	}

      	if (Win) CloseWindow (Win);
      	if (Scr) CloseScreen (Scr);
    }

ExitOpenDisplay:	
	return (Win);
}

/* OpenIdScreen() */
struct Screen *OpenIdScreen (struct ILBMInfo *Ilbm, int16 Width, int16 Height, int16 Depth, uint32 ModeID)
{
  struct Screen *Scr = NULL;
  struct Task *task = NULL; /* new for reaction-support */
  int32 ErrorCode = NULL;
  uint32 BitMapTag, PassedTags;

  	if (!Ilbm) goto ExitOpenIdScreen;

  	ModeID = BestModeID (BIDTAG_NominalWidth, Width, BIDTAG_NominalHeight, Height,
        					BIDTAG_DesiredWidth, Width, BIDTAG_DesiredHeight, Height,
        					BIDTAG_Depth, Depth, BIDTAG_DIPFMustNotHave,
        					(DIPF_IS_DUALPF | DIPF_IS_PF2PRI | DIPF_IS_HAM |
	       					DIPF_IS_EXTRAHALFBRITE | DIPF_IS_PAL), TAG_DONE);

  	if (ModeID == INVALID_ID)
    {
      	DisplayError (Ilbm->win, TXT_ERR_ModeNotAvailable, 20L);
      	goto ExitOpenIdScreen;
    }

  	Ilbm->camg = ModeID;

  	if (Ilbm->stype & CUSTOMBITMAP)
    {
      	if (Ilbm->brbitmap = AllocBitMapTags (Width, Height, Depth,
                        						BMATags_Displayable, FALSE,
                        						BMATags_Alignment, 16,
                        						BMATags_ConstantBytesPerRow, TRUE, 
												BMATags_Clear, TRUE,
                        						BMATags_PixelFormat, ((Depth == MIN_DEPTH) ? PIXF_CLUT : PIXF_A8R8G8B8),
                        						BMATags_ModeWidth, Width,
                        						BMATags_ModeHeight, Height,
                        						BMATags_DisplayID, ModeID, TAG_DONE))
    	MASK |= CMASK;
    }

  	BitMapTag = (Ilbm->brbitmap ? SA_BitMap : TAG_IGNORE);
  	PassedTags = (Ilbm->stags ? TAG_MORE : TAG_IGNORE);

  	Forbid ();
  	task = (struct Task *) &((struct Process *) FindTask (NULL))->pr_Task; /* new for reaction-support */
  	Permit ();

  	SetPubScreenName (FMSCREENNAME);

  	Scr = OpenScreenTags (NULL, SA_DisplayID, ModeID, SA_Type, Ilbm->stype, SA_Width, Width, SA_Height, Height, SA_Depth, Depth, 
							SA_Colors32, PALETTE, SA_Pens, PENS, SA_DetailPen, WHITE, SA_BlockPen, BLACK, 
							SA_Interleaved, TRUE, SA_Font, &MYFONTSTRUCT, SA_Title, COPYRIGHT_STRING, SA_PubName, FMSCREENNAME,    /* new for reaction-support */
            				SA_PubSig, allocsignal, /* new for reaction-support */
            				SA_PubTask, task, /* new for reaction-support */
            				SA_SharePens, FALSE,
            				SA_OffScreenDragging, TRUE,
            				SA_Compositing, TRUE,
            				SA_WindowDropShadows, TRUE,
            				SA_ErrorCode, &ErrorCode, 
							SA_ShowTitle, Ilbm->TBState,
							//  SA_BitMap, Ilbm->brbitmap,
            				BitMapTag, Ilbm->brbitmap,
            				PassedTags, Ilbm->stags, TAG_DONE);

  	if (!Scr)
    {
      	switch (ErrorCode)
    	{
    		case OSERR_NOMONITOR:
      			DisplayError (NULL, TXT_ERR_NoMonitor, 20L);
      		break;
			
    		case OSERR_NOCHIPS:
		      	DisplayError (NULL, TXT_ERR_NoChips, 20L);
      		break;
			
    		case OSERR_NOMEM:
      			DisplayError (NULL, TXT_ERR_NoMem, 20L);
      		break;
			
    		case OSERR_NOCHIPMEM:
      			DisplayError (NULL, TXT_ERR_NoChipMem, 20L);
      		break;
			
    		case OSERR_PUBNOTUNIQUE:
      			DisplayError (NULL, TXT_ERR_PubNotUnique, 20L);
      		break;
			
    		case OSERR_UNKNOWNMODE:
      			DisplayError (NULL, TXT_ERR_UnknownMode, 20L);
      		break;
			
    		case OSERR_TOODEEP:
    			DisplayError (NULL, TXT_ERR_ScreenToDeep, 20L);
      		break;
			
    		case OSERR_ATTACHFAIL:
      			DisplayError (NULL, TXT_ERR_AttachScreen, 20L);
      		break;
			
    		case OSERR_NOTAVAILABLE:
      			DisplayError (NULL, TXT_ERR_ModeNotAvailable, 20L);
      		break;
			
    		default:
      			DisplayError (NULL, TXT_ERR_UnknownErr, 20L);
      		break;
    	}

      	goto ExitOpenIdScreen;
    }
#ifdef FM_REACT_SUPPORT
  	oldstatus = PubScreenStatus (Scr, 0); /* new for reaction-support */
  	InitReaction (FMSCREENNAME);
#endif
ExitOpenIdScreen:
  	return (Scr);
}

/* MakeDisplay() */
int32 MakeDisplay (struct ILBMInfo * Ilbm)
{
  static uint32 SAVED_COMPONENT = NULL, SAVED_POSITION = NULL;
  int32 NColors = 0L;

  	Ilbm->Bmhd.w = MIN (MAX_WIDTH, MAX (MIN_WIDTH, Ilbm->Bmhd.w));
  	Ilbm->Bmhd.h = MIN (MAX_HEIGHT, MAX (MIN_HEIGHT, Ilbm->Bmhd.h));
  	Ilbm->Bmhd.nPlanes = MIN (MAX_DEPTH, MAX (MIN_DEPTH, Ilbm->Bmhd.nPlanes));

	// if (Ilbm->Bmhd.nPlanes == MAX_DEPTH)
  	{
    	if (!(ARGBMEM = AllocVecTags ((Ilbm->Bmhd.w) * (Ilbm->Bmhd.h) * 4, AVT_Type, MEMF_PRIVATE, 
										AVT_Contiguous, TRUE, AVT_Lock, TRUE,
               							AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE)))
      	{
         	DisplayError (NULL, TXT_ERR_NoMem, 20L);
			goto ExitMakeDisplay;
      	}
  	}

  	MASK |= AMASK;

  	if (!(RNDMEM = AllocVecTags (sizeof (uint32) * (Ilbm->Bmhd.w) * (Ilbm->Bmhd.h),	AVT_Type, MEMF_PRIVATE, 
									AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
									AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE)))
    {
      	DisplayError (NULL, TXT_ERR_NoMem, 20L);
		goto ExitMakeDisplay;
    }

  	MASK |= RMASK;

  	if (!(GFXMEM = AllocVecTags ((Ilbm->Bmhd.w) * (Ilbm->Bmhd.h), AVT_Type, MEMF_PRIVATE, 
									AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
									AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE)))
    {
      	DisplayError (NULL, TXT_ERR_NoMem, 20L);
		goto ExitMakeDisplay;     	
    }

  	MASK |= PMASK;

  	if (!(PIXMEM = AllocVecTags (MAX (Ilbm->Bmhd.w, Ilbm->Bmhd.h),	AVT_Type, MEMF_PRIVATE, 
									AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
									AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE)))
    {
      	DisplayError (NULL, TXT_ERR_NoMem, 20L);
		goto ExitMakeDisplay;
    }

  	MASK |= LMASK;

  	if ((NEWFONT = OpenDiskFont (&MYFONTSTRUCT)) == NULL)
    {
      	DisplayError (NULL, TXT_ERR_Font, 20L);

      	// default to Topaz font  
      	Strlcpy (MYFONT, "topaz.font", sizeof (MYFONT));
      	MYFONTSTRUCT.ta_Name = MYFONT;
      	MYFONTSTRUCT.ta_YSize = DEF_FONTSIZE;
      	MYFONTSTRUCT.ta_Style = FS_NORMAL;
      	MYFONTSTRUCT.ta_Flags = FPF_ROMFONT | FPF_DESIGNED;

      	if ((NEWFONT = OpenFont (&MYFONTSTRUCT)) == NULL)
      	{
          	DisplayError (NULL, TXT_ERR_Font, 20L);
			goto ExitMakeDisplay;
      	}
    }

  	if (!(OpenDisplay (Ilbm, Ilbm->Bmhd.w, Ilbm->Bmhd.h, Ilbm->Bmhd.nPlanes, Ilbm->camg)))
    {
      	DisplayError (NULL, TXT_ERR_NoMem, 20L);
		goto ExitMakeDisplay;
    }

  	MASK |= SMASK | WMASK;

  	if (!(VINFO = GetVisualInfo (Ilbm->scr, TAG_DONE)))
    {
      	DisplayError (NULL, TXT_ERR_VisualInfo, 20L);
		goto ExitMakeDisplay;
    }

  	MASK |= VMASK;

  	if (!(MAINMENU = CreateMenus (&FLASHMANDELMENU, GTMN_FullMenu, TRUE, TAG_DONE)))
    {
      	DisplayError (NULL, TXT_ERR_Menu, 20L);
		goto ExitMakeDisplay;
    }

  	MASK |= MMASK;

  	if (!(LayoutMenus (MAINMENU, VINFO, /*GTMN_TextAttr, &MYFONTSTRUCT, GTMN_CharSet, 3,*/ GTMN_NewLookMenus, TRUE, TAG_DONE)))
    {
      	DisplayError (NULL, TXT_ERR_Menu, 20L);
		goto ExitMakeDisplay;
    }

  	CheckMenu (Ilbm->win);
  	UpdatePrecMenuItem (MANDChunk->PrecisionBits, MAINMENU, Ilbm);

#ifdef FM_AREXX_SUPPORT
  	SetARexxMenu (Ilbm);
#endif /* FM_AREXX_SUPPORT */

	// if (Ilbm->IFFPFlags & IFFPF_USERMODE) Ilbm->camg = Ilbm->usermodeid;
  	PALETTE[0L] = ((uint32) (1L << 8) << 16); // Palette will be always 256 colors (8 bit)
	
  	if (SAVED_POSITION) PALETTE[SAVED_POSITION] = SAVED_COMPONENT;

  	SAVED_POSITION = (3L * (1L << 8)) + 1; // Palette will be always 256 colors (8 bit)

  	SAVED_COMPONENT = PALETTE[SAVED_POSITION];
  	PALETTE[SAVED_POSITION] = NULL;

	NColors = (1L << Ilbm->Bmhd.nPlanes);
	
ExitMakeDisplay:
  	return (NColors); // return colors number
}

/* CloseDisplay() */
void CloseDisplay (struct ILBMInfo *Ilbm)
{
  	ClearMenuStrip (Ilbm->win);

#ifdef FM_AREXX_SUPPORT
  	RemoveARexxMenu (Ilbm);
	// FreeARexxMenu ();
#endif /* FM_AREXX_SUPPORT */

  	if (MMASK & MASK)
    {
      	if (MAINMENU)
    	{
      		FreeMenus (MAINMENU);
      		MAINMENU = NULL;
      		MASK &= ~MMASK;
    	}
    }

  	if (VMASK & MASK)
    {
      	if (VINFO)
    	{
      		FreeVisualInfo (VINFO);
      		VINFO = NULL;
      		MASK &= ~VMASK;
    	}
    }

  	if (WMASK & MASK)
    {
      	if (Ilbm->win)
    	{
      		CloseWindow (Ilbm->win);
      		Ilbm->win = NULL;
      		MASK &= ~WMASK;
    	}
    }

  	if (AMASK & MASK)
    {
      	if (ARGBMEM)
    	{
      		FreeVec (ARGBMEM);
      		ARGBMEM = NULL;
      		MASK &= ~AMASK;
    	}
    }

  	if (RMASK & MASK)
    {
      	if (RNDMEM)
    	{
      		FreeVec (RNDMEM);
      		RNDMEM = NULL;
      		MASK &= ~RMASK;
    	}
    }

  	if (PMASK & MASK)
    {
      	if (GFXMEM)
    	{
      		FreeVec (GFXMEM);
      		GFXMEM = NULL;
      		MASK &= ~PMASK;
    	}
    }

  	if (LMASK & MASK)
    {
      	if (PIXMEM)
    	{
      		FreeVec (PIXMEM);
      		PIXMEM = NULL;
      		MASK &= ~LMASK;
    	}
    }

  	if (SMASK & MASK)
    {
      	CloseDownDisplay (Ilbm->scr);

    	if (Ilbm->scr)
    	{
      		CloseScreen (Ilbm->scr);
      		Ilbm->scr = NULL;
      		MASK &= ~SMASK;
    	}
    }

  	if (CMASK & MASK)
    {
      	if (Ilbm->brbitmap)
    	{
      		FreeBitMapSafety (Ilbm->brbitmap);
      		Ilbm->brbitmap = NULL;
      		MASK &= ~CMASK;
    	}
    }

  	if (NEWFONT)
    {
      	CloseFont (NEWFONT);
      	NEWFONT = NULL;
    }
}

/* ShowTime() */
void ShowTime (struct Window *Win, STRPTR String, int32 Secs)
{
	/*convert seconds to h,m,s -> seconds/3600,(seconds%3600)/60,(seconds%3600)%60*/
  	if (Secs) snprintf (BAR_STRING, sizeof (BAR_STRING), CATSTR (TXT_AverageSpeed), String, Secs / 3600L,
          				(Secs % 3600L) / 60L, (Secs % 3600L) % 60L, (float64) (Win->GZZWidth * Win->GZZHeight) /
          				(float64) ((Secs % 3600L) % 60L)); // there is %.1f value can't use SNPrintf OS4 API Call
  	else
    	SNPrintf (BAR_STRING, sizeof (BAR_STRING), CATSTR (TXT_LessThanOne), String);

  	SetWindowTitles (Win, (STRPTR) ~0, BAR_STRING);
}

/* SaveCoords() */
int32 SaveCoords (struct Window *Win)
{
	if (UNDOBuffer)
    {
        if (UNDOCOUNTER < LEVELUNDO)
        {
            mpf_inits (UNDOBuffer[UNDOCOUNTER].GRMin, UNDOBuffer[UNDOCOUNTER].GRMax, 
              			  UNDOBuffer[UNDOCOUNTER].GIMin, UNDOBuffer[UNDOCOUNTER].GIMax,
                          UNDOBuffer[UNDOCOUNTER].GJKre, UNDOBuffer[UNDOCOUNTER].GJKim, 0);

            mpf_set (UNDOBuffer[UNDOCOUNTER].GRMin, MANDChunk->GRMin);
            mpf_set (UNDOBuffer[UNDOCOUNTER].GRMax, MANDChunk->GRMax);
            mpf_set (UNDOBuffer[UNDOCOUNTER].GIMin, MANDChunk->GIMin);
            mpf_set (UNDOBuffer[UNDOCOUNTER].GIMax, MANDChunk->GIMax);
            mpf_set (UNDOBuffer[UNDOCOUNTER].GJKre, MANDChunk->GJKre);
            mpf_set (UNDOBuffer[UNDOCOUNTER].GJKim, MANDChunk->GJKim);

            /* save hardware float64 */
            UNDOBuffer[UNDOCOUNTER].AreaRange[0] = MANDChunk->RMin;
            UNDOBuffer[UNDOCOUNTER].AreaRange[1] = MANDChunk->RMax;
            UNDOBuffer[UNDOCOUNTER].AreaRange[2] = MANDChunk->IMin;
            UNDOBuffer[UNDOCOUNTER].AreaRange[3] = MANDChunk->IMax;
            UNDOBuffer[UNDOCOUNTER].JuliaConst[0] = MANDChunk->JKre;
            UNDOBuffer[UNDOCOUNTER].JuliaConst[1] = MANDChunk->JKim;
      
            UNDOBuffer[UNDOCOUNTER].Iterations = MANDChunk->Iterations;
            UNDOBuffer[UNDOCOUNTER].Power = MANDChunk->Power;
            UNDOBuffer[UNDOCOUNTER].Flags = MANDChunk->Flags;
            UNDOBuffer[UNDOCOUNTER].PrecisionBits = MANDChunk->PrecisionBits;
            UNDOBuffer[UNDOCOUNTER].PrecisionDigits = MANDChunk->PrecisionDigits;

            UNDOCOUNTER++;

            CheckMenu (Win);
        }

        else
        {
        	SetWindowTitles (Win, (STRPTR) ~ 0, "Warning undo buffer full!");
            DisplayBeep (Win->WScreen);
        }
    }

    else
    {
	    SetWindowTitles (Win, (STRPTR) ~ 0, "Warning undo not available");
        DisplayBeep (Win->WScreen);
    }

    return (UNDOCOUNTER);
}

/* RestoreCoords () */
int32 RestoreCoords (struct Window *Win)
{
    if (UNDOBuffer)
    {
        if (UNDOCOUNTER > 0)
        {
            UNDOCOUNTER--;

            MANDChunk->Iterations = UNDOBuffer[UNDOCOUNTER].Iterations;
            MANDChunk->Power = UNDOBuffer[UNDOCOUNTER].Power;
            MANDChunk->Flags = UNDOBuffer[UNDOCOUNTER].Flags;
            MANDChunk->PrecisionBits = UNDOBuffer[UNDOCOUNTER].PrecisionBits;
            MANDChunk->PrecisionDigits = UNDOBuffer[UNDOCOUNTER].PrecisionDigits;
  
            /* restore hardware float64 */
            MANDChunk->RMin = UNDOBuffer[UNDOCOUNTER].AreaRange[0];
            MANDChunk->RMax = UNDOBuffer[UNDOCOUNTER].AreaRange[1];
            MANDChunk->IMin = UNDOBuffer[UNDOCOUNTER].AreaRange[2];
            MANDChunk->IMax = UNDOBuffer[UNDOCOUNTER].AreaRange[3];
            MANDChunk->JKre = UNDOBuffer[UNDOCOUNTER].JuliaConst[0];
            MANDChunk->JKim = UNDOBuffer[UNDOCOUNTER].JuliaConst[1];

            mpf_set (MANDChunk->GRMin, UNDOBuffer[UNDOCOUNTER].GRMin);
            mpf_set (MANDChunk->GRMax, UNDOBuffer[UNDOCOUNTER].GRMax);
            mpf_set (MANDChunk->GIMin, UNDOBuffer[UNDOCOUNTER].GIMin);
            mpf_set (MANDChunk->GIMax, UNDOBuffer[UNDOCOUNTER].GIMax);
            mpf_set (MANDChunk->GJKre, UNDOBuffer[UNDOCOUNTER].GJKre);
            mpf_set (MANDChunk->GJKim, UNDOBuffer[UNDOCOUNTER].GJKim);

            mpf_clears (UNDOBuffer[UNDOCOUNTER].GRMin, UNDOBuffer[UNDOCOUNTER].GRMax,
                        UNDOBuffer[UNDOCOUNTER].GIMin, UNDOBuffer[UNDOCOUNTER].GIMax,
                        UNDOBuffer[UNDOCOUNTER].GJKre, UNDOBuffer[UNDOCOUNTER].GJKim, 0);

            CheckMenu (Win);
        }

        else OffMenu (Win, FULLMENUNUM (2, 4, NOSUB));
    }

    else
    {
        SetWindowTitles (Win, (STRPTR) ~ 0, "Warning undo not available!");
        DisplayBeep (Win->WScreen);
    }

    return (UNDOCOUNTER);
}

/* NewCoords() */
int16 NewCoords (struct Window * Win, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  int16 Success = FALSE;
  mpf_t KReal, KImag;

  	if (((a2 - a1) < 2) || ((b2 - b1) < 2))
    {
      	mpf_clears (KReal, KImag, 0);
      	goto ExitNewCoords;
    }

  	mpf_inits (KReal, KImag, 0);

  	/* save coordinates */
  	SaveCoords (Win);

  	/*  KReal = fabs (->RMax-MANDChunk->RMin) / ((float64) Win->GZZWidth); */
  	mpf_sub (gtmp, MANDChunk->GRMax, MANDChunk->GRMin);
  	mpf_abs (KReal, gtmp);
  	mpf_div_ui (KReal, KReal, Win->GZZWidth);
	
  	/* KImag = fabs (MANDChunk->IMax-MANDChunk->IMin) / ((float64) Win->GZZHeight); */
  	mpf_sub (gtmp, MANDChunk->GIMax, MANDChunk->GIMin);
  	mpf_abs (KImag, gtmp);
  	mpf_div_ui (KImag, KImag, Win->GZZHeight);

  	/* MANDChunk->RMax = MANDChunk->RMin + ((float64) a2 * KReal); */
  	mpf_mul_ui (gtmp, KReal, a2);
  	mpf_add (MANDChunk->GRMax, MANDChunk->GRMin, gtmp);

  	/* MANDChunk->IMin = MANDChunk->IMax - ((float64) b2 * KImag); */
  	mpf_mul_ui (gtmp, KImag, b2);
  	mpf_sub (MANDChunk->GIMin, MANDChunk->GIMax, gtmp);

  	/* MANDChunk->RMin = MANDChunk->RMin + ((float64) a1 * KReal); */
  	mpf_mul_ui (gtmp, KReal, a1);
  	mpf_add (MANDChunk->GRMin, MANDChunk->GRMin, gtmp);

  	/* MANDChunk->IMax = MANDChunk->IMax - ((float64) b1 * KImag); */
  	mpf_mul_ui (gtmp, KImag, b1);
  	mpf_sub (MANDChunk->GIMax, MANDChunk->GIMax, gtmp);

  	/* save GMP Z SET coordnates also into float64 numbers */
  	MANDChunk->RMax = mpf_get_d (MANDChunk->GRMax);
  	MANDChunk->RMin = mpf_get_d (MANDChunk->GRMin);
  	MANDChunk->IMax = mpf_get_d (MANDChunk->GIMax);
  	MANDChunk->IMin = mpf_get_d (MANDChunk->GIMin);

  	/* clear unused local vars */
  	mpf_clears (KReal, KImag, 0);

	Success = TRUE;

ExitNewCoords:  	
	return (Success);
}

/* AdjustRatio() */
void AdjustRatio (mpf_t * RMin, mpf_t * IMax, mpf_t * RMax, mpf_t * IMin, int16 ScrWidth, int16 ScrHeight, int16 KeepReal)
{
  mpf_t RWidth, IHeight, RCenter, ICenter;

  	mpf_inits (RWidth, IHeight, RCenter, ICenter, 0);
  	mpf_sub (IHeight, *IMax, *IMin);
  	mpf_sub (RWidth, *RMax, *RMin);

  	if (KeepReal)
    {
      	mpf_div_ui (ICenter, IHeight, 2);
      	mpf_add (ICenter, *IMin, ICenter);
      	mpf_mul_ui (IHeight, RWidth, ScrHeight);
      	mpf_div_ui (IHeight, IHeight, ScrWidth);
      	mpf_div_ui (*IMax, IHeight, 2);
      	mpf_add (*IMax, *IMax, ICenter);
      	mpf_div_ui (*IMin, IHeight, 2);
      	mpf_sub (*IMin, ICenter, *IMin);
    }

  	else
    {
      	mpf_div_ui (RCenter, RWidth, 2);
      	mpf_add (RCenter, *RMin, RCenter);
      	mpf_mul_ui (RWidth, IHeight, ScrWidth);
      	mpf_div_ui (RWidth, RWidth, ScrHeight);
      	mpf_div_ui (*RMax, RWidth, 2);
      	mpf_add (*RMax, *RMax, RCenter);
      	mpf_div_ui (*RMin, RWidth, 2);
      	mpf_sub (*RMin, RCenter, *RMin);
    }

  	mpf_clears (RWidth, IHeight, RCenter, ICenter, 0);
}

/* SystemInfo() */
void SystemInfo (struct Window *Win)
{
  struct EasyStruct MyReq = { sizeof (struct EasyStruct), 0, CATSTR (TITLE_SysInfoReq), CATSTR (TXT_SysInfo), CATSTR (TXT_OK) };

  	EasyRequest (Win, &MyReq, NULL, CPUPPC_STR, VERPPC_STR, VECPPC_STR,
					AvailMem (MEMF_CHIP) / 1024 / 1024, AvailMem (MEMF_FAST) / 1024 / 1024,
           			AvailMem (MEMF_CHIP | MEMF_LARGEST) / 1024 / 1024, AvailMem (MEMF_FAST | MEMF_LARGEST) / 1024 / 1024);
}

/* About() */
int32 About (struct Window *Win)
{
  int32 More = 0L;
  struct EasyStruct MyReq_1 = { sizeof (struct EasyStruct), 0, CATSTR (TITLE_InfoReq), CATSTR (TXT_Info), CATSTR (TXT_OKMore) };
  struct EasyStruct MyReq_2 = { sizeof (struct EasyStruct), 0, CATSTR (TITLE_WorkgroupReq), CATSTR (Work_TXT_Workgroup), CATSTR (TXT_OK) };

  	if (!(More = EasyRequest (Win, &MyReq_1, NULL, VERSION, COPYRIGHT_DATE, AUTHOR, ADDRESS, EMAIL))) EasyRequest (Win, &MyReq_2, NULL);
  	
	return (More);
}

/* Fail() 
uint32 Fail (STRPTR ErrorString, uint32 ErrorLevel)
{
  	DisplayBeep (NULL);
  	Printf ("%s\n", ErrorString);
  	RETURNVALUE = ErrorLevel;

  	return (ErrorLevel);
}*/

/* Choice() */
int32 Choice (struct Window * Win, STRPTR Title, STRPTR String)
{
  struct EasyStruct MyReq = { sizeof (struct EasyStruct), NULL, 0, 0, CATSTR (TXT_YesNo) };

  	MyReq.es_Title = Title;
  	MyReq.es_TextFormat = String;

  	return (EasyRequest (Win, &MyReq, 0));
}

/* ShowCoords() */
int16 ShowCoords (struct Window * Win)
{
  struct Gadget *GadList = NULL, *StringGad_1 = NULL, *StringGad_2 = NULL, 
  		*StringGad_3 = NULL, *StringGad_4 = NULL, *StringGad_5 = NULL, *StringGad_6 = NULL;
  struct Gadget *MyButtonGad = NULL, *MyCheckBoxGad = NULL, *MyGad = NULL;
  struct Window *GadWin = NULL;
  struct IntuiMessage *Message = NULL;
  int32 Exit = FALSE, Accept = FALSE, Reset = FALSE, Ratio = FALSE, KeepReal = TRUE;
  uint8 __attribute__ ((aligned (16))) String[MAX_MATH_DIGITS + 2];
  uint16 MyCode;
  uint32 MyClass;
  mpf_t Tmp_RMIN, Tmp_IMAX, Tmp_RMAX, Tmp_IMIN, Tmp_JKRE, Tmp_JKIM;
  mpf_inits (Tmp_RMIN, Tmp_IMAX, Tmp_RMAX, Tmp_IMIN, Tmp_JKRE, Tmp_JKIM, 0);

  	mpf_set (Tmp_IMAX, MANDChunk->GIMax);
  	mpf_set (Tmp_RMIN, MANDChunk->GRMin);
  	mpf_set (Tmp_RMAX, MANDChunk->GRMax);
  	mpf_set (Tmp_IMIN, MANDChunk->GIMin);
  	mpf_set (Tmp_JKRE, MANDChunk->GJKre);
  	mpf_set (Tmp_JKIM, MANDChunk->GJKim);

  	TEXTGAD.ng_VisualInfo = BUTTONGAD.ng_VisualInfo = CHECKBOXGAD.ng_VisualInfo = VINFO;
  	StringGad_1 = CreateContext (&GadList);
  	TEXTGAD.ng_Width = (MIN_WIDTH - (MARGIN * 2)) / 3;
  	TEXTGAD.ng_TopEdge = MARGIN;
  	TEXTGAD.ng_LeftEdge = MARGIN + (((MIN_WIDTH - (MARGIN * 2)) - TEXTGAD.ng_Width) / 2);
  	TEXTGAD.ng_Height = MYFONTSTRUCT.ta_YSize * 3 / 2;
  	TEXTGAD.ng_GadgetText = CATSTR (Coord_TXT_Top);
  	TEXTGAD.ng_Flags = PLACETEXT_LEFT;
  	gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMAX);    /* write always max supported digits 1235 *//* ..only 16 digits for now */
  	StringGad_1 = CreateGadget (STRING_KIND, StringGad_1, &TEXTGAD, GTST_String,
          						String, STRINGA_ReplaceMode, TRUE, GTST_MaxChars,
          						MANDChunk->PrecisionDigits, GT_Underscore, '_', TAG_DONE);
  	TEXTGAD.ng_TopEdge += (20 + TEXTGAD.ng_Height);
  	TEXTGAD.ng_GadgetText = CATSTR (Coord_TXT_Left);
  	gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMIN);
  	StringGad_2 =  CreateGadget (STRING_KIND, StringGad_1, &TEXTGAD, GTST_String,
          						String, STRINGA_ReplaceMode, TRUE, GTST_MaxChars,
          						MANDChunk->PrecisionDigits, GT_Underscore, '_', TAG_DONE);
  	TEXTGAD.ng_TopEdge += (20 + TEXTGAD.ng_Height);
  	TEXTGAD.ng_GadgetText = CATSTR (Coord_TXT_Right);
  	TEXTGAD.ng_Flags = PLACETEXT_RIGHT;
  	gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMAX);
  	StringGad_3 = CreateGadget (STRING_KIND, StringGad_2, &TEXTGAD, GTST_String,
          						String, STRINGA_ReplaceMode, TRUE, GTST_MaxChars,
          						MANDChunk->PrecisionDigits, GT_Underscore, '_', TAG_DONE);
  	TEXTGAD.ng_TopEdge += (20 + TEXTGAD.ng_Height);
  	TEXTGAD.ng_GadgetText = CATSTR (Coord_TXT_Bottom);
  	gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMIN);
  	StringGad_4 = CreateGadget (STRING_KIND, StringGad_3, &TEXTGAD, GTST_String,
          						String, STRINGA_ReplaceMode, TRUE, GTST_MaxChars,
          						MANDChunk->PrecisionDigits, GT_Underscore, '_', TAG_DONE);
  	TEXTGAD.ng_TopEdge += (40 + TEXTGAD.ng_Height);
  	TEXTGAD.ng_GadgetText = CATSTR (Coord_TXT_JuliaReal);
  	TEXTGAD.ng_Flags = PLACETEXT_LEFT;
  	gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_JKRE);
  	StringGad_5 = CreateGadget (STRING_KIND, StringGad_4, &TEXTGAD, GTST_String,
          						String, STRINGA_ReplaceMode, TRUE, GTST_MaxChars,
          						MANDChunk->PrecisionDigits, GT_Underscore, '_', TAG_DONE);
  	TEXTGAD.ng_TopEdge += (20 + TEXTGAD.ng_Height);
  	TEXTGAD.ng_GadgetText = CATSTR (Coord_TXT_JuliaImag);
  	gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_JKIM);
  	StringGad_6 = CreateGadget (STRING_KIND, StringGad_5, &TEXTGAD, GTST_String,
          						String, STRINGA_ReplaceMode, TRUE, GTST_MaxChars,
          						MANDChunk->PrecisionDigits, GT_Underscore, '_', TAG_DONE);
  	CHECKBOXGAD.ng_LeftEdge = MARGIN + 100;
  	CHECKBOXGAD.ng_TopEdge = TEXTGAD.ng_TopEdge + TEXTGAD.ng_Height + 30;
  	CHECKBOXGAD.ng_Width = MYFONTSTRUCT.ta_YSize * 3 / 2;
  	CHECKBOXGAD.ng_Height = MYFONTSTRUCT.ta_YSize * 3 / 2;
  	CHECKBOXGAD.ng_GadgetText = CATSTR (Coord_TXT_KeepAxis);
  	CHECKBOXGAD.ng_Flags = PLACETEXT_RIGHT;
  	CHECKBOXGAD.ng_GadgetID = KEEP;
  	MyCheckBoxGad = CreateGadget (CHECKBOX_KIND, StringGad_6, &CHECKBOXGAD, GTCB_Checked,
          							TRUE, GTCB_Scaled, TRUE, GT_Underscore, '_', TAG_DONE);
  	BUTTONGAD.ng_LeftEdge = MARGIN;
  	BUTTONGAD.ng_TopEdge = CHECKBOXGAD.ng_TopEdge + CHECKBOXGAD.ng_Height + 30;
  	BUTTONGAD.ng_Width = (MIN_WIDTH - (MARGIN * 7)) / 4;
  	BUTTONGAD.ng_Height = MYFONTSTRUCT.ta_YSize * 3 / 2;
  	BUTTONGAD.ng_GadgetText = CATSTR (Coord_TXT_Accept);
  	BUTTONGAD.ng_GadgetID = ACCEPT;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyCheckBoxGad, &BUTTONGAD, GT_Underscore, '_', TAG_DONE);
  	BUTTONGAD.ng_LeftEdge += (MARGIN + BUTTONGAD.ng_Width);
  	BUTTONGAD.ng_GadgetText = CATSTR (Coord_TXT_Ratio);
  	BUTTONGAD.ng_GadgetID = RATIO;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTONGAD, GT_Underscore, '_', TAG_DONE);
  	BUTTONGAD.ng_LeftEdge += (MARGIN + BUTTONGAD.ng_Width);
  	BUTTONGAD.ng_GadgetText = CATSTR (Coord_TXT_Default);
  	BUTTONGAD.ng_GadgetID = RESET;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTONGAD, GT_Underscore, '_', TAG_DONE);
  	BUTTONGAD.ng_LeftEdge += (MARGIN + BUTTONGAD.ng_Width);
  	BUTTONGAD.ng_GadgetText = CATSTR (Coord_TXT_Cancel);
  	BUTTONGAD.ng_GadgetID = CANCEL;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTONGAD, GT_Underscore, '_', TAG_DONE);

  	if (MyButtonGad)
    {
      	GadWin = OpenWindowTags (NULL,
        			WA_Left, MARGIN,
                   	WA_Top, MARGIN,
                   	WA_Width, MIN_WIDTH - (MARGIN * 2),
                   	WA_Height, MIN_HEIGHT - (MARGIN * 2),
                   	WA_Title, CATSTR (TITLE_CoordReq),
                   	WA_ScreenTitle,	CATSTR (TXT_ScrTitle_Coord),
                   	WA_CustomScreen, Win->WScreen, 
					WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_VANILLAKEY|STRINGIDCMP|BUTTONIDCMP, 
					WA_Flags, WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_SIMPLE_REFRESH|WFLG_GIMMEZEROZERO|WFLG_RMBTRAP|WFLG_NW_EXTENDED,
                   	WA_Gadgets, GadList, 
					WA_StayTop, TRUE,
                   	WA_DropShadows, TRUE, 
					TAG_DONE);

    	if (GadWin)
    	{
      		GT_RefreshWindow (GadWin, NULL);

      		do
        	{
          		WaitPort (GadWin->UserPort);

          		while ((!Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (GadWin->UserPort)))
        		{
          			MyGad = (struct Gadget *) Message->IAddress;
          			MyClass = Message->Class;
          			MyCode = Message->Code;
          			GT_ReplyIMsg ((struct IntuiMessage *) Message);

          			switch (MyClass)
          			{
            			case IDCMP_REFRESHWINDOW:
              			{
							GT_BeginRefresh (GadWin);
              				GT_EndRefresh (GadWin, TRUE);
              			}
						break;

            			case IDCMP_VANILLAKEY:
              			{
							switch (ToUpper ((uint32) MyCode))
            				{
            					case 'L':
              						ActivateGadget (StringGad_1, GadWin, 0);
              					break;
            					
								case 'O':
								    ActivateGadget (StringGad_2, GadWin, 0);
              					break;
            					
								case 'R':
              						ActivateGadget (StringGad_3, GadWin, 0);
              					break;
            					
								case 'T':
              						ActivateGadget (StringGad_4, GadWin, 0);
              					break;
            					
								case 'J':
            						ActivateGadget (StringGad_5, GadWin, 0);
              					break;
            					
								case 'U':
              						ActivateGadget (StringGad_6, GadWin, 0);
              					break;
            					
								case 'A':
              						Accept = TRUE;
              					break;
            					
								case 'I':
              						Ratio = TRUE;
              					break;
            					
								case 'D':
              						Reset = TRUE;
              					break;
            					
								case 'X':
              						KeepReal = !KeepReal;
              						GT_SetGadgetAttrs (MyCheckBoxGad, GadWin, NULL, GTCB_Checked, KeepReal, TAG_DONE);
              					break;
            					
								case 'C':
           						case VAN_ESC:
              						Exit = TRUE;
								break;
            				}
              			}
						break;

            			case IDCMP_GADGETUP:
              			{
							switch (MyGad->GadgetID)
            				{
            					case ACCEPT:
	              					Accept = TRUE;
	              				break;
	            				
								case RATIO:
	              					Ratio = TRUE;
	              				break;
	            				
								case RESET:
	              					Reset = TRUE;
	              				break;
	            				
								case KEEP:
	              					KeepReal = !KeepReal;
	              				break;
	            				
								case CANCEL:
	              					Exit = TRUE;
								break;
            				}
              			}
						break;

            			case IDCMP_CLOSEWINDOW:
              				Exit = TRUE;
						break;							
            		}

          			if (Accept)
            		{
              			mpf_set_str (Tmp_IMAX, &((struct StringInfo *) StringGad_1->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_RMIN, &((struct StringInfo *) StringGad_2->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_RMAX, &((struct StringInfo *) StringGad_3->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_IMIN, &((struct StringInfo *) StringGad_4->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_JKRE, &((struct StringInfo *) StringGad_5->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_JKIM, &((struct StringInfo *) StringGad_6->SpecialInfo)->Buffer, 10);

              			if ((mpf_cmp (Tmp_RMIN, Tmp_RMAX) >= 0) || (mpf_cmp (Tmp_IMIN, Tmp_IMAX)) >= 0) DisplayBeep (Win->WScreen);
              			else
            			{
              				SaveCoords (Win);

              				mpf_set (MANDChunk->GIMax, Tmp_IMAX);
              				mpf_set (MANDChunk->GRMin, Tmp_RMIN);
              				mpf_set (MANDChunk->GRMax, Tmp_RMAX);
              				mpf_set (MANDChunk->GIMin, Tmp_IMIN);
              				mpf_set (MANDChunk->GJKre, Tmp_JKRE);
              				mpf_set (MANDChunk->GJKim, Tmp_JKIM);

              				/* save coords even in float64 numbers for compatibility */
              				MANDChunk->IMax = mpf_get_d (Tmp_IMAX);
              				MANDChunk->RMin = mpf_get_d (Tmp_RMIN);
              				MANDChunk->RMax = mpf_get_d (Tmp_RMAX);
              				MANDChunk->IMin = mpf_get_d (Tmp_IMIN);
              				MANDChunk->JKre = mpf_get_d (Tmp_JKRE);
              				MANDChunk->JKim = mpf_get_d (Tmp_JKIM);

              				Exit = TRUE;
            			}
            		}

          			if (Ratio)
            		{
              			mpf_set_str (Tmp_IMAX, &((struct StringInfo *) StringGad_1->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_RMIN, &((struct StringInfo *) StringGad_2->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_RMAX, &((struct StringInfo *) StringGad_3->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_IMIN, &((struct StringInfo *) StringGad_4->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_JKRE, &((struct StringInfo *) StringGad_5->SpecialInfo)->Buffer, 10);
              			mpf_set_str (Tmp_JKIM, &((struct StringInfo *) StringGad_6->SpecialInfo)->Buffer, 10);

              			if ((mpf_cmp (Tmp_RMIN, Tmp_RMAX) >= 0) || (mpf_cmp (Tmp_IMIN, Tmp_IMAX)) >= 0) DisplayBeep (Win->WScreen);
              			else AdjustRatio (&Tmp_RMIN, &Tmp_IMAX, &Tmp_RMAX, &Tmp_IMIN, Win->GZZWidth, Win->GZZHeight, KeepReal);

              			TEXTGAD.ng_Flags = PLACETEXT_LEFT;
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMAX);
              			GT_SetGadgetAttrs (StringGad_1, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMIN);
              			GT_SetGadgetAttrs (StringGad_2, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMAX);
              			TEXTGAD.ng_Flags = PLACETEXT_RIGHT;
              			GT_SetGadgetAttrs (StringGad_3, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMIN);
              			GT_SetGadgetAttrs (StringGad_4, GadWin, NULL, GTST_String, String, TAG_DONE);
              			Ratio = FALSE;
            		}

          			if (Reset)
            		{
              			mpf_set_d (Tmp_IMAX, DEF_IMAX);
              			mpf_set_d (Tmp_RMIN, DEF_RMIN);
              			mpf_set_d (Tmp_RMAX, DEF_RMAX);
              			mpf_set_d (Tmp_IMIN, DEF_IMIN);
              			mpf_set_d (Tmp_JKRE, DEF_JKRE);
              			mpf_set_d (Tmp_JKIM, DEF_JKIM);
	
              			TEXTGAD.ng_Flags = PLACETEXT_LEFT;
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMAX);
              			GT_SetGadgetAttrs (StringGad_1, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMIN);
              			GT_SetGadgetAttrs (StringGad_2, GadWin, NULL, GTST_String, String, TAG_DONE);
              			TEXTGAD.ng_Flags = PLACETEXT_RIGHT;
						gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_RMAX);
              			GT_SetGadgetAttrs (StringGad_3, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_IMIN);
              			GT_SetGadgetAttrs (StringGad_4, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_JKRE);
						GT_SetGadgetAttrs (StringGad_5, GadWin, NULL, GTST_String, String, TAG_DONE);
              			gmp_snprintf (String, sizeof (String), "%+2.1235Ff", Tmp_JKIM);
              			GT_SetGadgetAttrs (StringGad_6, GadWin, NULL, GTST_String, String, TAG_DONE);
              			Reset = FALSE;
            		}
        		}
       		} while (!Exit);

   			CloseWindow (GadWin);
   		}

   		else DisplayError (NULL, TXT_ERR_WindowGadget, 15L);
   	}
  		
	else DisplayError (NULL, TXT_ERR_Gadget, 15L);

  	FreeGadgets (GadList);
  	mpf_clears (Tmp_RMIN, Tmp_IMAX, Tmp_RMAX, Tmp_IMIN, Tmp_JKRE, Tmp_JKIM, 0);
  	
	return (Accept);
}

/* IntegerGad() */
uint32 IntegerGad (struct Window * Win, STRPTR TitleWin, STRPTR TitleScr, STRPTR TxtString, uint32 Var)
{
  struct IntuiMessage *Message = NULL;
  struct Gadget *GadList = NULL, *MyIntGad = NULL, *MyButtonGad = NULL, *MyGad = NULL;
  struct Window *GadWin = NULL;
  int16 Exit = FALSE;
  uint16 MyCode;
  uint32 ReturnValue = Var;
  uint32 MyClass;

  	TEXTGAD.ng_VisualInfo = BUTTONGAD.ng_VisualInfo = VINFO;
  	MyIntGad = CreateContext (&GadList);
  	TEXTGAD.ng_LeftEdge = 245;
  	TEXTGAD.ng_TopEdge = 20;
  	TEXTGAD.ng_Width = 130;
  	TEXTGAD.ng_Height = (MYFONTSTRUCT.ta_YSize * 3L) / 2;
  	TEXTGAD.ng_GadgetText = TxtString;
  	TEXTGAD.ng_Flags = PLACETEXT_LEFT;
  	MyIntGad = CreateGadget (INTEGER_KIND, MyIntGad, &TEXTGAD, GTIN_Number, Var,
          						GTIN_MaxChars, 12, STRINGA_ReplaceMode, TRUE,
          						GT_Underscore, '_', TAG_DONE);
  	BUTTONGAD.ng_LeftEdge = 55;
  	BUTTONGAD.ng_TopEdge = TEXTGAD.ng_TopEdge + TEXTGAD.ng_Height + 30;
  	BUTTONGAD.ng_Width = 130;
  	BUTTONGAD.ng_Height = (MYFONTSTRUCT.ta_YSize * 3L) / 2;
  	BUTTONGAD.ng_GadgetText = CATSTR (Cyc_TXT_Accept);
  	BUTTONGAD.ng_GadgetID = ACCEPT;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyIntGad, &BUTTONGAD, GT_Underscore, '_', TAG_DONE);
  	BUTTONGAD.ng_LeftEdge += (60 + BUTTONGAD.ng_Width);
  	BUTTONGAD.ng_GadgetText = CATSTR (Cyc_TXT_Cancel);
  	BUTTONGAD.ng_GadgetID = CANCEL;
  	MyButtonGad = CreateGadget (BUTTON_KIND, MyButtonGad, &BUTTONGAD, GT_Underscore, '_', TAG_DONE);

  	if (MyButtonGad)
    {
      	GadWin = OpenWindowTags (NULL,
        			WA_Left, WINDOW_X_OFFSET,
                   	WA_Top, WINDOW_Y_OFFSET,
                   	WA_Width, BUTTONGAD.ng_LeftEdge + BUTTONGAD.ng_Width + 90, 
					WA_Height, BUTTONGAD.ng_TopEdge + BUTTONGAD.ng_Height + 55, 
					WA_Title, TitleWin, 
					WA_ScreenTitle, TitleScr,
                   	WA_CustomScreen, Win->WScreen, 
					WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_VANILLAKEY|INTEGERIDCMP|BUTTONIDCMP, 
					WA_Flags, WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_SIMPLE_REFRESH|WFLG_GIMMEZEROZERO|WFLG_RMBTRAP|WFLG_NW_EXTENDED,
                   	WA_Gadgets, GadList, 
					WA_StayTop, TRUE,
                   	WA_DropShadows, TRUE, 
					TAG_DONE);

    	if (GadWin)
    	{
      		GT_RefreshWindow (GadWin, NULL);

      		do
        	{
          		WaitPort (GadWin->UserPort);

          		while ((!Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (GadWin->UserPort)))
        		{
          			MyGad = (struct Gadget *) Message->IAddress;
          			MyClass = Message->Class;
          			MyCode = Message->Code;
          			GT_ReplyIMsg ((struct IntuiMessage *) Message);

          			switch (MyClass)
            		{
            			case IDCMP_REFRESHWINDOW:
              			{
							GT_BeginRefresh (GadWin);
              				GT_EndRefresh (GadWin, TRUE);
              			}
						break;
            			
						case IDCMP_VANILLAKEY:
              			{
							switch (ToUpper ((uint32) MyCode))
            				{
            					case 'E':
            					case 'I':
              						ActivateGadget (MyIntGad, GadWin, 0);
              					break;
            					
								case 'A':
              						ReturnValue = ((struct StringInfo *) MyIntGad->SpecialInfo)->LongInt;
            					break;
								
								case 'C':
            					case VAN_ESC:
              						Exit = TRUE;
            					break;
							}
						}	
              			break;
           				
						case IDCMP_GADGETUP:
              			{
							switch (MyGad->GadgetID)
            				{
            					case ACCEPT:
              						ReturnValue = ((struct StringInfo *) MyIntGad->SpecialInfo)->LongInt;
              						Exit = TRUE;
              					break;
								
            					case CANCEL:	
              						Exit = TRUE;
								break;	
            				}
						}	
              			break;

            			case IDCMP_CLOSEWINDOW:
              			{
							Exit = TRUE;
						}
						break;
            		}
        		}

        	} while (!Exit);

      		CloseWindow (GadWin);
    	}

      	else DisplayError (NULL, TXT_ERR_Window, 15L);
    }

  	else DisplayError (NULL, TXT_ERR_Gadget, 15L);

  	FreeGadgets (GadList);
  	
	return (ReturnValue);
}

	/* Normalization formula to linearly rescale data values once having observed
   	Min and Max, into a new arbitrary range from NewMin to NewMax */
	/* NewValue = (NewMax - NewMin) / (Max - Min) * (Value - Max) + NewMax;
   	or  NewValue = (NewMax - NewMin) / (Max - Min) * (Value - Min) + NewMin; */
	/* To speedup calcs use:
   	a = (NewMax - NewMin) / (Max - Min); b = NewMax - a * Max; NewValue = a * Value + b;
   	note: for b you can use also b = NewMin - (a * Min); */

uint32 LinearRemap (const float64 Value, const float64 Min, const float64 Max,
         			const float64 NewMin, const float64 NewMax)
{   /* linear */
  	return (lround ((NewMax - NewMin) / (Max - Min) * (Value - Min) + NewMin));
}

uint32 LogRemap (const float64 Value, const float64 Min, const float64 Max,
      				const float64 NewMin, const float64 NewMax)
{   /* log (x) */
  	return (lround ((NewMax - NewMin) / (log10 (Max) - log10 (Min)) * (log10 (Value) - log10 (Min)) + NewMin));
}

uint32 RepeatedRemap (const float64 Value, const float64 Min, const float64 Max,
       					const float64 NewMin, const float64 NewMax)
{   /* modulo (x) */
  	return (lround (fmod (Value, (NewMax - NewMin + 1.0)) + NewMin));    /* (x % 251) -> 0<=x<=250 */
}

uint32 SquareRootRemap (const float64 Value, const float64 Min, const float64 Max, const float64 NewMin, const float64 NewMax)
{   /* sqrt (x) */
  	return (lround ((NewMax - NewMin) / (sqrt (Max) - sqrt (Min)) * (sqrt (Value) - sqrt (Min)) + NewMin));
}

uint32 OneRemap (const float64 Value, const float64 Min, const float64 Max, const float64 NewMin, const float64 NewMax)
{   /* x? */
  	return (lround (((NewMax - NewMin) / ((Max * Max) - (Min * Min)) * ((Value * Value) - (Min * Min)) + NewMin)));
}

uint32 TwoRemap (const float64 Value, const float64 Min, const float64 Max,
       			const float64 NewMin, const float64 NewMax)
{   /* sqrt (x?) */
  	return (lround ((NewMax - NewMin) / (sqrt (Max * Max * Max) - sqrt (Min * Min * Min)) * (sqrt (Value * Value * Value) - sqrt (Min * Min * Min)) + NewMin));
}

uint32 ThreeRemap (const float64 Value, const float64 Min, const float64 Max,
      				 const float64 NewMin, const float64 NewMax)
{   /* sqrt(log10(x?)) */
  	return (lround ((NewMax - NewMin) / (sqrt (log10 (Max * Max)) - sqrt (log10 (Min * Min))) * sqrt (log10 ((Value * Value)) - sqrt (log10 (Min * Min))) + NewMin));
}

uint32 FourRemap (const float64 Value, const float64 Min, const float64 Max,
      				 const float64 NewMin, const float64 NewMax)
{   /* small grayscale based un default GUI colors */
  	return (((uint32) Value % 3) + 1);
}

/* CheckBox() */
int16 CheckBox (struct RastPort *Rp, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  const uint32 Color = ReadPixelColor (Rp, a1, b1);
  int16 Index;

    if (Color != ReadPixelColor (Rp, a2, b2)) return FALSE; // for speed reasons exit now no jumps at end of function!
    if (Color != ReadPixelColor (Rp, a2, b1)) return FALSE;
    if (Color != ReadPixelColor (Rp, a1, b2)) return FALSE;
    if (Color != ReadPixelColor (Rp, (a1 + a2) / 2, (b1 + b2) / 2)) return FALSE; /* check also center point */

    for (Index = a1 + 1; Index < a2; Index++)
  	{
        if (Color != ReadPixelColor (Rp, Index, b1)) return FALSE;
        if (Color != ReadPixelColor (Rp, Index, b2)) return FALSE;
  	}

	for (Index = b1 + 1; Index < b2; Index++)
    {
    	if (Color != ReadPixelColor (Rp, a1, Index)) return FALSE;
        if (Color != ReadPixelColor (Rp, a2, Index)) return FALSE;
    }

    return TRUE;
}

/* RectangleDraw() */
int16 RectangleDraw (struct MandelChunk * MandelInfo, struct Window * Win, uint8 * ARGBMem, uint8 * PixMem, uint8 * GfxMem,
                    	uint32 * PixelVecBase, uint32 * RenderMem, uint32 * HistogramMem, const int16 a1, const int16 b1, const int16 a2, const int16 b2)
{
  struct IntuiMessage *Message = NULL;
  int16 helpx, helpy, halfx, halfy;
  uint16 MyCode;
  uint32 MyClass, ColorBox, Color;
	
	helpy = b2 - b1; // catch edge case, if difference is equal to 1 we have a line already computed 
    if (helpy < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!	
    helpx = a2 - a1; // catch edge case, if difference is equal to 1 we have a line already computed 
	if (helpx < MINLIMIT) return FALSE; // for speed reasons exit now no jumps at end of function!

    if (Win->UserPort->mp_SigBit)
    {
        while (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort))
        {
        	MyClass = Message->Class;
            MyCode = Message->Code;
            GT_ReplyIMsg ((struct IntuiMessage *) Message);

            switch (MyClass) // for speed reasons return without jumps at end of function!
            {
				case IDCMP_MENUPICK:
              	{
					if (MyCode != MENUNULL)
                	{
                   		if (ProcessMenu (MandelInfo, Win, ARGBMem, PixMem, PixelVecBase, RenderMem, GfxMem, MyCode) & STOP_MSG) return TRUE;
                	}
				}	
              	break;

              	case IDCMP_RAWKEY:
                {
					if (MyCode == RAW_TAB)
                	{
                    	BlinkRect (Win, a1, b1, a2, b2);
                    	return FALSE;
                	}

                	if (MyCode == RAW_ESC)
                	{
                    	DisplayBeep (Win->WScreen);
                    	return TRUE;
                	}
				}	
              	break;

            	case IDCMP_CLOSEWINDOW:
             	{
					return TRUE;
				}
				break;				
            }
        }
    }

	if ((helpx <= MINLIMIT2) && (helpy <= MINLIMIT2)) // intercept edge cases up to 4x4 (MINLIMIT2 == 3) pixels block and return
	{
		for (helpy = b1 + 1; helpy < b2; helpy++)
			for (helpx = a1 + 1; helpx < a2; helpx++)
			    (*C_POINT) (MandelInfo, Win->RPort, PixelVecBase, helpx, helpy); /* draw center points */
 	
		return FALSE; 
	}

    halfx = (a1 + a2) / 2; // x center point coords
    halfy = (b1 + b2) / 2; // y center point coords

    (*C_POINT) (MandelInfo, Win->RPort, PixelVecBase, halfx, halfy); /* draw center point */

    if (CheckBox (Win->RPort, a1, b1, a2, b2))
    {
        RectFillColor (Win->RPort, a1 + 1, b1 + 1, a2 - 1, b2 - 1, ReadPixelColor (Win->RPort, a2, b1));
        return FALSE;
    }

    (*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, a1 + 1, halfx - 1, halfy); // don't recalc center point
    (*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, b1 + 1, halfy - 1, halfx); // don't recalc center point

    if (RectangleDraw (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, b1, halfx, halfy)) return TRUE; // rectangle 1 sx upper
    (*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, halfx + 1, a2 - 1, halfy);

    if (RectangleDraw (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, b1, a2, halfy)) return TRUE; // rectangle 2 dx upper
    (*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, halfy + 1, b2 - 1, halfx);

    if (RectangleDraw (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, a1, halfy, halfx, b2)) return TRUE; // rectangle 3 sx bottom
    if (RectangleDraw (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, halfx, halfy, a2, b2)) return TRUE; // rectangle 4 dx bottom

    return FALSE;
}

/* CalcFractal() */
void CalcFractal (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *PixMem, uint8 *GfxMem,
         			uint32 *PixelVecBase, uint32 *RenderMem, uint32 *HistogramMem)
{
	if (MandelInfo->Flags & JULIA_BIT)
  	{
      	if (GetBitMapAttr (Win->RPort->BitMap,BMA_DEPTH) == MAX_DEPTH)
      	{
          	C_POINT = JCPoint24bit;
          	H_LINE = JHLine24bit;
          	V_LINE = JVLine24bit;
      	}
      
      	else
      	{
           	C_POINT = JCPoint;
            H_LINE = JHLine;
          	V_LINE = JVLine;
      	} 
  	}

  	else if (MandelInfo->Flags & MANDEL_BIT)
  	{  
           	if (GetBitMapAttr (Win->RPort->BitMap,BMA_DEPTH) == MAX_DEPTH)
        	{
          		C_POINT = MCPoint24bit;
           		H_LINE = MHLine24bit;
           		V_LINE = MVLine24bit;
          	}
      
          	else
          	{
              	C_POINT = MCPoint;
              	H_LINE = MHLine;
           		V_LINE = MVLine;
        	}   
  	}
    
  	if (MandelInfo->Flags & LINEAR_BIT) COLORREMAP = LinearRemap;
  	else if (MandelInfo->Flags & LOG_BIT) COLORREMAP = LogRemap;
  		else if (MandelInfo->Flags & REPEATED_BIT) COLORREMAP = RepeatedRemap;
  			else if (MandelInfo->Flags & SQUARE_BIT) COLORREMAP = SquareRootRemap;
  				else if (MandelInfo->Flags & ONE_BIT) COLORREMAP = OneRemap;
  					else if (MandelInfo->Flags & TWO_BIT) COLORREMAP = TwoRemap;
  						else if (MandelInfo->Flags & THREE_BIT) COLORREMAP = ThreeRemap;
  							else if (MandelInfo->Flags & FOUR_BIT) COLORREMAP = FourRemap;

    // draw perimeter
  	(*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->TopEdge);
  	(*H_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->LeftEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
  	(*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->LeftEdge);
  	(*V_LINE) (MandelInfo, Win->RPort, PixMem, PixelVecBase, MandelInfo->TopEdge + 1, MandelInfo->Height - 2, MandelInfo->Width - 1);
	// start divide et impera recursively!
  	RectangleDraw (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVecBase, RenderMem, HistogramMem, MandelInfo->LeftEdge,
         			MandelInfo->TopEdge, MandelInfo->Width - 1, MandelInfo->Height - 1);
}

/* DrawFractal() */
uint32 DrawFractal (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *ARGBMem, uint8 *PixMem, uint8 *GfxMem,
         uint32 *PixelVector, uint32 *RndMem, int16 BeepWhenReady)
{
  uint32 StartSecs = NULL, EndSecs = NULL, DummyMicros = NULL;
  uint32 *HistMem = NULL;

	ShowTitle (Win->WScreen, FALSE);

  	if (! (HistMem = AllocVecTags (sizeof (uint32) * (MandelInfo->Iterations + 1), AVT_Type, MEMF_PRIVATE, 
									AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
									AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE)))
    {
      	DisplayError (Win, TXT_ERR_NoMem, 20L);
      	goto ExitDrawFractal; // return 0
    }

	/* define new drawing limits */
  	MandelInfo->Width = Win->GZZWidth;
  	MandelInfo->Height = Win->GZZHeight;
	MandelInfo->Modulo = Win->GZZWidth;
	
//	LOCK = LockBitMapTags (Win->RPort->BitMap,LBM_BytesPerRow,&MandelInfo->Modulo,LBM_PixelFormat,&MandelInfo->PixelFormat,TAG_DONE);
//	UnlockBitMap (LOCK);

  	CurrentTime (&StartSecs, &DummyMicros); // start measuring time
		
  	/* copy Z coordinates into global vars */
  	mpf_set (gimin, MandelInfo->GIMin);
  	mpf_set (grmin, MandelInfo->GRMin);
  	mpf_set (gimax, MandelInfo->GIMax);
  	mpf_set (grmax, MandelInfo->GRMax);
  	mpf_set (gjkre, MandelInfo->GJKre);
  	mpf_set (gjkim, MandelInfo->GJKim);

  	/* gincremimag = (fabs (MandelInfo->IMax - MandelInfo->IMin)) / ((float64) (MandelInfo->Height)); */
  	mpf_sub (gtmp, gimax, gimin);
  	mpf_abs (gincremimag, gtmp);
  	mpf_div_ui (gincremimag, gincremimag, MandelInfo->Height);

  	/* gincremreal = (fabs (MandelInfo->RMax - MandelInfo->RMin)) / ((float64) (MandelInfo->Width)); */
  	mpf_sub (gtmp, grmax, grmin);
  	mpf_abs (gincremreal, gtmp);
  	mpf_div_ui (gincremreal, gincremreal, MandelInfo->Width);

  	if (MandelInfo->Flags & TURBO_BIT)
    {
      	// fill memory with zero value -> belong mandelbrot set case
      	// if ((MandelInfo->Depth == MAX_DEPTH) && (AMASK & MASK)) 
		// memset (ARGBMem, 0, (MandelInfo->Width * MandelInfo->Height) * 4);
      	// memset (GfxMem, 0, (MandelInfo->Width * MandelInfo->Height));
      	// memset (RndMem, 0L, sizeof (uint32) * (MandelInfo->Width * MandelInfo->Height));

      	ClearMem (ARGBMem, (MandelInfo->Width * MandelInfo->Height) * 4);
      	ClearMem (GfxMem, (MandelInfo->Width * MandelInfo->Height));
      	ClearMem (RndMem, sizeof (uint32) * (MandelInfo->Width * MandelInfo->Height));

      	if (MandelInfo->Flags & HIGHPREC_BIT) CalcFractalMem_GMP (MandelInfo, RndMem, HistMem);
      	else CalcFractalMem (MandelInfo, PixelVector, RndMem, HistMem);

      	if ((MandelInfo->Flags & HISTOGRAM_BIT) && (MandelInfo->Depth == MIN_DEPTH)) Histogram (MandelInfo, Win, GfxMem, RndMem, HistMem);
      	else DisplayRndMem (MandelInfo, Win, RndMem, GfxMem);
    }

  	else
    {
    	if (MandelInfo->Flags & HIGHPREC_BIT) CalcFractal_GMP (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVector, RndMem, HistMem);
      	else CalcFractal (MandelInfo, Win, ARGBMem, PixMem, GfxMem, PixelVector, RndMem, HistMem);
    }

  	CurrentTime (&EndSecs, &DummyMicros); // stop timer
  	if (HistMem) FreeVec (HistMem);
  	if (TMASK & MASK) ShowTitle (Win->WScreen, TRUE);
  	if (BeepWhenReady == TRUE) DisplayBeep (Win->WScreen);

ExitDrawFractal:
  	return (EndSecs - StartSecs); /* return only seconds, really no need to be more accurate! */
}

/* LandscapeRender */
/*
void LandscapeRender (struct MandelChunk *MandelInfo, struct Window *Win, uint32 *RndMem, uint8 *GfxMem)
{
  uint32 Rows, Cols, Color;
  int16 *PolyMem = NULL, *PolyIndex = NULL;
  
  if (! (PolyMem = AllocVecTags (sizeof (int16) * (MandelInfo->Width * MandelInfo->Height), AVT_Type, MEMF_PRIVATE, 
									AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
									AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE))) break;

	if (MandelInfo->Depth == MIN_DEPTH)
    {	
		PolyIndex = PolyMem;
		
      	for (Cols = MandelInfo->Height; Cols > MandelInfo->TopEdge; Cols--)
    	{
      		for (Rows = MandelInfo->LeftEdge; Rows < MandelInfo->Width; Rows++)
        	{
          		*PolyIndex++ = Cols;
				*PolyIndex++ = - (*(RndMem + (Cols * MandelInfo->Width + Rows)));
			}
			
			PolyIndex = PolyMem + (sizeof (int16) * (MandelInfo->Width * MandelInfo->Height));
			
			if (Cols < MandelInfo->Height)
			{
				for (Rows = MandelInfo->LeftEdge; Rows < MandelInfo->Width; Rows++)
        		{
        			PolyIndex-- = Rows;
					PolyIndex-- = - (*(RndMem + (Cols * MandelInfo->Width + Rows)));
				}
				
				PolyFill (Win->RPort, MandelInfo->Width * MandelInfo->Height, PolyMem);
			}
			
			else PolyDraw (Win->RPort, MandelInfo->Width * MandelInfo->Height, PolyMem);
		}
	}		
}*/

/* DisplayRndmem() */
void DisplayRndMem (struct MandelChunk *MandelInfo, struct Window *Win, uint32 *RndMem, uint8 *GfxMem)
{
  uint32 Rows, Cols, Color;

  	if (MandelInfo->Depth == MAX_DEPTH)
    {
      	for (Cols = MandelInfo->TopEdge; Cols < MandelInfo->Height * 4; Cols += 4)
    	{
      		for (Rows = MandelInfo->LeftEdge; Rows < MandelInfo->Width * 4; Rows += 4)
        	{
          		if (Color = *(RndMem + ((Cols / 4) * MandelInfo->Width + (Rows / 4))))
        		{
          			*(ARGBMEM + (Cols * MandelInfo->Width + Rows + 1)) = (uint8) lround ((sin (0.016 * (float64) Color + 0.20) * 127.5 + 127.5));
          			*(ARGBMEM + (Cols * MandelInfo->Width + Rows + 2)) = (uint8) lround ((sin (0.013 * (float64) Color + 0.15) * 127.5 + 127.5));
          			*(ARGBMEM + (Cols * MandelInfo->Width + Rows + 3)) = (uint8) lround ((sin (0.010 * (float64) Color + 0.10) * 127.5 + 127.5));
        		}
        	}
    	}

      	WritePixelArray (ARGBMEM, 0, 0, MandelInfo->Modulo * 4, PIXF_A8R8G8B8, 
							Win->RPort, MandelInfo->LeftEdge, MandelInfo->TopEdge, MandelInfo->Width, MandelInfo->Height);
    }

  	else if (MandelInfo->Depth == MIN_DEPTH)
    {
      	for (Cols = MandelInfo->TopEdge; Cols < MandelInfo->Height; Cols++)
    	{
      		for (Rows = MandelInfo->LeftEdge; Rows < MandelInfo->Width; Rows++)
        	{
          		if (Color = *(RndMem + (Cols * MandelInfo->Width + Rows)))
        		{
          			Color = COLORREMAP ((float64) Color, 1.0, (float64) MandelInfo->Iterations, 4.0, 255.0);
          			*(GfxMem + (Cols * MandelInfo->Width + Rows)) = (uint8) Color;
        		}
        	}
    	}

      	WriteChunkyPixels (Win->RPort, MandelInfo->LeftEdge, MandelInfo->TopEdge, 
							MandelInfo->Width - 1, MandelInfo->Height - 1, GfxMem, MandelInfo->Modulo);
    }
}

/*  Histogram() */
void Histogram (struct MandelChunk *MandelInfo, struct Window *Win, uint8 *GfxMem, uint32 *RndMem, uint32 *HistogramMem)
{
  uint32 Iterations, Rows, Cols, Result, Total = 0L;
  float64 Normalized;

  	SNPrintf (BAR_STRING, sizeof (BAR_STRING), "Appling histogram coloring algorithm, it may take very long time on slow systems and for high iterations. Please wait...");
  	SetWindowTitles (Win, (STRPTR) ~0, BAR_STRING);
  	ShowTitle (Win->WScreen, TRUE);

  	for (Iterations = 1; Iterations <= MandelInfo->Iterations; Iterations++) // Iterations=number of pixels reached that iteration before bailout
    	Total += *(HistogramMem + Iterations); // Total=sum of all stored values outside mandelbrot or julia set

  	for (Cols = MandelInfo->TopEdge; Cols < MandelInfo->Height; Cols++)
    {
      	for (Rows = MandelInfo->LeftEdge; Rows < MandelInfo->Width; Rows++)
    	{
      		if (Result = *(RndMem + (Cols * MandelInfo->Width + Rows)))
        	{
				Normalized = 0.0;
			
          		for (Iterations = 1; Iterations <= Result; Iterations++)
        		{
          			Normalized += (float64) (*(HistogramMem + Iterations)) / (float64) Total;
        		}

          		*(GfxMem + (Cols * MandelInfo->Width + Rows)) = COLORREMAP (Normalized, 0.0, 1.0, 4.0, 255.0);
        	}
    	}
    }

  	if (!(TMASK & MASK)) ShowTitle (Win->WScreen, FALSE);
  	WriteChunkyPixels (Win->RPort, MandelInfo->LeftEdge, MandelInfo->TopEdge,
             MandelInfo->Width - 1, MandelInfo->Height - 1, GfxMem, MandelInfo->Modulo);
}

void DrawAxis (struct Window *Win, int16 StepX, int16 StepY)
{
  int16 i;

	Move (Win->RPort, Win->GZZWidth / 2, 0);
    Draw (Win->RPort, Win->GZZWidth / 2, Win->GZZHeight);
    Move (Win->RPort, 0, Win->GZZHeight / 2);
    Draw (Win->RPort, Win->GZZWidth, Win->GZZHeight / 2);

    for (i = 0; i <= Win->GZZHeight; i += StepY)
    {
		Move (Win->RPort, (Win->GZZWidth / 2) - 5, i);
        Draw (Win->RPort, (Win->GZZWidth / 2) + 5, i);
    }

    for (i = 0; i <= Win->GZZWidth; i += StepX)
	{
        Move (Win->RPort, i, (Win->GZZHeight / 2) - 5);
        Draw (Win->RPort, i, (Win->GZZHeight / 2) + 5);
    }
}

int16 Orbit (struct Window *Win, int16 Width, int16 Height)
{
  struct Window *OrbitWin = NULL;
  int16 Error = FALSE;
  int16 OrbitWinDim = MAX (Width, Height) / 4;

	OrbitWin = OpenWindowTags (NULL,
								WA_Left, WINDOW_X_OFFSET, WA_Top, WINDOW_Y_OFFSET,
							   	//  WA_Width,400,
								//  WA_Height,400,
                                WA_Width, OrbitWinDim,
                                WA_Height, OrbitWinDim,
                                WA_Title, CATSTR (TXT_OrbitTitle),
                                WA_ScreenTitle, "Window orbit...",
                                WA_CustomScreen, Win->WScreen,
								//  WA_IDCMP,IDCMP_RAWKEY|IDCMP_CLOSEWINDOW,
                                WA_Flags,
                                WFLG_DRAGBAR | WFLG_NOCAREREFRESH |
                                WFLG_SMART_REFRESH | WFLG_RMBTRAP |
                                WFLG_GIMMEZEROZERO | WFLG_NW_EXTENDED
                                /*WFLG_CLOSEGADGET| */ ,
                                WA_BusyPointer, TRUE, WA_StayTop, TRUE,
                                WA_DropShadows, TRUE, TAG_DONE);

    if (OrbitWin)
    {
        ModifyIDCMP (Win,IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_MENUPICK|IDCMP_RAWKEY);
        SetRast (OrbitWin->RPort, DARK_GREY);
        SetDrMd (OrbitWin->RPort, JAM1);
        DrawAxis (OrbitWin, OrbitWin->GZZWidth / 5, OrbitWin->GZZHeight / 5);
        SetDrMd (OrbitWin->RPort, COMPLEMENT);
        if (Error = ShowOrbit (MANDChunk, Win, OrbitWin)) DisplayError (Win, TXT_ERR_OrbitWindow, 15L);
        ModifyIDCMP (Win, IDCMP_STANDARD);
        CloseWindow (OrbitWin);
    }

    else
    {
        DisplayError (Win, TXT_ERR_OrbitWindow, 15L);
        Error = TRUE;
    }

    return (Error);
}

/* ShowOrbit() */
int16 ShowOrbit (struct MandelChunk * MandelInfo, struct Window * Win, struct Window * OrbitWin)
{
  struct IntuiMessage *Message = NULL;
  int16 *PArray = NULL;
  int16 Error = FALSE, Exit = FALSE;
  int16 MouseX, MouseY;
  uint16 MyCode;
  uint32 MyClass, Index, OrbitIterations = 0L;
  mpf_t RealCoord, ImagCoord;

	mpf_inits (RealCoord, ImagCoord, 0);

	if (PArray = AllocVecTags (((sizeof (int16) * (2 * MandelInfo->Iterations)) + 1),
                                AVT_Type, MEMF_PRIVATE, AVT_Contiguous, TRUE, AVT_Lock, TRUE, 
                                AVT_Alignment, 16, AVT_ClearWithValue, 0, TAG_DONE))
	{    
    	do
        {
			WaitPort (Win->UserPort);

            while ((!Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort)))
            {
				MyClass = Message->Class;
                MyCode = Message->Code;
                MouseX = Message->MouseX;
                MouseY = Message->MouseY;
                GT_ReplyIMsg ((struct IntuiMessage *) Message);
        
                switch (MyClass)
                {
                	case IDCMP_MOUSEMOVE:
                    {
						for (Index = 0; Index < OrbitIterations * 2; Index += 2)
                        		DrawEllipse (OrbitWin->RPort, PArray[Index], PArray[Index + 1], 2, 2);

                        mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                        mpf_abs (RealCoord, gtmp);
                        mpf_mul_ui (RealCoord, RealCoord, MouseX);
                        mpf_div_ui (RealCoord, RealCoord, (Win->GZZWidth - 1));
                        mpf_add (RealCoord, MandelInfo->GRMin, RealCoord);
						// RealCoord = MandelInfo->RMin + ((float64) MouseX * fabs (MandelInfo->RMax-MandelInfo->RMin) / (float64) (Win->GZZWidth - 1));
                        mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                        mpf_abs (ImagCoord, gtmp);
                        mpf_mul_ui (ImagCoord, ImagCoord, MouseY);
                        mpf_div_ui (ImagCoord, ImagCoord, (Win->GZZHeight - 1));
                        mpf_sub (ImagCoord, MandelInfo->GIMax, ImagCoord);
						// ImagCoord = MandelInfo->IMax - ((float64) MouseY * fabs (MandelInfo->IMax-MandelInfo->IMin) / (float64) (Win->GZZHeight - 1));

                        if (MandelInfo->Flags & MANDEL_BIT)
                        {
                            if (MandelInfo->Flags & HIGHPREC_BIT)
                            	OrbitIterations = CalcMandelnOrbit_GMP (PArray, MandelInfo->Iterations, MandelInfo->Power,
                                                                        OrbitWin->GZZWidth, OrbitWin->GZZHeight, RealCoord, ImagCoord);
                            else
                                OrbitIterations = CalcMandelnOrbit (PArray, MandelInfo->Iterations, MandelInfo->Power,
                                                                    OrbitWin->GZZWidth, OrbitWin->GZZHeight, mpf_get_d (RealCoord), mpf_get_d (ImagCoord));
                        }

                        else if (MandelInfo->Flags & JULIA_BIT)
                        {
                        	if (MandelInfo->Flags & HIGHPREC_BIT)
                                OrbitIterations = CalcJulianOrbit_GMP (PArray, MandelInfo->Iterations, MandelInfo->Power,
                                                                        OrbitWin->GZZWidth, OrbitWin->GZZHeight, RealCoord, ImagCoord, gcre, gcim);
                            else
                                OrbitIterations = CalcJulianOrbit (PArray, MandelInfo->Iterations, MandelInfo->Power, OrbitWin->GZZWidth, OrbitWin->GZZHeight, 
                                                                    mpf_get_d (RealCoord), mpf_get_d (ImagCoord), mpf_get_d (gcre), mpf_get_d (gcim));
                        }

                        for (Index = 0; Index < OrbitIterations * 2; Index += 2)
                             DrawEllipse (OrbitWin->RPort, PArray[Index], PArray[Index + 1], 2, 2);

                        if (TMASK & MASK)
                        {
                            gmp_snprintf (BAR_STRING, sizeof (BAR_STRING), "Orbit Real %#+2.30Ff Imag %#+2.30Ffi", RealCoord, ImagCoord);
							// sprintf (BAR_STRING,"Orbit Real %#+2.16f Imag %#+2.16fi",RealCoord,ImagCoord);
                            WaitTOF ();
                            SetWindowTitles (Win, (STRPTR) ~0, BAR_STRING);
                        }
                    }
					break;

                    case IDCMP_RAWKEY:
                    {
						if (MyCode == RAW_ESC) Exit = TRUE;
                    }
					break;
        
                    case IDCMP_MOUSEBUTTONS:
                    {
					    Exit = TRUE;
                    }
					break;
                  
                    case IDCMP_MENUPICK:
                    {
					    Exit = TRUE;
                    }
					break;
                } // switch (MyClass)            
            } // while                         
        } while (!Exit); // do while

        if (PArray)
        {
              FreeVec (PArray);
              PArray = NULL;
        }
    } // if
    
	else Error = TRUE;

    mpf_clears (RealCoord, ImagCoord, 0);
    return (Error);
}

/* BlinkRect() */
void BlinkRect (struct Window *Win, const int16 LeftEdge, const int16 TopEdge,
       				const int16 RightEdge, const int16 BottomEdge)
{
  uint8 Blink = 2;

  	DisplayBeep (Win->WScreen);
  	ZOOMLINE[6] = ZOOMLINE[8] = LeftEdge;
  	ZOOMLINE[1] = ZOOMLINE[3] = ZOOMLINE[9] = TopEdge;
  	ZOOMLINE[2] = ZOOMLINE[4] = RightEdge;
  	ZOOMLINE[5] = ZOOMLINE[7] = BottomEdge;
  	ZOOMLINE[0] = LeftEdge + 1;

  	while (Blink--)
    {   /* give a visual alert of rectangle being skipped */
      	DrawBorder (Win->RPort, &MYBORDER, 0, 0);
      	Delay (1);
      	DrawBorder (Win->RPort, &MYBORDER, 0, 0);
      	Delay (1);
    }
}

/* Preview() */
int16 Preview (struct Window *Win, uint8 * PixelVector, uint8 * ARGBMem,
     			uint32 * RndMem, uint8 * PixMem, uint8 * GfxMem, int16 Width, int16 Height)
{
  struct Window *PreviewWin = NULL;
  struct IntuiMessage *Message = NULL;
  int16 Error = FALSE, Exit = FALSE;
  uint16 MyCode;
  uint32 MyClass;

  	MYBITMAP = CopyBitMap (Win, (uint16) ZOOMLINE [6], (uint16) ZOOMLINE [3], (uint16) (ZOOMLINE [4] - ZOOMLINE [6] + 1), (uint16) (ZOOMLINE [5] - ZOOMLINE [3] + 1));

  	PreviewWin = OpenWindowTags (NULL,
                   WA_Left, WINDOW_X_OFFSET,
                   WA_Top, WINDOW_Y_OFFSET,
                   WA_Width, Width / 2,
                   WA_Height, Height / 2,
                   WA_Title, CATSTR (TXT_PreviewTitle),
                   WA_ScreenTitle, "Window preview...",
                   WA_CustomScreen, Win->WScreen,
                   WA_IDCMP, IDCMP_RAWKEY | IDCMP_CLOSEWINDOW,
                   WA_Flags, WFLG_ACTIVATE | WFLG_CLOSEGADGET | WFLG_DRAGBAR | WFLG_NOCAREREFRESH |
                   				WFLG_SMART_REFRESH | WFLG_RMBTRAP | WFLG_GIMMEZEROZERO | WFLG_NW_EXTENDED,
                   WA_BusyPointer, TRUE, WA_StayTop, TRUE,
                   WA_DropShadows, TRUE, TAG_DONE);
				   
  	if (PreviewWin)
    {
  		PasteBitMap (MYBITMAP, PreviewWin, (uint16) PreviewWin->LeftEdge, (uint16) PreviewWin->TopEdge, (uint16) (ZOOMLINE [4] - ZOOMLINE [6] + 1),
           				(uint16) (ZOOMLINE [5] - ZOOMLINE [3] + 1));    	
		
		PutPointer (PreviewWin, 0, 0, 0, 0, 0, BUSY_POINTER);
      	ELAPSEDTIME = DrawFractal (MANDChunk, PreviewWin, ARGBMem, PixMem, GfxMem, PixelVector, RndMem, FALSE);
      	ClearPointer (PreviewWin);
      	ShowTime (PreviewWin, CATSTR (TXT_PreviewTime), ELAPSEDTIME);

    	do
    	{
      		WaitPort (PreviewWin->UserPort);

      		while ((!Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (PreviewWin->UserPort)))
        	{
          		MyClass = Message->Class;
          		MyCode = Message->Code;
          		GT_ReplyIMsg ((struct IntuiMessage *) Message);
          		if (MyClass == IDCMP_CLOSEWINDOW) Exit = TRUE;    
          		else if ((MyClass == IDCMP_RAWKEY) && (MyCode == RAW_ESC)) Exit = TRUE;
            }
			
    	} while (!Exit);

      	CloseWindow (PreviewWin);
    }

  	else
    {
      	if (BMASK & MASK)
      	{
          	if (MYBITMAP)
            {
              	FreeBitMapSafety (MYBITMAP);
              	MYBITMAP = NULL;
              	MASK &= ~BMASK;
            }
      	}

      	DisplayError (Win, TXT_ERR_PreviewWindow, 15L);
      	Error = TRUE;
    }

  	return (Error);
}

/* DrawFrame() */
int16 DrawFrame (struct Window * Win, const int16 Zx, const int16 Zy, const int16 Zw, const int16 Zh)
{
  int16 LeftEdge, TopEdge, RightEdge, BottomEdge;

  	if ((Zx > 2) && (Zy > 2) && (Zx < Win->GZZWidth - 2) && (Zy < Win->GZZHeight - 2) && ((Zw >= INITIALZOOM) || (Zh >= INITIALZOOM)))
    {
      	LeftEdge = Zx - Zw;
      	TopEdge = Zy - Zh;
      	RightEdge = Zx + Zw;
      	BottomEdge = Zy + Zh;

      	if ((LeftEdge >= 0) && (TopEdge >= 0) && (RightEdge <= Win->GZZWidth) && (BottomEdge <= Win->GZZHeight))
    	{
      		ZOOMLINE[6] = ZOOMLINE[8] = LeftEdge;
      		ZOOMLINE[1] = ZOOMLINE[3] = ZOOMLINE[9] = TopEdge;
      		ZOOMLINE[2] = ZOOMLINE[4] = RightEdge;
      		ZOOMLINE[5] = ZOOMLINE[7] = BottomEdge;
      		ZOOMLINE[0] = LeftEdge + 1;
    	}

      	DrawBorder (Win->RPort, &MYBORDER, 0, 0);
      	MASK |= ZMASK;
    }

  	else MASK &= ~ZMASK;

  	return (int16) (ZMASK & MASK);
}

/* FileRequest() */
int16 FileRequest (struct Window *Win, STRPTR String, STRPTR DrawerTxt, int16 DrawerType, int16 Save)
{
  struct FileRequester *MyFileReq = NULL;
  struct EasyStruct MyReq = { sizeof (struct EasyStruct), 0, CATSTR (TITLE_OverwriteReq), CATSTR (OverWr_TXT_Question), CATSTR (TXT_YesNo) };
  BPTR MyLock;
  int16 Success = FALSE;

  	if (MyFileReq = AllocAslRequest (ASL_FileRequest, 0))
    {
				
   		MyDir = ((DrawerType == PALETTES_DRAWER) ? PALETTESDIR : PICTURESDIR);	
		if (! Strlen (MyDir)) Strlcpy (MyDir, DrawerTxt, sizeof (PALETTESDIR));
									
      	if (AslRequestTags (MyFileReq, ASLFR_Window, Win,
              ASLFR_InitialLeftEdge, WINDOW_X_OFFSET,
              ASLFR_InitialTopEdge, WINDOW_Y_OFFSET,
              ASLFR_InitialWidth, (Win->GZZWidth / 3),
              ASLFR_InitialHeight, (Win->GZZHeight / 2),
              ASLFR_SleepWindow, TRUE,
              ASLFR_TextAttr, &MYFONTSTRUCT,
              ASLFR_TitleText, String,
              ASLFR_InitialDrawer, MyDir,
              ASLFR_InitialFile, MYFILE,
              ASLFR_DoSaveMode, Save,
              ASLFR_RejectPattern, "#?.fmng",
              ASLFR_RejectIcons, TRUE, TAG_DONE))
    	{
      		Strlcpy (MYFILE, MyFileReq->fr_File, sizeof (MYFILE));
      		Strlcpy (MyDir, MyFileReq->fr_Drawer, sizeof (PICTURESDIR));
      		Strlcpy (MYPATH, MyDir, sizeof (MYPATH));
      		Success = AddPart (MYPATH, MYFILE, sizeof (MYPATH));
		}

      	FreeAslRequest (MyFileReq);

      	if (Save && Success)
    	{
      		if (MyLock = Lock (MYPATH, ACCESS_READ))
        	{
          		UnLock (MyLock);
#ifdef FM_REACT_SUPPORT
          		if (!Do_OverwriteRequest (Win, FMSCREENNAME, 0, 0)) Success = FALSE;
#else /* FM_REACT_SUPPORT */
          		if (!EasyRequest (Win, &MyReq, 0)) Success = FALSE;
#endif /* FM_REACT_SUPPORT */
        	}
    	}
    }

  	return (Success);
}

/* FontRequest() */
int16 FontRequest (struct Window * Win)
{
  struct FontRequester *MyFontReq = NULL;
  int16 Success = FALSE;

  	if (MyFontReq = AllocAslRequest (ASL_FontRequest, 0))
    {
      	if (AslRequestTags (MyFontReq, ASLFO_Window, Win,
			ASLFO_InitialLeftEdge, WINDOW_X_OFFSET,
            ASLFO_InitialTopEdge, WINDOW_Y_OFFSET,
            ASLFO_InitialWidth, (Win->GZZWidth / 2),
            ASLFO_InitialHeight, (Win->GZZHeight / 2),
            ASLFO_SleepWindow, TRUE,
            ASLFO_TextAttr, &MYFONTSTRUCT,              
			ASLFO_TitleText, "Font requester",
            ASLFO_InitialName, MYFONTSTRUCT.ta_Name,
            ASLFO_InitialSize, MYFONTSTRUCT.ta_YSize,
            ASLFO_InitialStyle, MYFONTSTRUCT.ta_Style,
            ASLFO_InitialFlags, MYFONTSTRUCT.ta_Flags,
            ASLFO_DoFrontPen, FALSE,
            ASLFO_DoBackPen, FALSE,
            ASLFO_DoStyle, FALSE,
            ASLFO_DoDrawMode, FALSE,
            ASLFO_FixedWidthOnly, TRUE,
            ASLFO_MinHeight, MIN_FONTSIZE,
            ASLFO_MaxHeight, MAX_FONTSIZE, TAG_DONE))
    	{
      		Strlcpy (MYFONT, MyFontReq->fo_Attr.ta_Name, sizeof (MYFONT));
      		MYFONTSTRUCT.ta_Name = MYFONT;
      		MYFONTSTRUCT.ta_YSize = MIN (MAX_FONTSIZE, MAX (MIN_FONTSIZE, MyFontReq->fo_Attr.ta_YSize));
      		MYFONTSTRUCT.ta_Style = MyFontReq->fo_Attr.ta_Style;
      		MYFONTSTRUCT.ta_Flags = (MyFontReq->fo_Attr.ta_Flags) | FPF_DESIGNED;
      		Success = TRUE;
    	}

      	FreeAslRequest (MyFontReq);
    }

  	return (Success);
}

/* SMRequest() */
int16 SMRequest (struct ILBMInfo * Ilbm)
{
  struct ScreenModeRequester *SMReq = NULL;
  struct Window *Win = Ilbm->win;
  int16 NewScreen = FALSE;

    if (SMReq = AllocAslRequest (ASL_ScreenModeRequest, NULL))
    {
		if (AslRequestTags (SMReq, ASLSM_Window, Win,
            ASLSM_SleepWindow, TRUE,
            ASLSM_TitleText, "ScreenMode requester",
            ASLSM_InitialLeftEdge, WINDOW_X_OFFSET,
            ASLSM_InitialTopEdge, WINDOW_Y_OFFSET,
            ASLSM_InitialWidth, (Win->GZZWidth / 3),
            ASLSM_InitialHeight, (Win->GZZHeight / 2),
            ASLSM_InitialDisplayID, Ilbm->camg,
            ASLSM_InitialDisplayWidth, Ilbm->Bmhd.w,
            ASLSM_InitialDisplayHeight, Ilbm->Bmhd.h,
            ASLSM_InitialDisplayDepth, Ilbm->Bmhd.nPlanes, 
            ASLSM_InitialInfoOpened, FALSE, 
            ASLSM_DoWidth, FALSE, 
            ASLSM_DoHeight, FALSE, 
            ASLSM_DoDepth, FALSE,               
            ASLSM_PropertyFlags, DIPF_IS_RTG|DIPF_IS_WB, 
            ASLSM_PropertyMask, DIPF_IS_DUALPF|DIPF_IS_PF2PRI|DIPF_IS_HAM|DIPF_IS_EXTRAHALFBRITE|DIPF_IS_PAL,                       
            ASLSM_MinWidth, MIN_WIDTH, 
            ASLSM_MinHeight, MIN_HEIGHT, 
            ASLSM_MinDepth, MIN_DEPTH, 
            ASLSM_MaxDepth, MAX_DEPTH,
            ASLSM_FilterFunc, &SMFILTERHOOK,
            TAG_DONE))
        {
			Ilbm->camg = SMReq->sm_DisplayID;
            Ilbm->Bmhd.w = SMReq->sm_DisplayWidth;
            Ilbm->Bmhd.h = SMReq->sm_DisplayHeight;
            Ilbm->Bmhd.nPlanes = SMReq->sm_DisplayDepth;
            NewScreen = TRUE;
        }

        FreeAslRequest (SMReq);
    }

    return (NewScreen);
}

/* SetMenuStart() */
void SetMenuStart (struct ILBMInfo *Ilbm, int16 UndoBuffer)
{
    if (UndoBuffer > 0) OnMenu (Ilbm->win, FULLMENUNUM (2, 4, NOSUB));
    else OffMenu (Ilbm->win, FULLMENUNUM (2, 4, NOSUB));

    if (Ilbm->Bmhd.nPlanes == MAX_DEPTH)
	{
        OffMenu (Ilbm->win, FULLMENUNUM (0, 8, NOSUB));
        OffMenu (Ilbm->win, FULLMENUNUM (0, 9, NOSUB));
        OffMenu (Ilbm->win, FULLMENUNUM (1, 5, NOSUB));
        OffMenu (Ilbm->win, FULLMENUNUM (3, 0, NOSUB));
        OffMenu (Ilbm->win, FULLMENUNUM (3, 2, NOSUB));
    }

    else if (Ilbm->Bmhd.nPlanes == MIN_DEPTH)
    {
        OnMenu (Ilbm->win, FULLMENUNUM (0, 8, NOSUB));
        OnMenu (Ilbm->win, FULLMENUNUM (0, 9, NOSUB));
        OnMenu (Ilbm->win, FULLMENUNUM (1, 5, NOSUB));
        OnMenu (Ilbm->win, FULLMENUNUM (3, 0, NOSUB));
        OnMenu (Ilbm->win, FULLMENUNUM (3, 2, NOSUB));
    }

    OnMenu (Ilbm->win, FULLMENUNUM (0, 0, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (0, 2, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (0, 4, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (0, 6, NOSUB)); // Load iff
    OnMenu (Ilbm->win, FULLMENUNUM (0, 7, NOSUB)); // Save iff
    OnMenu (Ilbm->win, FULLMENUNUM (0, 11, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (0, 13, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (1, 0, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (1, 1, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (1, 2, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (1, 3, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (1, 6, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (1, 7, NOSUB)); // turbo mode  
    OnMenu (Ilbm->win, FULLMENUNUM (1, 8, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (2, 2, NOSUB));
    OffMenu (Ilbm->win, FULLMENUNUM (2, 7, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (2, 9, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (3, 4, NOSUB));
    OnMenu (Ilbm->win, FULLMENUNUM (3, 6, NOSUB));
}

/* SetMenuStop() */
void SetMenuStop (struct ILBMInfo *Ilbm)
{
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 0, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 2, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 4, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 6, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 7, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 8, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 9, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 11, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (0, 13, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 0, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 1, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 2, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 3, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 5, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 6, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 7, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (1, 8, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (2, 0, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (2, 2, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (2, 6, NOSUB));
  	OnMenu (Ilbm->win, FULLMENUNUM (2, 7, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (2, 9, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (3, 0, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (3, 2, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (3, 4, NOSUB));
  	OffMenu (Ilbm->win, FULLMENUNUM (3, 6, NOSUB));
}

/* SMFilterFunc() static */
int16 __attribute__ ((saveds)) SMFilterFunc (REG (a0, struct Hook *Hook), REG (a2, struct ScreenModeRequester *SMReq), REG (a1, uint32 DisplayID))
{
  const DisplayInfoHandle DisplayHandle = FindDisplayInfo (DisplayID);
  struct DisplayInfo DisplayInfo;
  struct DimensionInfo DimensionInfo;
  int16 Accept = NULL;

	if ((DisplayHandle) && GetDisplayInfoData (DisplayHandle, (APTR) &DisplayInfo, sizeof (struct DisplayInfo), DTAG_DISP, DisplayID))
    {
        if (GetDisplayInfoData (DisplayHandle, (APTR) &DimensionInfo, sizeof (struct DimensionInfo), DTAG_DIMS, DisplayID))
        {
            Accept = (((DimensionInfo.MaxDepth == MIN_DEPTH) || (DimensionInfo.MaxDepth == MAX_DEPTH))                 
            			&& (DisplayInfo.PropertyFlags & (DIPF_IS_WB|DIPF_IS_RTG)));
        }
    }

    return (Accept);
}

/* ProcessMenu() */
uint32 ProcessMenu (struct MandelChunk * MandelInfo, struct Window *Win, uint8 * ARGBMem, uint8 * PixMem, 
					uint32 * PixelVector, uint32 * RndMem, uint8 * GfxMem, uint16 Code)
{
  struct MenuItem *Item = NULL;
  uint32 Choice = NULL;

    while (Code != MENUNULL)
    {
    	Item = ItemAddress (MAINMENU, (uint32) Code);

        switch (MENUNUM (Code))
        {
        	case 0:
            {
				switch (ITEMNUM (Code))
                {
                	case 0:
                          Choice |= ABOUT_MSG;
                    break;
					
                    case 2:
                          Choice |= SYSINFO_MSG;
                    break;
					
                    case 4:
                          Choice |= SHOWGUIDE_MSG;
                    break;
					
                    case 6:
                          Choice |= LOADPICTURE_MSG;
                    break;
					
                    case 7:
                          Choice |= SAVEPICTURE_MSG;
                    break;
					
                    case 8:
                          Choice |= LOADPALETTE_MSG;
                    break;
					
                    case 9:
                          Choice |= SAVEPALETTE_MSG;
                    break;
					
                    case 11:
                          Choice |= DUMP_MSG;
                    break;
					
                    case 13:
                          Choice |= EXIT_MSG;
                    break;
                }
			}	
            break;

            case 1:
            {
				switch (ITEMNUM (Code))
                {
                    case 0:
                    {
					    switch (SUBNUM (Code))
                        {
                            case 0:
                                  Choice |= TITLE_MSG;
                            break;
                            
							case 2:
                                  Choice |= TIME_MSG;
                            break;
                        }
					}	
                    break;
                    
					case 1:
                    {
					      Choice |= COORDS_MSG;
                    }
					break;
                    
					case 2:
					{
                        switch (SUBNUM (Code))
                        {
                            case 0:
                                  MandelInfo->Iterations = 256;
                            break;
							
                            case 1:
                                  MandelInfo->Iterations = 320;
                            break;
							
                            case 2:
                                  MandelInfo->Iterations = 512;
                            break;
							
                            case 3:
                                  MandelInfo->Iterations = 1024;
                            break;
							
                            case 4:
                                  MandelInfo->Iterations = 2048;
                            break;
							
                            case 5:
                                  MandelInfo->Iterations = 4096;
                            break;
							
                            case 6:
                                  MandelInfo->Iterations = 8192;
                            break;
							
                            case 7:
                                  MandelInfo->Iterations = 16384;
                            break;
							
                            case 8:
                                  MandelInfo->Iterations = 32768;
                            break;
							
                            case 10:
                                  Choice |= ITER_MSG;
                            break;
                        }
                    }
					break;
					
                    case 3:
					{
                        switch (SUBNUM (Code))
                        {
                            case 0:
                                  MandelInfo->Power = 1;    /* 2 */
                            break;
							
                            case 1:
                                  MandelInfo->Power = 2;    /* 4 */
                            break;
							
                            case 2:
                                  MandelInfo->Power = 3;    /* 8 */
                            break;
							
                            case 3:
                                  MandelInfo->Power = 4;    /* 16 */
                            break;
							
                            case 4:
                                  MandelInfo->Power = 5;    /* 32 */
                            break;
							
                            case 5:
                                  MandelInfo->Power = 6;    /* 64 */
                            break;
							
                            case 6:
                                  MandelInfo->Power = 7;    /* 128 */
                            break;
							
                            case 7:
                                  MandelInfo->Power = 8;    /* 256 */
                            break;
							
                            case 8:
                                  MandelInfo->Power = 9;    /* 512 */
                            break;
							
                            case 9:
                                  MandelInfo->Power = 10;    /* 1024 */
                            break;
							
                            case 10:
                                  MandelInfo->Power = 11;    /* 2048 */
                            break;
                        }
					}	
                    break;
                    
					case 4:
					{
                        switch (SUBNUM (Code))
                        {
                            case 0:
                                  PRIORITY = -5;
                            break;
							
                            case 1:
                                  PRIORITY = -4;
                            break;
							
                            case 2:
                                  PRIORITY = -3;
                            break;
							
                            case 3:
                                  PRIORITY = -2;
                            break;
							
                            case 4:
                                  PRIORITY = -1;
                            break;
							
                            case 6:
                                  PRIORITY = 0;
                            break;
							
                            case 8:
                                  PRIORITY = 1;
                            break;
							
                            case 9:
                                  PRIORITY = 2;
                            break;
							
                            case 10:
                                  PRIORITY = 3;
                            break;
							
                            case 11:
                                  PRIORITY = 4;
                            break;
							
                            case 12:
                                  PRIORITY = 5;
                            break;
                        }

                        Forbid ();
                        SetTaskPri (FindTask (NULL), PRIORITY);
                        Permit ();
                    }
					break;
					
                    case 5:
					{
                        switch (SUBNUM (Code))
                        {
                            case 0:
                                if (!(MandelInfo->Flags & LINEAR_BIT))
                                {
                                    MandelInfo->Flags |= LINEAR_BIT;
                                    MandelInfo->Flags &= ~(SQUARE_BIT|REPEATED_BIT|LOG_BIT|ONE_BIT|TWO_BIT|THREE_BIT|FOUR_BIT);
                                }
                            break;
							
                            case 1:
                                if (!(MandelInfo->Flags & LOG_BIT))
                                {
                                    MandelInfo->Flags |= LOG_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|REPEATED_BIT|SQUARE_BIT|ONE_BIT|TWO_BIT|THREE_BIT|FOUR_BIT);
                                }
                            break;
							
                            case 2:
                                if (!(MandelInfo->Flags & REPEATED_BIT))
                                {                  
					                MandelInfo->Flags |= REPEATED_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|SQUARE_BIT|LOG_BIT|ONE_BIT|TWO_BIT|THREE_BIT|FOUR_BIT);
                                }
                              break;
							  
                            case 3:
                                if (!(MandelInfo->Flags & SQUARE_BIT))
                                {
                                    MandelInfo->Flags |= SQUARE_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|REPEATED_BIT|LOG_BIT|ONE_BIT|TWO_BIT|THREE_BIT|FOUR_BIT);
                                }
                            break;
							
                            case 4:
                                if (!(MandelInfo->Flags & ONE_BIT))
                                {
                                    MandelInfo->Flags |= ONE_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|REPEATED_BIT|LOG_BIT|SQUARE_BIT|TWO_BIT|THREE_BIT|FOUR_BIT);
                                }
                            break;
							
                            case 5:
                                if (!(MandelInfo->Flags & TWO_BIT))
                                {
                                    MandelInfo->Flags |= TWO_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|REPEATED_BIT|LOG_BIT|SQUARE_BIT|ONE_BIT|THREE_BIT|FOUR_BIT);
                                }
                            break;
							
                            case 6:
                                if (!(MandelInfo->Flags & THREE_BIT))
                                {
                                    MandelInfo->Flags |= THREE_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|REPEATED_BIT|LOG_BIT|SQUARE_BIT|ONE_BIT|TWO_BIT|FOUR_BIT);
                                }
                            break;
							
                            case 7:
                                if (!(MandelInfo->Flags & FOUR_BIT))
                                {
                                    MandelInfo->Flags |= FOUR_BIT;
                                    MandelInfo->Flags &= ~(LINEAR_BIT|REPEATED_BIT|LOG_BIT|SQUARE_BIT|ONE_BIT|TWO_BIT|THREE_BIT);
                                }
                            break;
                        }
					}
					break;	
                        
					case 6:
                    {
						switch (SUBNUM (Code))
                        {
                        	case 0:
                            {
								if (!(MandelInfo->Flags & JULIA_BIT))
                                {
                           			ClearZoomFrame (Win->RPort);
										
                                    MandelInfo->Flags &= ~(MANDEL_BIT); // not remove for PickJuliaK!!
                                    MandelInfo->Flags |= JULIA_BIT;

                                    if (PickJuliaK (MandelInfo, Win, ARGBMem, PixMem, PixelVector, RndMem, GfxMem))
                                    {
                                        MandelInfo->Flags &= ~(JULIA_BIT);
                                    	MandelInfo->Flags |= MANDEL_BIT;

                                        SaveCoords (Win);
              
                                        MandelInfo->Flags &= ~(MANDEL_BIT);
                                        MandelInfo->Flags |= JULIA_BIT;
                  
                                        Choice |= REDRAW_MSG;
                                    }
                                        
                                    else 
                                    {
                                        MandelInfo->Flags &= ~(JULIA_BIT);
                                        MandelInfo->Flags |= MANDEL_BIT;                
                                    }
                                }        
                            }
							break;
								
                            case 1:
							{
                            	if (!(MandelInfo->Flags & MANDEL_BIT))
                                {
                                    SaveCoords (Win);
                                    MandelInfo->Flags &= ~(JULIA_BIT);
                                    MandelInfo->Flags |= MANDEL_BIT;
                                    MANDChunk->Flags = MandelInfo->Flags;
                                    /* initilize default float64 start coordinates */
                                    MandelInfo->RMin = DEF_RMIN;
                                    MandelInfo->RMax = DEF_RMAX;
                                    MandelInfo->IMin = DEF_IMIN;
                                    MandelInfo->IMax = DEF_IMAX;

                                    /* of course initialize also mandelchunk GMP values with user default values */
                                    mpf_set_d (MandelInfo->GIMin, MandelInfo->IMin);
                                    mpf_set_d (MandelInfo->GRMin, MandelInfo->RMin);
                                    mpf_set_d (MandelInfo->GIMax, MandelInfo->IMax);
                                    mpf_set_d (MandelInfo->GRMax, MandelInfo->RMax);

                                    Choice |= REDRAW_MSG;
                                }
                            }
							break;
                        }
                    }
					break;
                        
					case 7:
					{
                    	switch (SUBNUM (Code))
                        {
                        	case 0:
                        	{
								if (!(MandelInfo->Flags & TURBO_BIT)) MandelInfo->Flags |= TURBO_BIT;
                                else MandelInfo->Flags &= ~(TURBO_BIT);
                            }
							break;
                            
							case 2:
							{
                                if (!(MandelInfo->Flags & HISTOGRAM_BIT))
                                {
                                	if ((MandelInfo->Flags & TURBO_BIT) && (MandelInfo->Depth == MIN_DEPTH)) MandelInfo->Flags |= HISTOGRAM_BIT;  
                                }                    

                                else MandelInfo->Flags &= ~(HISTOGRAM_BIT);
          
                                if (MandelInfo->Flags & HISTOGRAM_BIT)
                                { 
                                   if ((!(MandelInfo->Flags & TURBO_BIT)) || (!(MandelInfo->Depth == MIN_DEPTH))) MandelInfo->Flags &= ~(HISTOGRAM_BIT);
                                }
							}	
                            break;
                       	}
                    }
					break;
					
                    case 8:
					{
                    	switch (SUBNUM (Code))
                        {
                        	case 0:
                            	MandelInfo->Flags &= ~HIGHPREC_BIT;
                            break;
                            
							case 1:
                                MandelInfo->Flags |= HIGHPREC_BIT;
                            break;
                            
							case 3:
                                Choice |= PREC_MSG;
                            break;
                        }
					}	
                }
			}	
            break;
			
            case 2:
            {
			    switch (ITEMNUM (Code))
                {
                    case 0:
                        Choice |= PREVIEW_MSG;
                    break;
					
                    case 2:
                        Choice |= REDRAW_MSG;
                    break;
					
                    case 4:
                        Choice |= (UNDO_MSG | REDRAW_MSG);
                    break;
					
                    case 6:
                        Choice |= DRAW_MSG;
                    break;
					
                    case 7:
                        Choice |= STOP_MSG;
                    break;
					
                    case 9:
                        Choice |= ORBIT_MSG;
                    break;
                }
			}	
            break;
			
            case 3:
            {
			    switch (ITEMNUM (Code))
                {
                	case 0:
                    {
						switch (SUBNUM (Code))
                        {
                        	case 0:
                            	Choice |= CYCLERIGHT_MSG;
                            break;
							
                            case 1:
                                Choice |= CYCLELEFT_MSG;
                            break;
							
                            case 3:
                                Choice |= DELAY_MSG;
                            break;
                        }
					}	
                    break;
					
                    case 2:
                    {
					    Choice |= PALETTE_MSG;
                    }
					break;
					
                    case 4:
                    {
					    Choice |= NEWDISPLAY_MSG;
                    }
					break;
					
                    case 6:
                    {
					    Choice |= FONTREQ_MSG;
                    }
					break;
                }
			}	
			break;
				
            case 4:
            {
			    Choice |= AREXX_MSG;
            }
			break;
        } // switch

        Code = Item->NextSelect;
    } // while

    CheckMenu (Win);
    
	return (Choice);
}

/* CheckMenu() */
void CheckMenu (struct Window *Win)
{
  uint16 Index;

  	ClearMenuStrip (Win);

    /* set menu tile flag */
  	if (TMASK & MASK) ItemAddress (MAINMENU, FULLMENUNUM (1, 0, 0))->Flags |= CHECKED;
  	else ItemAddress (MAINMENU, FULLMENUNUM (1, 0, 0))->Flags &= ~CHECKED;

    /* set menu iterations flag */
  	for (Index = 0; Index <= 10; Index++)
    {
      	if (((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 2, Index)))->Flags & CHECKED)
    	{
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, Index))->Flags &= ~CHECKED;
    	}
    }

  	switch (MANDChunk->Iterations)
    {
    	case 256:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 0))->Flags |= CHECKED;
      	break;

    	case 320:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 1))->Flags |= CHECKED;
      	break;

    	case 512:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 2))->Flags |= CHECKED;
      	break;

    	case 1024:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 3))->Flags |= CHECKED;
      	break;

    	case 2048:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 4))->Flags |= CHECKED;
      	break;

    	case 4096:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 5))->Flags |= CHECKED;
      	break;

    	case 8192:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 6))->Flags |= CHECKED;
      	break;

    	case 16384:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 7))->Flags |= CHECKED;
      	break;

    	case 32768:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 8))->Flags |= CHECKED;
      	break;

    	default:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 2, 10))->Flags |= CHECKED;
      	break;
    }

	/* set menu power flag */
  	for (Index = 0; Index <= 10; Index++)
    {
      	if (((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 3, Index)))->Flags & CHECKED)
    	{
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, Index))->Flags &= ~CHECKED;
    	}
    }

  	switch (MANDChunk->Power)
    {
    	case 1:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 0))->Flags |= CHECKED;
      	break;

    	case 2:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 1))->Flags |= CHECKED;
      	break;

    	case 3:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 2))->Flags |= CHECKED;
      	break;

    	case 4:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 3))->Flags |= CHECKED;
      	break;

    	case 5:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 4))->Flags |= CHECKED;
      	break;

    	case 6:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 5))->Flags |= CHECKED;
      	break;

    	case 7:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 6))->Flags |= CHECKED;
      	break;

    	case 8:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 7))->Flags |= CHECKED;
      	break;

    	case 9:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 8))->Flags |= CHECKED;
      	break;

    	case 10:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 9))->Flags |= CHECKED;
      	break;

    	case 11:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 3, 10))->Flags |= CHECKED;
      	break;
    }

	/* set menu priority flag */
  	for (Index = 0; Index <= 12; Index++)
    {
      	if (((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 4, Index)))->Flags & CHECKED)
    	{
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, Index))->Flags &= ~CHECKED;
    	}
    }

  	switch (PRIORITY)
    {
    	case -5:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 0))->Flags |= CHECKED;
      	break;

    	case -4:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 1))->Flags |= CHECKED;
      	break;

    	case -3:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 2))->Flags |= CHECKED;
      	break;

    	case -2:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 3))->Flags |= CHECKED;
      	break;

    	case -1:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 4))->Flags |= CHECKED;
      	break;

    	case 0:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 6))->Flags |= CHECKED;
      	break;

    	case 1:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 8))->Flags |= CHECKED;
      	break;

    	case 2:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 9))->Flags |= CHECKED;
      	break;

    	case 3:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 10))->Flags |= CHECKED;
      	break;

    	case 4:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 11))->Flags |= CHECKED;
      	break;

    	case 5:
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 4, 12))->Flags |= CHECKED;
      	break;
    }

	/* set menu color drawing mode flag */
  	for (Index = 0; Index <= 7; Index++)
    {
      	if (((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, Index)))->Flags & CHECKED)
    	{
      		ItemAddress (MAINMENU, FULLMENUNUM (1, 5, Index))->Flags &= ~CHECKED;
    	}
    }

  	if (MANDChunk->Flags & LINEAR_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 0))->Flags |= CHECKED;
  	else if (MANDChunk->Flags & LOG_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 1))->Flags |= CHECKED;
		else if (MANDChunk->Flags & REPEATED_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 2))->Flags |= CHECKED;
  			else if (MANDChunk->Flags & SQUARE_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 3))->Flags |= CHECKED;
  				else if (MANDChunk->Flags & ONE_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 4))->Flags |= CHECKED;
  					else if (MANDChunk->Flags & TWO_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 5))->Flags |= CHECKED;
  						else if (MANDChunk->Flags & THREE_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 6))->Flags |= CHECKED;
  							else if (MANDChunk->Flags & FOUR_BIT) ItemAddress (MAINMENU, FULLMENUNUM (1, 5, 7))->Flags |= CHECKED;

	/* set menu mandel or julia bit flag */
  	if (MANDChunk->Flags & JULIA_BIT)
    {
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 1))->Flags &= ~CHECKED;
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 0))->Flags |= CHECKED;
    }

  	else if (MANDChunk->Flags & MANDEL_BIT)
    {
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 0))->Flags &= ~CHECKED;
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 1))->Flags |= CHECKED;
    }

	/* set menu  rendering flag */
  	if (MANDChunk->Flags & TURBO_BIT)
    {
      	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 0)))->Flags |= CHECKED;
      	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 2)))->Flags |= NM_ITEMDISABLED;
    }

  	else
    {
      	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 0)))->Flags &= ~CHECKED;
      	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 2)))->Flags &= ~NM_ITEMDISABLED;
    }

	/* set histogram coloring flag */
  	if ((MANDChunk->Flags & HISTOGRAM_BIT) && (GetBitMapAttr(Win->RPort->BitMap,BMA_DEPTH) == MIN_DEPTH))
    	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 2)))->Flags |= CHECKED;
  	else
    	((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 7, 2)))->Flags &= ~CHECKED;

	/* set menu high precision flag */
  	if (MANDChunk->Flags & HIGHPREC_BIT)
    {
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 8, 0))->Flags &= ~CHECKED;
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 8, 1))->Flags |= CHECKED;
    }

  	else
    {
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 8, 0))->Flags |= CHECKED;
      	ItemAddress (MAINMENU, FULLMENUNUM (1, 8, 1))->Flags &= ~CHECKED;
    }

  	SetMenuStrip (Win, MAINMENU);
}

/* ProcessMouse() */
void ProcessMouse (struct Window *Win, int16 CurMouseX, int16 CurMouseY, int32 MouseMode)
{
  // static int16 MX1 = 0 , MY1 = 0 , MX2 = 0 , MY2 = 0, W = 0, H = 0;
  struct IntuiMessage *Message = NULL;
  struct IntuiWheelData *MData = NULL;

  int16 MWX0, MWX1, MWY0, MWY1;
  uint16 MyCode, Exit=FALSE;
  int32 DefaultQueue;
  uint32 MyClass;

  	ModifyIDCMP (Win, IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK | IDCMP_RAWKEY | MouseMode);
  	DefaultQueue = SetMouseQueue (Win, 10);
  
  	if (ZMASK & MASK) DrawFrame (Win, MX1, MY1, W, H);
 
  	if (MouseMode == IDCMP_MOUSEMOVE)
  	{
		MX1 = CurMouseX;
        MY1 = CurMouseY;
        W = INITIALZOOM;
        H = ScalerDiv ((uint16) W, (uint16) Win->GZZHeight, (uint16) Win->GZZWidth);  
  	}

  	else if (MouseMode == IDCMP_EXTENDEDMOUSE)
  	{
        MWX0 = MX1;
        MWY0 = MY1;
        MWX1 = W;
        MWY1 = H;
  	}

  	DrawFrame (Win, MX1, MY1, W, H);

  	do
  	{
      	WaitPort (Win->UserPort);

      	while ((!Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort)))
      	{
          	MyClass = Message->Class;
          	MyCode = Message->Code;
          	MX2 = Message->MouseX;
          	MY2 = Message->MouseY;
          	MData = (struct IntuiWheelData *) Message->IAddress;      
          	GT_ReplyIMsg ((struct IntuiMessage *) Message);

          	switch (MyClass)
          	{
              	case IDCMP_MOUSEMOVE:
            	{
                	DrawFrame (Win, MX1, MY1, W, H);
                	W = abs (MX1 - MX2);
                	H = abs (MY1 - MY2);
                	
					if (W > H) (W = ScalerDiv ((uint16) H, (uint16) Win->GZZWidth, (uint16) Win->GZZHeight));
                	else (H = ScalerDiv ((uint16) W, (uint16) Win->GZZHeight, (uint16) Win->GZZWidth));
                	
					DrawFrame (Win, MX1, MY1, W, H);
            	}            	
				break;
            
            	case IDCMP_EXTENDEDMOUSE:
            	{
                	if (MyCode == IMSGCODE_INTUIWHEELDATA)
                	{
                    	if (MData->Version == INTUIWHEELDATA_VERSION)
                    	{                      
                        	DrawFrame (Win, MWX0, MWY0, MWX1, MWY1);
                        
                        	MWX1 += MData->WheelY;                  
                        	
							if (MWX1 < INITIALZOOM) 
                        	{
                            	MWX1 = INITIALZOOM;                                                
                            	DisplayBeep (Win->WScreen);
                        	}
                             
                        	MWY1 = ScalerDiv ((uint16) MWX1, (uint16) Win->GZZHeight, (uint16) Win->GZZWidth);
                        	DrawFrame (Win, MWX0, MWY0, MWX1, MWY1);
                                            
                        	MX1 = MWX0;
                        	MY1 = MWY0;
                        	W = MWX1;
                        	H = MWY1;                    
                    	}
                	}            
            	}
            	break;
            
            	case IDCMP_MENUPICK:    
            	case IDCMP_MOUSEBUTTONS:
            	{
                	Exit = TRUE;
            	}   	     
            	break;
                 
            	case IDCMP_RAWKEY:
            	{
                	if (MyCode == RAW_ESC) Exit = TRUE;
            	}
            	break;
          	}
      	}
    } while (!Exit);

  	SetMouseQueue (Win, (uint32) DefaultQueue);
  	ModifyIDCMP (Win, IDCMP_STANDARD);

  	if (ZMASK & MASK)
  	{
      	OnMenu (Win, FULLMENUNUM (2, 0, NOSUB));
      	OnMenu (Win, FULLMENUNUM (2, 6, NOSUB));
  	}

  	else
  	{
      	OffMenu (Win, FULLMENUNUM (2, 0, NOSUB));
      	OffMenu (Win, FULLMENUNUM (2, 6, NOSUB));
  	}
}

/* Pick() */
int16 PickJuliaK (struct MandelChunk *MandelInfo, struct Window *Win,
        			uint8 * ARGBMem, uint8 * PixMem, uint32 * PixelVector, uint32 * RndMem, uint8 * GfxMem)
{
  struct Window *JuliaPreviewWin = NULL;
  struct IntuiMessage *Message = NULL;
  int16 Selected = FALSE, Exit = FALSE;
  int16 MouseX = 0, MouseY = 0;
  uint16 MyCode = 0;
  uint32 MyClass = NULL;
  mpf_t RealCoord, ImagCoord, TmpJKRE, TmpJKIM;

	JuliaPreviewWin = OpenWindowTags (NULL, WA_Left, WINDOW_X_OFFSET, WA_Top, WINDOW_Y_OFFSET, 
                                      WA_Width, Win->GZZWidth / 4, WA_Height, Win->GZZHeight / 4,    
                                      WA_Title, CATSTR (TXT_PreviewTitle), WA_ScreenTitle, "Julia window preview...",
                                      WA_CustomScreen, Win->WScreen, WA_IDCMP, IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_MENUPICK,
                                      WA_Flags, WFLG_DRAGBAR|WFLG_NOCAREREFRESH|WFLG_SMART_REFRESH|WFLG_RMBTRAP|WFLG_GIMMEZEROZERO|WFLG_NW_EXTENDED,
                                      WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, 
                                      WA_StayTop, TRUE, WA_DropShadows, TRUE, TAG_DONE);

    if (JuliaPreviewWin == NULL) goto ExitPickJuliaK;

    ClearMenuStrip (Win);
    SetWindowTitles (Win, (STRPTR) ~0, CATSTR (TXT_LeftButtonForJulia));
    PutPointer (Win, 0, 0, 0, 0, 0, ZOOM_POINTER);
    mpf_inits (RealCoord, ImagCoord, TmpJKRE, TmpJKIM, 0);

	do
    {
        WaitPort (Win->UserPort);

        while ((! Exit) && (Message = (struct IntuiMessage *) GT_GetIMsg (Win->UserPort)))
        {
			MyClass = Message->Class;
            MyCode = Message->Code;
            MouseX = Message->MouseX;
            MouseY = Message->MouseY;
            GT_ReplyIMsg ((struct IntuiMessage *) Message);
    
            switch (MyClass)
            {
            	case IDCMP_MOUSEMOVE:
                {
					mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                    mpf_abs (RealCoord, gtmp);
                    mpf_mul_ui (RealCoord, RealCoord, MouseX);
                    mpf_div_ui (RealCoord, RealCoord, (Win->GZZWidth - 1));
                    mpf_add (RealCoord, MandelInfo->GRMin, RealCoord);
					//  RealCoord = MandelInfo->RMin + ((float64) MouseX * fabs (MandelInfo->RMax-MandelInfo->RMin) / (float64) (Win->GZZWidth - 1));
                    mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                    mpf_abs (ImagCoord, gtmp);
                    mpf_mul_ui (ImagCoord, ImagCoord, MouseY);
                    mpf_div_ui (ImagCoord, ImagCoord, (Win->GZZHeight - 1));
                    mpf_add (ImagCoord, MandelInfo->GIMin, ImagCoord);
					// ImagCoord = MandelInfo->IMax - ((float64) MouseY * fabs (MandelInfo->IMax-MandelInfo->IMin) / (float64) (Win->GZZHeight - 1));
					// ImagCoord = MandelInfo->IMin + ((float64) MouseY * fabs (MandelInfo->IMax-MandelInfo->IMin) / (float64) (Win->GZZHeight - 1));

                    if (TMASK & MASK)
                    {
						// Julia constant: Real %+2.30Ff Imag %+2.30fi
                        gmp_snprintf (BAR_STRING, sizeof (BAR_STRING), CATSTR (TXT_JuliaConstant), RealCoord, ImagCoord);
						// sprintf (BAR_STRING, CATSTR(TXT_JuliaConstant),mpf_get_d (RealCoord), mpf_get_d (ImagCoord));
                        WaitTOF ();    // try to reduce titlebar flicker
                        SetWindowTitles (Win, (STRPTR) ~0, BAR_STRING);
                        WaitTOF ();
                        Delay (ONESEC / 5);
                    }
      
                    mpf_swap (MandelInfo->GJKre, RealCoord);
                    mpf_swap (MandelInfo->GJKim, ImagCoord);
                    WaitTOF ();
                    DrawFractal (MandelInfo, JuliaPreviewWin, ARGBMem, PixMem, GfxMem, PixelVector, RndMem, FALSE);
                    WaitTOF ();
                    mpf_swap (MandelInfo->GJKre, RealCoord);
                    mpf_swap (MandelInfo->GJKim, ImagCoord);
                }
				break;
                    
                case IDCMP_MOUSEBUTTONS:
                {
				    if (MyCode == SELECTDOWN)
                    {
                        mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                        mpf_abs (MandelInfo->GJKre, gtmp);
                        mpf_mul_ui (MandelInfo->GJKre, MandelInfo->GJKre, MouseX);
                        mpf_div_ui (MandelInfo->GJKre, MandelInfo->GJKre, (Win->GZZWidth - 1));
                        mpf_add (MandelInfo->GJKre, MandelInfo->GRMin, MandelInfo->GJKre);
						//  MandelInfo->JKre = MandelInfo->RMin + ((float64) MouseX * fabs (MandelInfo->RMax-MandelInfo->RMin) / (float64) (Win->GZZWidth - 1));
                        mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                        mpf_abs (MandelInfo->GJKim, gtmp);
                        mpf_mul_ui (MandelInfo->GJKim, MandelInfo->GJKim, MouseY);
                        mpf_div_ui (MandelInfo->GJKim, MandelInfo->GJKim, (Win->GZZHeight - 1));
                        mpf_add (MandelInfo->GJKim, MandelInfo->GIMin, MandelInfo->GJKim);
						// MandelInfo->JKim = MandelInfo->IMax - ((float64) MouseY * fabs (MandelInfo->IMax-MandelInfo->IMin) / (float64) (Win->GZZHeight - 1));
						// MandelInfo->JKim = MandelInfo->IMin + ((float64) MouseY * fabs (MandelInfo->IMax-MandelInfo->IMin) / (float64) (Win->GZZHeight - 1));

                        MandelInfo->JKre = mpf_get_d (MandelInfo->GJKre);
                        MandelInfo->JKim = mpf_get_d (MandelInfo->GJKim);

                        Exit = Selected = TRUE;
                    }
                }
				break;
    
                case IDCMP_RAWKEY:
                {
					if (MyCode == RAW_ESC)
                    {
                    	Exit = TRUE;
                        ((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 0)))->Flags &= ~CHECKED;
                        ((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 1)))->Flags |= CHECKED;
                    }
                }
				break;
    
                case IDCMP_MENUPICK:
                {
				    Exit = TRUE;
                    ((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 0)))->Flags &= ~CHECKED;
                    ((struct MenuItem *) ItemAddress (MAINMENU, FULLMENUNUM (1, 6, 1)))->Flags |= CHECKED;
                }
				break;
            } // switch
        } // while            
    } while (Exit != TRUE); // do while

    CloseWindow (JuliaPreviewWin);
    ResetMenuStrip (Win, MAINMENU);
    ModifyIDCMP (Win, IDCMP_STANDARD);
    mpf_clears (RealCoord, ImagCoord, TmpJKRE, TmpJKIM, 0);

ExitPickJuliaK:    
	return (Selected);
}

/* HandleEvents() */
uint32 HandleEvents (struct ILBMInfo *Ilbm, struct MandelChunk *MandelInfo)
{
  struct IntuiMessage *Message = NULL;
  BPTR MyFile;
  int16 MouseX = 0, MouseY = 0, New_Granularity = 0;
  uint16 MyCode = 0;
  int32 Result = 0;
  uint32 MyClass = NULL, MyMenu = NULL, Scrl_Zoom_Step = 10;
  mpf_t RealCoord, ImagCoord, ScrRatio, FracRatio;
  mp_exp_t exp = 0;

  	mpf_inits (RealCoord, ImagCoord, ScrRatio, FracRatio, 0);
  	wsignal = 1 << Ilbm->win->UserPort->mp_SigBit; /* new for ARexx-support */

  	while (!((MyMenu == EXIT_MSG) || (MyMenu & NEWDISPLAY_MSG)))
  	{
      	ClearPointer (Ilbm->win);
#ifdef  FM_AREXX_SUPPORT /* new for ARexx-support */  		
		receivedsig = Wait (wsignal | AREXXSIGNAL);

      	if (receivedsig & AREXXSIGNAL)
      	{          	
            IDoMethod (AREXXOBJ, AM_HANDLEEVENT);
          	MyMenu = HandleARexxEvents (Ilbm);
      	}
#else /* !FM_AREXX_SUPPORT */
      	receivedsig = Wait (wsignal);
#endif /* FM_AREXX_SUPPORT */
     	if (receivedsig & wsignal)  	
		{
           	while ((!((MyMenu == EXIT_MSG) || (MyMenu & NEWDISPLAY_MSG))) && (Message = (struct IntuiMessage *) GT_GetIMsg (Ilbm->win->UserPort)))
         	{
             	MyClass = Message->Class;
              	MyCode = Message->Code;
              	MouseX = Message->MouseX;
              	MouseY = Message->MouseY;
              	GT_ReplyIMsg ((struct IntuiMessage *) Message);

				if (New_Granularity)
				{	
					sprintf (BAR_STRING, "Keyboard Scroll and Zoom granularity set to 1/%ld", Scrl_Zoom_Step);
					WaitTOF ();                                            
                    SetWindowTitles (Ilbm->win, (STRPTR) ~0, BAR_STRING);
					New_Granularity = 0;
				}

              	switch (MyClass)
            	{
                	case IDCMP_MOUSEMOVE:
                    {
						if (TMASK & MASK)
                        {
                        	mpf_set_si (ScrRatio, Ilbm->win->GZZWidth);
                            mpf_div_ui (ScrRatio, ScrRatio, Ilbm->win->GZZHeight);
                            mpf_sub (FracRatio, MandelInfo->GRMax, MandelInfo->GRMin);
                            mpf_abs (FracRatio, FracRatio);
                            mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                            mpf_abs (gtmp, gtmp);
                            mpf_div (FracRatio, FracRatio, gtmp);

                            mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                            mpf_abs (RealCoord, gtmp);
                            mpf_mul_ui (RealCoord, RealCoord, MouseX);
                            mpf_div_ui (RealCoord, RealCoord, (Ilbm->win->GZZWidth - 1));
                            mpf_add (RealCoord, MandelInfo->GRMin, RealCoord);
							//  RealCoord = MandelInfo->RMin + ((float64) MouseX * fabs (MandelInfo->RMax-MandelInfo->RMin) / (float64) (Win->GZZWidth - 1));
                            mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                            mpf_abs (ImagCoord, gtmp);
                            mpf_mul_ui (ImagCoord, ImagCoord, MouseY);
                            mpf_div_ui (ImagCoord, ImagCoord, (Ilbm->win->GZZHeight - 1));
                            mpf_sub (ImagCoord, MandelInfo->GIMax, ImagCoord);
							//  ImagCoord = MandelInfo->IMax - ((float64) MouseY * fabs (MandelInfo->IMax-MandelInfo->IMin) / (float64) (Win->GZZHeight - 1));

                            gmp_snprintf (BAR_STRING, sizeof (BAR_STRING), "Cr %#+2.20Ff Ci %#+2.20Ffi W:H %.3Ff R:I %.3Ff Mx %#4d My %4d W %#4d H %#4d",
                                        	RealCoord, ImagCoord, ScrRatio, FracRatio, MouseX + 1, MouseY + 1, Ilbm->win->GZZWidth, Ilbm->win->GZZHeight);

							//  sprintf (BAR_STRING,"Cr %#+2.16f Ci %#+2.16fi W:H %.3f R:I %.3f Mx %#4d My %4d W %#4d H %#4d",RealCoord, ImagCoord, ScrRatio, FracRatio, MouseX, MouseY, Ilbm->win->GZZWidth, Ilbm->win->GZZHeight);                             
                       		WaitTOF ();                                            
                            SetWindowTitles (Ilbm->win, (STRPTR) ~0, BAR_STRING);
                        }
                  	}
					break;                                                            

                	case IDCMP_EXTENDEDMOUSE:
                    {
						PutPointer (Ilbm->win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
                        ProcessMouse (Ilbm->win, MouseX, MouseY, IDCMP_EXTENDEDMOUSE);                                                    
                	}
					break;                      
        
                	case IDCMP_MOUSEBUTTONS:
                    {
						switch (MyCode)
                        {
                        	case SELECTDOWN:
                            {
                            	PutPointer (Ilbm->win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
                                ProcessMouse (Ilbm->win, MouseX, MouseY, IDCMP_MOUSEMOVE);
                            }
                            break;            

                            case MIDDLEDOWN:
                            {
                                MyMenu |= DRAW_MSG;
                                goto DRAW; /* I know not nice to see but it works! */
                            }
                            break;         
                        }    
					}
                	break;
                    
                	case IDCMP_RAWKEY:
					{
						switch (MyCode)
						{
							case RAW_UPARROW:
							{	
							  	/* save coordinates */
  								SaveCoords (Ilbm->win);

								mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* scroll 1/Scrl_Zoom_Step of fractal plane */                           
								mpf_add (MandelInfo->GIMax, MandelInfo->GIMax, gtmp);
								mpf_add (MandelInfo->GIMin, MandelInfo->GIMin, gtmp);                            	

  								MANDChunk->IMax = mpf_get_d (MANDChunk->GIMax);
  								MANDChunk->IMin = mpf_get_d (MANDChunk->GIMin);
								
								MyMenu |= REDRAW_MSG;
                                goto REDRAW; /* I know not very nice to see but it works! */
							}
							break;

							case RAW_DNARROW:
							{
							  	/* save coordinates */
  								SaveCoords (Ilbm->win);

								mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* scroll 1/Scrl_Zoom_Step of fractal plane */                           
								mpf_sub (MandelInfo->GIMax, MandelInfo->GIMax, gtmp);
								mpf_sub (MandelInfo->GIMin, MandelInfo->GIMin, gtmp);                            	
								
 								MANDChunk->IMax = mpf_get_d (MANDChunk->GIMax);
  								MANDChunk->IMin = mpf_get_d (MANDChunk->GIMin);

								MyMenu |= REDRAW_MSG;
                                goto REDRAW; /* I know not very nice to see but it works! */							
							}
							break;

							case RAW_RHARROW:
							{
							  	/* save coordinates */
  								SaveCoords (Ilbm->win);

								mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* scroll 1/Scrl_Zoom_Step of fractal plane */                           
								mpf_add (MandelInfo->GRMax, MandelInfo->GRMax, gtmp);
								mpf_add (MandelInfo->GRMin, MandelInfo->GRMin, gtmp);                            	

								MANDChunk->RMax = mpf_get_d (MANDChunk->GRMax);
  								MANDChunk->RMin = mpf_get_d (MANDChunk->GRMin);
								
								MyMenu |= REDRAW_MSG;
                                goto REDRAW; /* I know not very nice to see but it works! */							
							}
							break;

							case RAW_LFARROW:
							{
							  	/* save coordinates */
  								SaveCoords (Ilbm->win);

								mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* scroll 1/Scrl_Zoom_Step of fractal plane */
								mpf_sub (MandelInfo->GRMax, MandelInfo->GRMax, gtmp);
								mpf_sub (MandelInfo->GRMin, MandelInfo->GRMin, gtmp);

								MANDChunk->RMax = mpf_get_d (MANDChunk->GRMax);
  								MANDChunk->RMin = mpf_get_d (MANDChunk->GRMin);

								MyMenu |= REDRAW_MSG;
                                goto REDRAW; /* I know not very nice to see but it works! */							
							}
							break;
																											
							case RAW_ESC:
							{
                           		MyMenu = EXIT_MSG;
                       		}
							break;

                       		case RAW_HELP:
                       		{
                           		if (MyFile = Open ("CON:0/40/640/150/Flashmandel window/auto/close/wait", MODE_OLDFILE))
                           		{
                               		Result = SystemTags ((STRPTR) CATSTR (TXT_CMD_Guide), SYS_Input, MyFile, SYS_Output, NULL, SYS_Asynch, TRUE, TAG_DONE);
                               		if (Result == -1) Close (MyFile);
                           		}
                        	}
							break;							                  											
												
							case RAW_1:
							{
								Scrl_Zoom_Step = 11;
								New_Granularity = 1;													
							}
							break;

							case RAW_2:
							{
								Scrl_Zoom_Step = 12;
								New_Granularity = 1;																	
							}
							break; 

							case RAW_3:
							{
								Scrl_Zoom_Step = 3;
								New_Granularity = 1;									
							}
							break;							
													
							case RAW_4:
							{
								Scrl_Zoom_Step = 4;
								New_Granularity = 1;																
							}
							break;
							
							case RAW_5:
							{
								Scrl_Zoom_Step = 5;
								New_Granularity = 1;								
							}
							break;
							
							case RAW_6:
							{
								Scrl_Zoom_Step = 6;
								New_Granularity = 1;								
							}
							break;
							
							case RAW_7:
							{
								Scrl_Zoom_Step = 7;
								New_Granularity = 1;									
							}
							break;
							
							case RAW_8:
							{
								Scrl_Zoom_Step = 8;
								New_Granularity = 1;								
							}
							break;
							
							case RAW_9:
							{
								Scrl_Zoom_Step = 9;
								New_Granularity = 1;									
							}
							break;
							
							case RAW_0:
							{
								Scrl_Zoom_Step = 10;
								New_Granularity = 1;							
							}
							break;							
																																										
							case RAW_i:
							{	
							  	/* save coordinates */
  								SaveCoords (Ilbm->win);

								mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* zoom 1/Scrl_Zoom_Step of fractal plane */                           
								mpf_sub (MandelInfo->GIMax, MandelInfo->GIMax, gtmp);
								mpf_add (MandelInfo->GIMin, MandelInfo->GIMin, gtmp);                            	

								mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* zoom 1/Scrl_Zoom_Step of fractal plane */
								mpf_sub (MandelInfo->GRMax, MandelInfo->GRMax, gtmp);
								mpf_add (MandelInfo->GRMin, MandelInfo->GRMin, gtmp);

								MANDChunk->RMax = mpf_get_d (MANDChunk->GRMax);
  								MANDChunk->RMin = mpf_get_d (MANDChunk->GRMin);
  								MANDChunk->IMax = mpf_get_d (MANDChunk->GIMax);
  								MANDChunk->IMin = mpf_get_d (MANDChunk->GIMin);
								
								MyMenu |= REDRAW_MSG;
                                goto REDRAW; /* I know not very nice to see but it works! */
							}
							break;

							case RAW_o:
							{	
							  	/* save coordinates */
  								SaveCoords (Ilbm->win);

								mpf_sub (gtmp, MandelInfo->GIMax, MandelInfo->GIMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* zoom 1/Scrl_Zoom_Step of fractal plane */                           
								mpf_add (MandelInfo->GIMax, MandelInfo->GIMax, gtmp);
								mpf_sub (MandelInfo->GIMin, MandelInfo->GIMin, gtmp);                            	

								mpf_sub (gtmp, MandelInfo->GRMax, MandelInfo->GRMin);
                            	mpf_abs (gtmp, gtmp);
                            	mpf_div_ui (gtmp, gtmp, Scrl_Zoom_Step); /* zoom 1/Scrl_Zoom_Step of fractal plane */
								mpf_add (MandelInfo->GRMax, MandelInfo->GRMax, gtmp);
								mpf_sub (MandelInfo->GRMin, MandelInfo->GRMin, gtmp);

								MANDChunk->RMax = mpf_get_d (MANDChunk->GRMax);
  								MANDChunk->RMin = mpf_get_d (MANDChunk->GRMin);
  								MANDChunk->IMax = mpf_get_d (MANDChunk->GIMax);
  								MANDChunk->IMin = mpf_get_d (MANDChunk->GIMin);
								
								MyMenu |= REDRAW_MSG;
                                goto REDRAW; /* I know not very nice to see but it works! */
							}
							break;							
						}
					}
					break;
						
                	case IDCMP_MENUPICK:
                    {
						MyMenu = ProcessMenu (MandelInfo, Ilbm->win, ARGBMEM, PIXMEM, PIXELVECTOR, RNDMEM, GFXMEM, MyCode);

                        if (MyMenu & EXIT_MSG) break;

                        if (MyMenu & SHOWGUIDE_MSG)
                        {
                            if (MyFile = Open ("CON:0/40/640/150/Flashmandel window/auto/close/wait", MODE_OLDFILE))
                          	{
                               	Result = SystemTags ((STRPTR) CATSTR (TXT_CMD_Guide), SYS_Input, MyFile, SYS_Output, NULL, SYS_Asynch, TRUE, TAG_DONE);
                               	if (Result == -1) Close (MyFile);
                          	}
                          	break;
                       	}

                        if (MyMenu & TITLE_MSG)
                       	{
                            if (TMASK & MASK)
                          	{
                               	ItemAddress (MAINMENU, FULLMENUNUM (1, 0, 0))->Flags &= ~CHECKED;
                               	Ilbm->TBState = FALSE;
                               	MASK &= ~TMASK;
                          	}

                           	else
                          	{
                               	ItemAddress (MAINMENU, FULLMENUNUM (1, 0, 0))->Flags |= CHECKED;
                               	Ilbm->TBState = TRUE;
                               	MASK |= TMASK;
                          	}

                            ShowTitle (Ilbm->win->WScreen, (int32) Ilbm->TBState);
                            break;
                       	}

                        if (MyMenu & TIME_MSG)
                       	{
                            ShowTitle (Ilbm->win->WScreen, TRUE);
                            ShowTime (Ilbm->win, CATSTR (TXT_LastCalcTime), ELAPSEDTIME), Delay (TWOSECS);
                            if (!(TMASK & MASK)) ShowTitle (Ilbm->win->WScreen, FALSE);
                            break;
                       	}

                        if (MyMenu & ITER_MSG)
                       	{
                            ModifyIDCMP (Ilbm->win, NULL);
                            ClearMenuStrip (Ilbm->win);
                            while (1L)
                            {
#ifdef FM_REACT_SUPPORT
                            	res = Do_IterationsRequest (Ilbm->win, FMSCREENNAME, 0, 0, MandelInfo->Iterations);
#else /* FM_REACT_SUPPORT */
                                res = IntegerGad (Ilbm->win, CATSTR (TITLE_IterationsReq),    CATSTR (TXT_ScrTitle_It), CATSTR (It_TXT_Iterations), MandelInfo->Iterations);
#endif /* FM_REACT_SUPPORT */
                                if ((res >= MIN_ITERATIONS) && (res <= MAX_ALLOWED_ITERATIONS)) break;
                                DisplayBeep (Ilbm->win->WScreen);
                            }

                            MandelInfo->Iterations = res;
                            CheckMenu (Ilbm->win);
                            ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                            break;
                       	}

                        if (MyMenu & ABOUT_MSG)
                       	{
                           	ModifyIDCMP (Ilbm->win, NULL);
                            ClearMenuStrip (Ilbm->win);
                            ShowTitle (Ilbm->win->WScreen, FALSE);
#ifdef  FM_REACT_SUPPORT
                            Do_InfoRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
                            About (Ilbm->win);
#endif /* FM_REACT_SUPPORT */
                            if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);
                            ResetMenuStrip (Ilbm->win, MAINMENU);
                            ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                            break;
                       	}

                        if (MyMenu & SYSINFO_MSG)
                       	{
                           	ModifyIDCMP (Ilbm->win, NULL);
                            ClearMenuStrip (Ilbm->win);
                            ShowTitle (Ilbm->win->WScreen, FALSE);
#ifdef FM_REACT_SUPPORT
                            Do_SysInfoRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
                            SystemInfo (Ilbm->win);
#endif /* FM_REACT_SUPPORT */
                            if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);
                            ResetMenuStrip (Ilbm->win, MAINMENU);
                            ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                            break;
                       	}

                        if (MyMenu & PALETTE_MSG)
                       	{
                            ModifyIDCMP (Ilbm->win, NULL);
                            ClearMenuStrip (Ilbm->win);
#ifdef FM_REACT_SUPPORT
                            res = Do_PalettePrefRequest (Ilbm->win, FMSCREENNAME, 0, 0, (struct loadrgb *) PALETTE);
#else /* FM_REACT_SUPPORT */
                            res = ModifyPalette (Ilbm->win, WINDOW_X_OFFSET, WINDOW_Y_OFFSET, PALETTE);
#endif /* FM_REACT_SUPPORT */
                            if (!res) DisplayError (Ilbm->win, TXT_ERR_PaletteRequester, 15L);
                            ResetMenuStrip (Ilbm->win, MAINMENU);
                            ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                            break;
                       	}

                        if (MyMenu & CYCLERIGHT_MSG)
                       	{
                            ClearZoomFrame (Ilbm->wrp);                          

                            ModifyIDCMP (Ilbm->win, IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY | IDCMP_MENUPICK);
                            ShowTitle (Ilbm->win->WScreen, FALSE);
                            ClearMenuStrip (Ilbm->win);
                            PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                            Cycle (Ilbm->win, DELAY, SHIFTRIGHT);
                            ResetMenuStrip (Ilbm->win, MAINMENU);
                            if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);
                            LoadRGB32 (Ilbm->vp, PALETTE);
                            ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                            break;
                       	}

                        if (MyMenu & CYCLELEFT_MSG)
                       	{
                            ClearZoomFrame (Ilbm->wrp);                            						
 
                            ModifyIDCMP (Ilbm->win, IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY | IDCMP_MENUPICK);
                            ShowTitle (Ilbm->win->WScreen, FALSE);
                            ClearMenuStrip (Ilbm->win);
                            PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                            Cycle (Ilbm->win, DELAY, SHIFTLEFT);
                            ResetMenuStrip (Ilbm->win, MAINMENU);
                            if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);
                            LoadRGB32 (Ilbm->vp, PALETTE);
                            ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                            break;
                       	}

                        if (MyMenu & DELAY_MSG)
                       	{
                            while (1L)
                       	    {
#ifdef FM_REACT_SUPPORT
                            	res = Do_CycleDelayRequest (Ilbm->win, FMSCREENNAME, 0, 0, DELAY);
#else /* FM_REACT_SUPPORT */
                                res = IntegerGad (Ilbm->win, CATSTR (TITLE_CycleDelayReq), CATSTR (TXT_ScrTitle_Cyc), CATSTR (Cyc_TXT_DelayTime), DELAY);
#endif /* FM_REACT_SUPPORT */
                                if ((res >= MIN_DELAY) && (DELAY <= MAX_DELAY)) break;
                                DisplayBeep (Ilbm->win->WScreen);
                           	}

                            DELAY = res;
                            break;
                       	}

                        if (MyMenu & STOP_MSG)
                       	{
                            SetMenuStart (Ilbm, UNDOCOUNTER);
                            break;
                       	}

                        if (MyMenu & LOADPICTURE_MSG)
                       	{
                            if (FileRequest (Ilbm->win, CATSTR (TXT_LoadPictureTitle), "Pictures", PICTURES_DRAWER, FALSE))
                           	{
                                PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                                SaveCoords (Ilbm->win);

                                if (! (QueryMandFile (Ilbm, LSFMChunk, MYPATH)))
                                {
                                	MandelInfo->LeftEdge = LSFMChunk->LeftEdge;
                                    MandelInfo->TopEdge = LSFMChunk->TopEdge;
                                    MandelInfo->Width = LSFMChunk->Width;
                                    MandelInfo->Height = LSFMChunk->Height;
                                    MandelInfo->Iterations = LSFMChunk->Iterations;
                                    MandelInfo->PixelFormat = LSFMChunk->PixelFormat;
                                    MandelInfo->Modulo = LSFMChunk->Modulo;
                                    MandelInfo->Flags = LSFMChunk->Flags;
                                    MandelInfo->Power = LSFMChunk->Power;
                                    MandelInfo->PrecisionDigits = LSFMChunk->PrecisionDigits;
                                    MandelInfo->Depth = LSFMChunk->Depth;
                                    MandelInfo->ModeID = LSFMChunk->ModeID;
                                    MandelInfo->PrecisionBits = LSFMChunk->PrecisionBits;

                                    MandelInfo->RMin = LSFMChunk->RMin;
                                    MandelInfo->IMin = LSFMChunk->IMin;
                                    MandelInfo->RMax = LSFMChunk->RMax;
                                    MandelInfo->IMax = LSFMChunk->IMax;
                                    MandelInfo->JKre = LSFMChunk->JKre;
                                    MandelInfo->JKim = LSFMChunk->JKim;
									//  copy coords from float64 values
									/*  mpf_set_d (MandelInfo->GRMax,MandelInfo->RMax);
                                   	mpf_set_d (MandelInfo->GRMin,MandelInfo->RMin);
                                   	mpf_set_d (MandelInfo->GIMax,MandelInfo->IMax);
                                   	mpf_set_d (MandelInfo->GIMin,MandelInfo->IMin);
                                   	mpf_set_d (MandelInfo->GJKre,MandelInfo->JKre);
                                   	mpf_set_d (MandelInfo->GJKim,MandelInfo->JKim); */
									//  copy coords from string base 10 GMP values
                                    mpf_set_str (MandelInfo->GRMin, &LSFMChunk->GRMinSTR, 10);
                                    mpf_set_str (MandelInfo->GIMin, &LSFMChunk->GIMinSTR, 10);
                                    mpf_set_str (MandelInfo->GRMax, &LSFMChunk->GRMaxSTR, 10);
                                    mpf_set_str (MandelInfo->GIMax, &LSFMChunk->GIMaxSTR, 10);
                                    mpf_set_str (MandelInfo->GJKre, &LSFMChunk->GJKreSTR, 10);
                                    mpf_set_str (MandelInfo->GJKim, &LSFMChunk->GJKimSTR, 10);

                       	            Ilbm->camg = BestModeID (BIDTAG_NominalWidth, MandelInfo->Width,
									                        	BIDTAG_DesiredWidth, MandelInfo->Width,
                                                            	BIDTAG_NominalHeight, MandelInfo->Height,
                                                            	BIDTAG_DesiredHeight, MandelInfo->Height,
                                                            	BIDTAG_Depth, MandelInfo->Depth,
                                                            	BIDTAG_DIPFMustNotHave, (DIPF_IS_DUALPF|DIPF_IS_PF2PRI|DIPF_IS_HAM|DIPF_IS_EXTRAHALFBRITE|DIPF_IS_PAL), 
                                    							TAG_DONE);

                                    if (Ilbm->camg == INVALID_ID)
                                   	{
                                        DisplayError (Ilbm->win, TXT_ERR_ModeNotAvailable, 20L);
                                        break;
                                   	}

                                    Ilbm->Bmhd.w = MandelInfo->Width;
                                    Ilbm->Bmhd.h = MandelInfo->Height;
                                    Ilbm->Bmhd.nPlanes = MandelInfo->Depth;
									// if (MandelInfo->Depth == MAX_DEPTH)  MandelInfo->Flags |= TURBO_BIT;

                                    Fade (Ilbm->win, ARGBMEM, PALETTE, 25L, 1L, TOBLACK);
                                    CloseDisplay (Ilbm);

                                    if (!MakeDisplay (Ilbm))
                                   	{
                                        DisplayError (Ilbm->win, TXT_ERR_MakeDisplay, 20L);
                                   	    CloseDisplay (Ilbm);
                                        MyMenu = EXIT_MSG;
                                        break;
                                   	}

                                    MASK &= ~ZMASK;
                                    ShowTitle (Ilbm->win->WScreen, FALSE);
                                    if (LoadMandPic (Ilbm, MYPATH)) DisplayError (Ilbm->win, TXT_ERR_LoadMandPic, 5L);
                                   	if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);
                                    GetRGB32 (Ilbm->vp->ColorMap, 0, Ilbm->vp->ColorMap->Count, (PALETTE + 1L));
                                    SetMenuStart (Ilbm, UNDOCOUNTER);
                               	}

                                else
                                {
                           			ClearZoomFrame (Ilbm->wrp);                                   
                                   	DisplayError (Ilbm->win, TXT_ERR_QueryMandPic, 0);
                                }    
                           	}                            
							break;
                       	}

                        if (MyMenu & SAVEPICTURE_MSG)
                       	{
                            if (FileRequest (Ilbm->win, CATSTR (TXT_SavePictureTitle), "Pictures", PICTURES_DRAWER, TRUE))
                           	{
                                PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                                LSFMChunk->LeftEdge = MandelInfo->LeftEdge;
                                LSFMChunk->TopEdge = MandelInfo->TopEdge;
                                LSFMChunk->Width = MandelInfo->Width;
                                LSFMChunk->Height = MandelInfo->Height;
                                LSFMChunk->Iterations = MandelInfo->Iterations;
                                LSFMChunk->PixelFormat = MandelInfo->PixelFormat;
                                LSFMChunk->Modulo = MandelInfo->Modulo;
                                LSFMChunk->Flags = MandelInfo->Flags;
                                LSFMChunk->Power = MandelInfo->Power;
                                LSFMChunk->PrecisionDigits = MandelInfo->PrecisionDigits;
                                LSFMChunk->Depth = MandelInfo->Depth;
                                LSFMChunk->ModeID = MandelInfo->ModeID;
                                LSFMChunk->PrecisionBits = MandelInfo->PrecisionBits;

                                LSFMChunk->RMin = MandelInfo->RMin;
                                LSFMChunk->IMin = MandelInfo->IMin;
                                LSFMChunk->RMax = MandelInfo->RMax;
                                LSFMChunk->IMax = MandelInfo->IMax;
                                LSFMChunk->JKre = MandelInfo->JKre;
                                LSFMChunk->JKim = MandelInfo->JKim;

								// gmp_printf ("save0 gimax %+2.20Ff grmax %+2.20Ff gimin %+2.20Ff grmin %+2.20Ff jkre %+2.20Ff jkim %+2.20Ff\n",LSFMChunk->GIMax,LSFMChunk->GRMax,LSFMChunk->GIMin,LSFMChunk->GRMin,LSFMChunk->GJKre,LSFMChunk->GJKim);
								/* GMP BUG in mpf_get_str function prevents use it to store coordinates into multiprecision format */
								// strcpy (LSFMChunk->GRMinSTR,mpf_get_str (/*&LSFMChunk->GRMinSTR*/ 0,&exp,10,0,MandelInfo->GRMin));
								// strcpy (LSFMChunk->GIMinSTR,mpf_get_str (/*&LSFMChunk->GIMinSTR*/ 0,&exp,10,0,MandelInfo->GIMin));
								// strcpy (LSFMChunk->GRMaxSTR,mpf_get_str (/*&LSFMChunk->GRMaxSTR*/ 0,&exp,10,0,MandelInfo->GRMax));
								// strcpy (LSFMChunk->GIMaxSTR,mpf_get_str (/*&LSFMChunk->GIMaxSTR*/ 0,&exp,10,0,MandelInfo->GIMax));
								// strcpy (LSFMChunk->GJKreSTR,mpf_get_str (/*&LSFMChunk->GJKreSTR*/ 0,&exp,10,0,MandelInfo->GJKre));
								// strcpy (LSFMChunk->GJKimSTR,mpf_get_str (/*&LSFMChunk->GJKimSTR*/ 0,&exp,10,0,MandelInfo->GJKim));
								// gmp_printf ("save_gmp0 gimax %+2.16Ff grmax %+2.16Ff gimin %+2.16Ff grmin %+2.16Ff jkre %+2.16Ff jkim %+2.16Ff\n",MandelInfo->GIMax,MandelInfo->GRMax,MandelInfo->GIMin,MandelInfo->GRMin,MandelInfo->GJKre,MandelInfo->GJKim);

                                gmp_snprintf (LSFMChunk->GRMinSTR, MandelInfo->PrecisionDigits, "%.1235Ff", MandelInfo->GRMin);
                                gmp_snprintf (LSFMChunk->GRMaxSTR, MandelInfo->PrecisionDigits, "%.1235Ff", MandelInfo->GRMax);
                                gmp_snprintf (LSFMChunk->GIMinSTR, MandelInfo->PrecisionDigits, "%.1235Ff", MandelInfo->GIMin);
                                gmp_snprintf (LSFMChunk->GIMaxSTR, MandelInfo->PrecisionDigits, "%.1235Ff", MandelInfo->GIMax);
                                gmp_snprintf (LSFMChunk->GJKreSTR, MandelInfo->PrecisionDigits, "%.1235Ff", MandelInfo->GJKre);
                                gmp_snprintf (LSFMChunk->GJKimSTR, MandelInfo->PrecisionDigits, "%.1235Ff", MandelInfo->GJKim);

                           		ClearZoomFrame (Ilbm->wrp);
                                
                                ShowTitle (Ilbm->win->WScreen, FALSE);
                                GetRGB32 (Ilbm->vp->ColorMap, 0, Ilbm->vp->ColorMap->Count, (PALETTE + 1L));
                                if (SaveMandPic  (Ilbm, LSFMChunk, USERNAME_STRING, COPYRIGHT_STRING, MYPATH)) DisplayError (Ilbm->win, TXT_ERR_SaveMandPic, 5L);
                                if (TMASK & MASK) ShowTitle (Ilbm->win->WScreen, TRUE);                                  
                          	}
                          	break;
                      	}

                        if (MyMenu & LOADPALETTE_MSG)
                      	{
                            if (FileRequest (Ilbm->win, CATSTR (TXT_LoadPaletteTitle), "Palettes", PALETTES_DRAWER, FALSE))
                          	{
                                PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);

                                if (Ilbm->ParseInfo.iff = AllocIFF ())
                              	{
                                    Fade (Ilbm->win, ARGBMEM, PALETTE, 25L, 1L, TOBLACK);								
                                    if (LoadPalette (Ilbm, MYPATH)) DisplayError (Ilbm->win, TXT_ERR_LoadMandPal, 5L);
                                  	GetRGB32 (Ilbm->vp->ColorMap, 0, Ilbm->vp->ColorMap->Count, (PALETTE + 1L));
                                    FreeIFF (Ilbm->ParseInfo.iff);
                              	}

                                else DisplayError (Ilbm->win, TXT_ERR_NoMem, 5L);
                          	}
                          	break;
                      	}

                        if (MyMenu & SAVEPALETTE_MSG)
                      	{
                            if (FileRequest (Ilbm->win, CATSTR (TXT_SavePaletteTitle), "Palettes", PALETTES_DRAWER, TRUE))
                          	{
                              	PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);

                                if (Ilbm->ParseInfo.iff = AllocIFF ())
                              	{
                                  	if (SavePalette (Ilbm, &USERNAME_CHUNK, &COPYRIGHT_CHUNK, MYPATH)) DisplayError (Ilbm->win, TXT_ERR_SaveMandPal, 5L);
                                    FreeIFF (Ilbm->ParseInfo.iff);
                              	}

                                else DisplayError (Ilbm->win, TXT_ERR_NoMem, 5L);
                          	}
                            break;
                      	}

                        if (MyMenu & FONTREQ_MSG)
                      	{
                            if (FontRequest (Ilbm->win))
                          	{
                                PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);

                           		ClearZoomFrame (Ilbm->wrp);                                
 
                                MYBITMAP = CopyBitMap (Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) Ilbm->win->GZZWidth, (uint16) Ilbm->win->GZZHeight);
                                Fade (Ilbm->win, ARGBMEM, PALETTE, 25L, 1L, TOBLACK);
                                CloseDisplay (Ilbm);

                                if (!(MakeDisplay (Ilbm)))
                              	{
                                    if (MASK & BMASK)
                                  	{
                                        if (MYBITMAP)
                                      	{
                                            FreeBitMapSafety (MYBITMAP);
                                            MYBITMAP = NULL;
                                            MASK &= ~BMASK;
                                      	}
                                  	}

                                    DisplayError (Ilbm->win, TXT_ERR_MakeDisplay, 20L);
                                    CloseDisplay (Ilbm);
                                    MyMenu = EXIT_MSG;
                                    break;
                              	}

                                PasteBitMap (MYBITMAP, Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) Ilbm->win->GZZWidth, (uint16) Ilbm->win->GZZHeight);
                                SetMenuStart (Ilbm, UNDOCOUNTER);
                          	}
                            break;
                      	}

                        if (MyMenu & DUMP_MSG)
                      	{
                            ModifyIDCMP (Ilbm->win, NULL);
                            ClearMenuStrip (Ilbm->win);
                            PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
#ifdef FM_REACT_SUPPORT
                            res = Do_PrintRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
                            res = Choice (Ilbm->win, CATSTR (TITLE_PrintReq), CATSTR (Prt_TXT_AreYouSure));
#endif /* FM_REACT_SUPPORT */
                            if (res)
                          	{
                                WinDump (Ilbm->win);
                          	}

                             ResetMenuStrip (Ilbm->win, MAINMENU);
                             ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                             break;
                       	}

                        if (MyMenu & PREVIEW_MSG)
                       	{
                            if (!(ZMASK & MASK)) break;
							
							if (NewCoords (Ilbm->win, ZOOMLINE[6], ZOOMLINE[3], ZOOMLINE[4], ZOOMLINE[5]))
                        	{
                               	MYBITMAP = CopyBitMap (Ilbm->win, (uint16) ZOOMLINE [6], (uint16) ZOOMLINE [3], (uint16) (ZOOMLINE [4] - ZOOMLINE [6] + 1), (uint16) (ZOOMLINE [5] - ZOOMLINE [3] + 1));
                                ModifyIDCMP (Ilbm->win, NULL);
                                ClearMenuStrip (Ilbm->win);
                                PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                                Preview (Ilbm->win, PIXELVECTOR, ARGBMEM, RNDMEM, PIXMEM, GFXMEM, Ilbm->win->GZZWidth, Ilbm->win->GZZHeight);
                                ResetMenuStrip (Ilbm->win, MAINMENU);
                                ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                                RestoreCoords (Ilbm->win);
                           }
                           break;
                       }

                       if (MyMenu & ORBIT_MSG)
                       {
                           ModifyIDCMP (Ilbm->win, NULL);
                           ClearMenuStrip (Ilbm->win);
                           Orbit (Ilbm->win, Ilbm->win->GZZWidth, Ilbm->win->GZZHeight);
                           ResetMenuStrip (Ilbm->win, MAINMENU);
                           ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                           break;
                       }

                       if (MyMenu & COORDS_MSG)
                       {
                           ModifyIDCMP (Ilbm->win, NULL);
                           ClearMenuStrip (Ilbm->win);
#ifdef FM_REACT_SUPPORT
                           res = Do_CoordinatesRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
                           res = ShowCoords (Ilbm->win);
#endif /* FM_REACT_SUPPORT */
                           if (res)
                           {
#ifdef FM_REACT_SUPPORT
                               res = Do_RenderRequest (Ilbm->win, FMSCREENNAME, 0, 0);
#else /* FM_REACT_SUPPORT */
                               res = Choice (Ilbm->win, CATSTR (TITLE_RenderReq), CATSTR (Rend_TXT_Question));
#endif /* FM_REACT_SUPPORT */
                               if (res) MyMenu |= REDRAW_MSG;
                           }

                           ResetMenuStrip (Ilbm->win, MAINMENU);
                           ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
                           if (MyMenu & REDRAW_MSG) goto REDRAW;
					   	   break;
					   }

                       if (MyMenu & UNDO_MSG)
                       {
                           RestoreCoords (Ilbm->win);
                           MyMenu |= REDRAW_MSG;
						   goto REDRAW;
						   break; // not necessary, here only for simmetry 
                       }
REDRAW:
                       if (MyMenu & REDRAW_MSG)
                       {
                           ClearZoomFrame (Ilbm->wrp);                           
 
                           SetMenuStop (Ilbm);
                           PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                           ELAPSEDTIME = DrawFractal (MandelInfo, Ilbm->win, ARGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, TRUE);
                           SetMenuStart (Ilbm, UNDOCOUNTER);
                           ShowTime (Ilbm->win, CATSTR (TXT_RecalculateTime), ELAPSEDTIME);
                           break;
                       }
DRAW:
                       if (MyMenu & DRAW_MSG)
                       {
                           ClearZoomFrame (Ilbm->wrp);
 
                           if (NewCoords (Ilbm->win, ZOOMLINE [6], ZOOMLINE [3], ZOOMLINE [4], ZOOMLINE [5]))
                           {
                               MYBITMAP = CopyBitMap (Ilbm->win, (uint16) ZOOMLINE [6], (uint16) ZOOMLINE [3], (uint16) (ZOOMLINE [4] - ZOOMLINE [6] + 1), (uint16) (ZOOMLINE [5] - ZOOMLINE [3] + 1));
                               PasteBitMap (MYBITMAP, Ilbm->win, (uint16) Ilbm->win->LeftEdge, (uint16) Ilbm->win->TopEdge, (uint16) (ZOOMLINE [4] - ZOOMLINE [6] + 1), (uint16) (ZOOMLINE [5] - ZOOMLINE [3] + 1));
                               SetMenuStop (Ilbm);
                               PutPointer (Ilbm->win, 0, 0, 0, 0, 0, BUSY_POINTER);
                               ELAPSEDTIME = DrawFractal (MandelInfo, Ilbm->win, ARGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, TRUE);
                               SetMenuStart (Ilbm, UNDOCOUNTER);
                               ShowTime (Ilbm->win, CATSTR (TXT_ZoomTime), ELAPSEDTIME);
                           }
                           break;
                       }
#ifdef  FM_AREXX_SUPPORT
                       if (MyMenu & AREXX_MSG)
                       {
                           ClearZoomFrame (Ilbm->wrp);
 
                           HandleARexxMenu (Ilbm, MyCode);
                           break;
                       }
#endif /* FM_AREXX_SUPPORT */
                       if (MyMenu & PREC_MSG)
                       {
                           	while (1L)
                           	{
#ifdef  FM_REACT_SUPPORT    
                            	res = Do_PrecisionRequest (Ilbm->win, FMSCREENNAME, 0, 0, MandelInfo->PrecisionBits);
#else /* ! FM_REACT_SUPPORT */
                                res = IntegerGad (Ilbm->win, CATSTR (TITLE_PrecisionReq), CATSTR (TXT_Math_ChangePrec), CATSTR (Prec_TXT_NumOfDigits), MandelInfo->PrecisionBits);
#endif /* FM_REACT_SUPPORT */
                                if ((res >= MIN_PRECISION_BITS) && (DELAY <= MAX_PRECISION_BITS)) break;
                                DisplayBeep (Ilbm->win->WScreen);
                           	}

                           	MandelInfo->PrecisionBits = res;
							// Reset_MandelInfo_GMP (MandelInfo->PrecisionBits); /* already set to max possible precision */
                            Reset_GMP (MandelInfo->PrecisionBits);
                            MandelInfo->PrecisionDigits = (int16) ceil (log10 (pow (2, MandelInfo->PrecisionBits)));
                            UpdatePrecMenuItem (res, MAINMENU, Ilbm);
                           	break;
                        }      	
					}					   					                                 
 					break; /* case IDCMP_MENUPICK */

                	case IDCMP_CLOSEWINDOW:
                    {
						MyMenu = EXIT_MSG;
                	}
					break;
            	} // switch myclass
         	} // while getmsg (!((MyMenu == EXIT_MSG) || (MyMenu & NEWDISPLAY_MSG)))
      	} // if ((receivedsig & wsignal)
    } // while !((MyMenu == EXIT_MSG) || (MyMenu & NEWDISPLAY_MSG))

  	mpf_clears (RealCoord, ImagCoord, ScrRatio, FracRatio, 0);
  	
	return (MyMenu);
}

/* FreeBitMapSafety() */
void FreeBitMapSafety (struct BitMap *Bitmap)
{
    WaitBlit ();
    FreeBitMap (Bitmap);
}

/* CopyBitMap() */
struct BitMap *CopyBitMap (struct Window *Win, uint16 Left, uint16 Top, uint16 Width, uint16 Height)
{
  struct BitMap *NewBM = NULL;
  int16 Depth;
  
    Depth = GetBitMapAttr (Win->RPort->BitMap, BMA_DEPTH);
	NewBM = AllocBitMapTags (Width, Height, Depth, 
								BMATags_Friend, Win->RPort->BitMap,	BMATags_Clear, TRUE,
								BMATags_Displayable, TRUE, BMATags_Alignment, 16,
								BMATags_ConstantBytesPerRow, TRUE, TAG_DONE);

    if (NewBM)
    {
        if (ZMASK & MASK) DrawBorder (Win->RPort, &MYBORDER, 0, 0);
        ShowTitle (Win->WScreen, FALSE);

		BltBitMapTags (BLITA_SrcX, Left, BLITA_SrcY, Top,
							BLITA_DestX, 0, BLITA_DestY, 0,
							BLITA_Width, Width, BLITA_Height, Height,
							BLITA_Source, Win->RPort->BitMap, BLITA_Dest, NewBM,
							BLITA_SrcType, BLITT_BITMAP, BLITA_DestType, BLITT_BITMAP,
							BLITA_Minterm, 0XC0, BLITA_Mask, 0XFF, TAG_DONE);
														
        if (TMASK & MASK) ShowTitle (Win->WScreen, TRUE);
        if (ZMASK & MASK) DrawBorder (Win->RPort, &MYBORDER, 0, 0);
        MASK |= BMASK;
    }

    return (NewBM);
}

/* PasteBitMap() */
int16 PasteBitMap (struct BitMap *SrcBM, struct Window *DstWin, uint16 SrcLeft, uint16 SrcTop, uint16 SrcWidth, uint16 SrcHeight)
{
  int16 Depth, Success = FALSE;
  uint16 DstWinWidth, DstWinHeight;
  struct BitMap *TmpBM = NULL;

	if (SrcBM && (MASK & BMASK))
    {
        DstWinWidth = ((DstWin->Flags & WFLG_GIMMEZEROZERO) ? DstWin->GZZWidth : DstWin->Width);
        DstWinHeight = ((DstWin->Flags & WFLG_GIMMEZEROZERO) ? DstWin->GZZHeight : DstWin->Height);
        Depth = GetBitMapAttr (DstWin->RPort->BitMap, BMA_DEPTH);

		TmpBM = AllocBitMapTags (DstWinWidth, DstWinHeight, Depth,
									BMATags_Friend, DstWin->RPort->BitMap, BMATags_Clear, TRUE,
									BMATags_Displayable, TRUE, BMATags_Alignment, 16,
									BMATags_ConstantBytesPerRow, TRUE, TAG_DONE);

        if (TmpBM)
        {
			BSA.bsa_SrcX = 0;
            BSA.bsa_SrcY = 0;
			BSA.bsa_SrcWidth = SrcWidth;
            BSA.bsa_SrcHeight = SrcHeight;
			// BSA.bsa_DestWidth = result
            // BSA.bsa_DestHeight = result
			BSA.bsa_DestX = 0;
            BSA.bsa_DestY = 0;
            BSA.bsa_XSrcFactor = SrcWidth;
            BSA.bsa_YSrcFactor = SrcHeight;
            BSA.bsa_XDestFactor = DstWinWidth;
            BSA.bsa_YDestFactor = DstWinHeight;
            BSA.bsa_SrcBitMap = SrcBM;
            BSA.bsa_DestBitMap = TmpBM;
			BSA.bsa_Flags = 0;
            BitMapScale (&BSA);
            BltBitMapRastPort (TmpBM, 0, 0, DstWin->RPort, 0, 0, DstWinWidth, DstWinHeight, 0xC0);
            FreeBitMapSafety (TmpBM);
            Success = TRUE;
        }

        FreeBitMapSafety (SrcBM);
        SrcBM = NULL;
        MASK &= ~BMASK;
    }

    return (Success);
}

/* WinDump() */
int32 WinDump (struct Window * Win)
{
  struct IODRPReq *IODrp = NULL;
  struct MsgPort *PrinterPort = NULL;
  struct ViewPort *Vp = NULL;
  int32 Error = PDERR_BADDIMENSION;

	if (! Win) goto ExitWinDump;

    if (PrinterPort = (struct MsgPort *) AllocSysObject (ASOT_PORT, NULL))
    {
        if (IODrp = (struct IODRPReq *) AllocSysObjectTags (ASOT_IOREQUEST, ASOIOR_Size, sizeof (struct IODRPReq), ASOIOR_ReplyPort, PrinterPort, TAG_DONE))
        {
            if (! (Error = OpenDevice (PRINTERDEVICE, 0, (struct IORequest *) IODrp, 0)))
            {
                Vp = ViewPortAddress (Win);

				IODrp->io_Command = PRD_DUMPRPORT;
				IODrp->io_RastPort = Win->RPort;
	            IODrp->io_ColorMap = Vp->ColorMap;
    	        IODrp->io_Modes = GetVPModeID (Vp);
        	    IODrp->io_SrcX = 0;
            	IODrp->io_SrcY = 0;
	            IODrp->io_SrcWidth = Win->GZZWidth;
    	        IODrp->io_SrcHeight = Win->GZZHeight;
        	    //   IODrp->io_DestCols  = 0;
            	//   IODrp->io_DestRows  = 0;
	            IODrp->io_Special =
    	        SPECIAL_ASPECT | SPECIAL_TRUSTME | SPECIAL_CENTER;
        	    Error = DoIO ((struct IORequest *) IODrp);
            	CloseDevice ((struct IORequest *) IODrp);
        	}

	        else Printf ("Printer-Device non opened, Error: %ld\n", Error);

        	FreeSysObject (ASOT_IOREQUEST, IODrp);
		}
	    
	    else  PutStr ("IODrp AllocSysOnject failed !");
    
    	FreeSysObject (ASOT_PORT, PrinterPort);
    }
    
    else PutStr ("Printer Port AllocSysOnject failed !");

ExitWinDump:
    return (Error);
}
