#ifndef FM_REACTIONCD_H
#define FM_REACTIONCD_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_CODE
#ifndef CATCOMP_BLOCK
#define CATCOMP_ARRAY
#endif
#endif

#ifdef CATCOMP_ARRAY
#ifndef CATCOMP_NUMBERS
#define CATCOMP_NUMBERS
#endif
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif

#ifdef CATCOMP_BLOCK
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define Pal_TXT_R 257
#define Pal_TXT_G 259
#define Pal_TXT_B 260
#define Pal_TXT_Accept 261
#define Pal_TXT_Reset 262
#define Pal_TXT_Cancel 263
#define Pal_TXT_Copy 264
#define Pal_TXT_Swap 265
#define Pal_TXT_Spread 266
#define Pal_TXT_Invert 267
#define Pal_TXT_ShiftLeft 268
#define Pal_TXT_ShiftRight 269
#define Pal_TXT_Undo 270
#define Info_TXT_Info 276
#define Info_TXT_OK 277
#define Info_TXT_More 278
#define Work_TXT_Workgroup 279
#define Work_TXT_OK 280
#define Cyc_TXT_DelayTime 281
#define Cyc_TXT_Accept 282
#define Cyc_TXT_Cancel 283
#define It_TXT_Iterations 284
#define SysI_TXT_CpuInfo 285
#define SysI_TXT_PPCVER 289
#define SysI_TXT_PPCCPU 292
#define SysI_TXT_CPU_PPC 293
#define SysI_TXT_PPCFPU 294
#define SysI_TXT_FPU_PPC 295
#define SysI_TXT_MemInfo 296
#define SysI_TXT_Available 297
#define SysI_TXT_AvailChip 298
#define SysI_TXT_AvailFast 299
#define SysI_TXT_LargestFree 300
#define SysI_TXT_LargestChip 301
#define SysI_TXT_LargestFast 302
#define Coord_TXT_Coord 303
#define Coord_TXT_TopValue 304
#define Coord_TXT_Top 305
#define Coord_TXT_LeftValue 306
#define Coord_TXT_Left 307
#define Coord_TXT_RightValue 308
#define Coord_TXT_Right 309
#define Coord_TXT_BottomValue 310
#define Coord_TXT_Bottom 311
#define Coord_TXT_Julia 312
#define Coord_TXT_JuliaRealValue 313
#define Coord_TXT_JuliaReal 314
#define Coord_TXT_JuliaImagValue 315
#define Coord_TXT_JuliaImag 316
#define Coord_TXT_KeepAxis 317
#define Coord_TXT_Accept 318
#define Coord_TXT_Ratio 319
#define Coord_TXT_Default 320
#define Coord_TXT_Cancel 321
#define Prt_TXT_AreYouSure 322
#define Prt_TXT_Yes 323
#define Prt_TXT_No 324
#define TITLE_WorkgroupReq 326
#define TITLE_CycleDelayReq 327
#define TITLE_IterationsReq 328
#define TITLE_SysInfoReq 329
#define TITLE_CoordReq 330
#define TITLE_PrintReq 331
#define TITLE_ExitReq 332
#define TXT_SavePicture 333
#define TXT_Project 334
#define TXT_About 335
#define TXT_SystemInfo 336
#define TXT_Help 337
#define TXT_LoadPicture 338
#define TXT_LoadPalette 339
#define TXT_SavePalette 340
#define TXT_Print 341
#define TXT_Quit 342
#define TXT_Options 346
#define TXT_Title 347
#define TXT_TitleBar 348
#define TXT_LastTime 349
#define TXT_Limits 350
#define TXT_Iterations 351
#define TXT_Custom 352
#define TXT_Priority 353
#define TXT_ColorRemap 354
#define TXT_Linear 355
#define TXT_Repeated 356
#define TXT_FractalType 357
#define TXT_Julia 358
#define TXT_Mandelbrot 359
#define TXT_Processor 360
#define TXT_Calculate 361
#define TXT_Preview 362
#define TXT_Recalculate 363
#define NewScr_TXT_Question 364
#define TITLE_RendReq 365
#define TXT_Undo 366
#define TXT_Zoom 367
#define TXT_Stop 368
#define TXT_Video 369
#define TXT_Cycle 370
#define TXT_Forward 371
#define TXT_Backward 372
#define TXT_Delay 373
#define TXT_Palette 374
#define TXT_ScreenMode 375
#define TXT_FontSettings 376
#define TXT_ERR_DiskFontLibrary 377
#define TXT_ERR_IconLibrary 378
#define TXT_ERR_GadToolsLibrary 379
#define TXT_ERR_AslLibrary 380
#define TXT_ERR_UtilityLibrary 381
#define TXT_ERR_IFFParseLibrary 382
#define TXT_ERR_OpenDisplay 383
#define TXT_ERR_NoMemForTempRastPort 384
#define TXT_ERR_Font 385
#define TXT_ERR_NoMonitor 386
#define TXT_ERR_NoChips 387
#define TXT_ERR_NoMem 388
#define TXT_ERR_NoChipMem 389
#define TXT_ERR_PubNotUnique 390
#define TXT_ERR_UnknownMode 391
#define TXT_ERR_ScreenToDeep 392
#define TXT_ERR_AttachScreen 393
#define TXT_ERR_ModeNotAvailable 394
#define TXT_ERR_UnknownErr 395
#define TXT_ERR_VisualInfo 396
#define TXT_ERR_Window 397
#define TXT_ERR_Menu 398
#define TXT_ERR_Gadget 399
#define TXT_ERR_WindowGadget 400
#define TXT_ERR_PreviewWindow 401
#define TXT_ERR_CreateDisplay 402
#define TXT_ERR_DimensionInfo 403
#define TXT_ERR_PaletteRequester 404
#define TXT_ERR_MakeDisplay 405
#define TXT_ERR_QueryMandPic 406
#define TXT_ERR_LoadMandPic 407
#define TXT_ERR_SaveMandPic 408
#define TXT_ERR_LoadMandPal 409
#define TXT_ERR_SaveMandPal 410
#define TXT_ERR_NoMemForPPC 411
#define TXT_CMD_Guide 412
#define TXT_RenderTime 413
#define TXT_AverageSpeed 414
#define TXT_PreviewTime 415
#define TXT_LeftButtonForJulia 416
#define TXT_JuliaConstant 417
#define TXT_LoadPictureTitle 418
#define TXT_SavePictureTitle 419
#define TXT_LoadPaletteTitle 420
#define TXT_SavePaletteTitle 421
#define TXT_ZoomTime 422
#define TITLE_RenderReq 423
#define TITLE_OverwriteReq 424
#define Rend_TXT_Question 425
#define OverWr_TXT_Question 426
#define TXT_LastCalcTime 427
#define TXT_OK 428
#define TXT_FMErrorTitle 429
#define TXT_ERR_NoSignal 430
#define TXT_ERR_CantMakeScreenPrivate 431
#define TXT_ERR_LayoutMenu 432
#define TXT_ScrTitle_Cyc 433
#define TXT_ScrTitle_It 434
#define TXT_ERR_Displayinfo 435
#define TXT_LessThanOne 436
#define TXT_SysInfo 437
#define TXT_PreviewTitle 438
#define TXT_RecalculateTime 439
#define TXT_YesNo 440
#define TXT_OKMore 441
#define TXT_Info 442
#define TXT_None 443
#define TXT_ScrTitle_Coord 444
#define Pal_ActKey_Red 445
#define Pal_ActKey_Green 446
#define Pal_ActKey_Blue 447
#define TITLE_PaletteReq 448
#define TITLE_InfoReq 449
#define TXT_ZPower 450
#define TXT_Unknown 451
#define TXT_Launch 452
#define TXT_SelectRxScript 453
#define TXT_Orbit 454
#define TXT_ERR_OrbitWindow 455
#define TXT_OrbitTitle 456
#define TXT_RenderingMode 457
#define TITLE_PrecisionReq 458
#define TXT_MathMode 459
#define TXT_MathMode_Fast 460
#define TXT_MathMode_HighPrec 461
#define TXT_Math_ChangePrec 462
#define It_TXT_Accept 463
#define It_TXT_Cancel 464
#define Prec_TXT_NumOfDigits 465
#define Prec_TXT_Accept 466
#define Prec_TXT_Cancel 467

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define Pal_TXT_R_STR "R"
#define Pal_TXT_G_STR "G"
#define Pal_TXT_B_STR "B"
#define Pal_TXT_Accept_STR "_Accept"
#define Pal_TXT_Reset_STR "Reset"
#define Pal_TXT_Cancel_STR "_Cancel"
#define Pal_TXT_Copy_STR "C_opy"
#define Pal_TXT_Swap_STR "_Swap"
#define Pal_TXT_Spread_STR "Sp_read"
#define Pal_TXT_Invert_STR "_Invert"
#define Pal_TXT_ShiftLeft_STR "_<<"
#define Pal_TXT_ShiftRight_STR "_>>"
#define Pal_TXT_Undo_STR "_Undo"
#define Info_TXT_Info_STR "\nFlashMandelNG 4.9  (c)1995-2023\n\n  Snail address:\n  Dino Papararo\n  Via Manzoni, 184\n  80123 Napoli\n  Italia\n\n\n  E-Mail address:\n  Dino.Papararo@GMail.Com\n\n\n****************************\n*This program is Giftware !!*\n****************************\n"
#define Info_TXT_OK_STR "_OK"
#define Info_TXT_More_STR "_More..."
#define Work_TXT_Workgroup_STR "\nGreetings and thanks to:\n\n  Edgar Schwan (Co-Author)\n\n\nBottomless wonders spring from simple rules, \nwhich are repeated without end.\nBenoit Mandelbrot.\n\nAmiga forever!\n\n\n\n\n"
#define Work_TXT_OK_STR "_OK"
#define Cyc_TXT_DelayTime_STR "D_elay time"
#define Cyc_TXT_Accept_STR "_Accept"
#define Cyc_TXT_Cancel_STR "_Cancel"
#define It_TXT_Iterations_STR "_Iterations"
#define SysI_TXT_CpuInfo_STR "CPU Informations"
#define SysI_TXT_PPCVER_STR "None"
#define SysI_TXT_PPCCPU_STR "None"
#define SysI_TXT_CPU_PPC_STR "CPU:"
#define SysI_TXT_PPCFPU_STR "None"
#define SysI_TXT_FPU_PPC_STR "FPU:"
#define SysI_TXT_MemInfo_STR "Memory Informations"
#define SysI_TXT_Available_STR "Available"
#define SysI_TXT_AvailChip_STR "Chip:"
#define SysI_TXT_AvailFast_STR "Fast:"
#define SysI_TXT_LargestFree_STR "Largest free"
#define SysI_TXT_LargestChip_STR "Chip:"
#define SysI_TXT_LargestFast_STR "Fast:"
#define Coord_TXT_Coord_STR "Coordinates"
#define Coord_TXT_TopValue_STR "+1.1250000000000000"
#define Coord_TXT_Top_STR "T_op"
#define Coord_TXT_LeftValue_STR "2.5"
#define Coord_TXT_Left_STR "_Left"
#define Coord_TXT_RightValue_STR "+1.5"
#define Coord_TXT_Right_STR "_Right"
#define Coord_TXT_BottomValue_STR "-1.1250000000000000"
#define Coord_TXT_Bottom_STR "Bo_ttom"
#define Coord_TXT_Julia_STR "Julia"
#define Coord_TXT_JuliaRealValue_STR "-0.7200000000000000"
#define Coord_TXT_JuliaReal_STR "_Julia real"
#define Coord_TXT_JuliaImagValue_STR "-0.2600000000000000"
#define Coord_TXT_JuliaImag_STR "J_ulia imag"
#define Coord_TXT_KeepAxis_STR "Keep _X axis for ratio"
#define Coord_TXT_Accept_STR "_Accept"
#define Coord_TXT_Ratio_STR "Rat_io"
#define Coord_TXT_Default_STR "_Default"
#define Coord_TXT_Cancel_STR "_Cancel"
#define Prt_TXT_AreYouSure_STR "Are you sure?"
#define Prt_TXT_Yes_STR "_Yes"
#define Prt_TXT_No_STR "_No"
#define TITLE_WorkgroupReq_STR "Workgroup Requester"
#define TITLE_CycleDelayReq_STR "Cycle-Delay Requester"
#define TITLE_IterationsReq_STR "Iterations Requester"
#define TITLE_SysInfoReq_STR "System-Info Requester"
#define TITLE_CoordReq_STR "Coordinates Requester"
#define TITLE_PrintReq_STR "Print Requester"
#define TITLE_ExitReq_STR "Exit Requester"
#define TXT_SavePicture_STR "Save Picture..."
#define TXT_Project_STR "Project"
#define TXT_About_STR "About..."
#define TXT_SystemInfo_STR "System info..."
#define TXT_Help_STR "Help..."
#define TXT_LoadPicture_STR "Load Picture..."
#define TXT_LoadPalette_STR "Load Palette..."
#define TXT_SavePalette_STR "Save Palette..."
#define TXT_Print_STR "Print..."
#define TXT_Quit_STR "Quit..."
#define TXT_Options_STR "Options"
#define TXT_Title_STR "Title"
#define TXT_TitleBar_STR "Title bar"
#define TXT_LastTime_STR "Last time"
#define TXT_Limits_STR "Limits..."
#define TXT_Iterations_STR "Iterations"
#define TXT_Custom_STR "Custom..."
#define TXT_Priority_STR "Priority"
#define TXT_ColorRemap_STR "Color remap"
#define TXT_Linear_STR "Linear"
#define TXT_Repeated_STR "Repeated"
#define TXT_FractalType_STR "Fractal type"
#define TXT_Julia_STR "Julia"
#define TXT_Mandelbrot_STR "Mandelbrot"
#define TXT_Processor_STR "Processor"
#define TXT_Calculate_STR "Calculate"
#define TXT_Preview_STR "Preview"
#define TXT_Recalculate_STR "Recalculate"
#define NewScr_TXT_Question_STR "Screen propreties are changed.\nImage and colors can be inaccurate.\n\nRecalculate ?\n"
#define TITLE_RendReq_STR "Rendering Requester"
#define TXT_Undo_STR "Undo"
#define TXT_Zoom_STR "Zoom"
#define TXT_Stop_STR "Stop"
#define TXT_Video_STR "Video"
#define TXT_Cycle_STR "Cycle"
#define TXT_Forward_STR "Forward"
#define TXT_Backward_STR "Backward"
#define TXT_Delay_STR "Delay..."
#define TXT_Palette_STR "Palette..."
#define TXT_ScreenMode_STR "Screen mode..."
#define TXT_FontSettings_STR "Font settings..."
#define TXT_ERR_DiskFontLibrary_STR "I need at least DiskFontLibrary v39\nor better, sorry !\n"
#define TXT_ERR_IconLibrary_STR "I need at least IconLibrary v39\nor better, sorry !\n"
#define TXT_ERR_GadToolsLibrary_STR "I need at least GadToolsibrary v39\nor better, sorry !\n"
#define TXT_ERR_AslLibrary_STR "I need at least AslLibrary v39\nor better, sorry !\n"
#define TXT_ERR_UtilityLibrary_STR "I need at least UtilityLibrary v39\nor better, sorry !\n"
#define TXT_ERR_IFFParseLibrary_STR "I need at least IFFParseLibrary v39\nor better, sorry !\n"
#define TXT_ERR_OpenDisplay_STR "I need at least 8 colors, sorry.\n"
#define TXT_ERR_NoMemForTempRastPort_STR "No memory for temporary RastPort!\n"
#define TXT_ERR_Font_STR "Font failed!\n"
#define TXT_ERR_NoMonitor_STR "No monitor!\n"
#define TXT_ERR_NoChips_STR "No chips!\n"
#define TXT_ERR_NoMem_STR "No memory!\n"
#define TXT_ERR_NoChipMem_STR "No chipmemory!\n"
#define TXT_ERR_PubNotUnique_STR "Pub not unique!\n"
#define TXT_ERR_UnknownMode_STR "Unknown mode!\n"
#define TXT_ERR_ScreenToDeep_STR "Screen too deep!\n"
#define TXT_ERR_AttachScreen_STR "Attach screen failed!\n"
#define TXT_ERR_ModeNotAvailable_STR "Mode not available!\n"
#define TXT_ERR_UnknownErr_STR "Unknown screen error!\n"
#define TXT_ERR_VisualInfo_STR "Visualinfo failed!\n"
#define TXT_ERR_Window_STR "Openwindow failed!\n"
#define TXT_ERR_Menu_STR "Menu failed!\n"
#define TXT_ERR_Gadget_STR "Gadget error!\n"
#define TXT_ERR_WindowGadget_STR "Window gadget error!\n"
#define TXT_ERR_PreviewWindow_STR "Preview window failed!\n"
#define TXT_ERR_CreateDisplay_STR "Createdisplay error!\n"
#define TXT_ERR_DimensionInfo_STR "Dimensioninfo error!\n"
#define TXT_ERR_PaletteRequester_STR "Palette requester error!\n"
#define TXT_ERR_MakeDisplay_STR "Display error!\n"
#define TXT_ERR_QueryMandPic_STR "Not a FlashMandel picture !\n"
#define TXT_ERR_LoadMandPic_STR "Load picture error!\n"
#define TXT_ERR_SaveMandPic_STR "Save picture error!\n"
#define TXT_ERR_LoadMandPal_STR "Load palette error!\n"
#define TXT_ERR_SaveMandPal_STR "Save palette error!\n"
#define TXT_ERR_NoMemForPPC_STR "No mem for memory buffer, only Mc 68k rendering allowed!\n\n"
#define TXT_CMD_Guide_STR "SYS:Utilities/MultiView Docs/FlashMandelNG.guide\0"
#define TXT_RenderTime_STR "Rendering elapsed time:"
#define TXT_AverageSpeed_STR "%s %uh %um %us - Average speed: %.1f Pixels per second"
#define TXT_PreviewTime_STR "Preview elapsed time:"
#define TXT_LeftButtonForJulia_STR "Press left button to choose the Julia constant or right button to cancel"
#define TXT_JuliaConstant_STR "Julia constant: Real %+2.30Ff Imag %+2.30Ffi"
#define TXT_LoadPictureTitle_STR "Load iff picture"
#define TXT_SavePictureTitle_STR "Save iff picture"
#define TXT_LoadPaletteTitle_STR "Load palette"
#define TXT_SavePaletteTitle_STR "Save palette"
#define TXT_ZoomTime_STR "Zoom elapsed time:"
#define TITLE_RenderReq_STR "Rendering Requester"
#define TITLE_OverwriteReq_STR "Overwrite file requester"
#define Rend_TXT_Question_STR "Recalculate ?\n"
#define OverWr_TXT_Question_STR "File already exists...\n\nDo you want overwrite it ?"
#define TXT_LastCalcTime_STR "Last calculating time:"
#define TXT_OK_STR "OK"
#define TXT_FMErrorTitle_STR "FlashMandel Error"
#define TXT_ERR_NoSignal_STR "No signal available\n"
#define TXT_ERR_CantMakeScreenPrivate_STR "Can't make screen private.\nPlease close all windows\n"
#define TXT_ERR_LayoutMenu_STR "Can't layout menus.\n"
#define TXT_ScrTitle_Cyc_STR "Insert cycle delay time... between 0 (fastest) and 200 (slowest)"
#define TXT_ScrTitle_It_STR "Insert new iterations... between 2^6 (64) and 2^30 (1073741824)"
#define TXT_ERR_Displayinfo_STR "Displayinfo error!\n"
#define TXT_LessThanOne_STR "%s Less than one second - Average speed: Realtime"
#define TXT_SysInfo_STR "Cpu informations\n\nPowerPc\nCpu:%s  Version:%s\n\nVector unit:%s\n\n\nMemory informations\n\nAvailable total\nChip:%ld  Fast:%ld\n\nLargest free\nChip:%ld  Fast:%ld\n"
#define TXT_PreviewTitle_STR "Preview Window"
#define TXT_RecalculateTime_STR "Recalculate elapsed time:"
#define TXT_YesNo_STR "Yes|No"
#define TXT_OKMore_STR "OK|More..."
#define TXT_Info_STR "%s  %s \n\n\n  Snail address: \n  %s\n  %s\n\n\n  %s\n\n*******************************\n* This program is GiftWare !! *\n*******************************\n\n"
#define TXT_None_STR "None"
#define TXT_ScrTitle_Coord_STR "Insert new range..."
#define Pal_ActKey_Red_STR "R"
#define Pal_ActKey_Green_STR "G"
#define Pal_ActKey_Blue_STR "B"
#define TITLE_PaletteReq_STR "Palette Requester"
#define TITLE_InfoReq_STR "Info Reqester"
#define TXT_ZPower_STR "Power"
#define TXT_Unknown_STR "Unknown"
#define TXT_Launch_STR "Launch..."
#define TXT_SelectRxScript_STR "Please select an arexx-script!"
#define TXT_Orbit_STR "Orbit"
#define TXT_ERR_OrbitWindow_STR "Orbit window failed!"
#define TXT_OrbitTitle_STR "Orbit Window"
#define TXT_RenderingMode_STR "Rendering Mode"
#define TITLE_PrecisionReq_STR "Change Precision"
#define TXT_MathMode_STR "Math mode"
#define TXT_MathMode_Fast_STR "Fast"
#define TXT_MathMode_HighPrec_STR "High Precision (%ld bits)"
#define TXT_Math_ChangePrec_STR "Change Precision..."
#define It_TXT_Accept_STR "_Accept"
#define It_TXT_Cancel_STR "_Cancel"
#define Prec_TXT_NumOfDigits_STR "_Precision Bits"
#define Prec_TXT_Accept_STR "_Accept"
#define Prec_TXT_Cancel_STR "_Cancel"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG         cca_ID;
    CONST_STRPTR cca_Str;
};

STATIC CONST struct CatCompArrayType CatCompArray[] =
{
    {Pal_TXT_R,(CONST_STRPTR)Pal_TXT_R_STR},
    {Pal_TXT_G,(CONST_STRPTR)Pal_TXT_G_STR},
    {Pal_TXT_B,(CONST_STRPTR)Pal_TXT_B_STR},
    {Pal_TXT_Accept,(CONST_STRPTR)Pal_TXT_Accept_STR},
    {Pal_TXT_Reset,(CONST_STRPTR)Pal_TXT_Reset_STR},
    {Pal_TXT_Cancel,(CONST_STRPTR)Pal_TXT_Cancel_STR},
    {Pal_TXT_Copy,(CONST_STRPTR)Pal_TXT_Copy_STR},
    {Pal_TXT_Swap,(CONST_STRPTR)Pal_TXT_Swap_STR},
    {Pal_TXT_Spread,(CONST_STRPTR)Pal_TXT_Spread_STR},
    {Pal_TXT_Invert,(CONST_STRPTR)Pal_TXT_Invert_STR},
    {Pal_TXT_ShiftLeft,(CONST_STRPTR)Pal_TXT_ShiftLeft_STR},
    {Pal_TXT_ShiftRight,(CONST_STRPTR)Pal_TXT_ShiftRight_STR},
    {Pal_TXT_Undo,(CONST_STRPTR)Pal_TXT_Undo_STR},
    {Info_TXT_Info,(CONST_STRPTR)Info_TXT_Info_STR},
    {Info_TXT_OK,(CONST_STRPTR)Info_TXT_OK_STR},
    {Info_TXT_More,(CONST_STRPTR)Info_TXT_More_STR},
    {Work_TXT_Workgroup,(CONST_STRPTR)Work_TXT_Workgroup_STR},
    {Work_TXT_OK,(CONST_STRPTR)Work_TXT_OK_STR},
    {Cyc_TXT_DelayTime,(CONST_STRPTR)Cyc_TXT_DelayTime_STR},
    {Cyc_TXT_Accept,(CONST_STRPTR)Cyc_TXT_Accept_STR},
    {Cyc_TXT_Cancel,(CONST_STRPTR)Cyc_TXT_Cancel_STR},
    {It_TXT_Iterations,(CONST_STRPTR)It_TXT_Iterations_STR},
    {SysI_TXT_CpuInfo,(CONST_STRPTR)SysI_TXT_CpuInfo_STR},
    {SysI_TXT_PPCVER,(CONST_STRPTR)SysI_TXT_PPCVER_STR},
    {SysI_TXT_PPCCPU,(CONST_STRPTR)SysI_TXT_PPCCPU_STR},
    {SysI_TXT_CPU_PPC,(CONST_STRPTR)SysI_TXT_CPU_PPC_STR},
    {SysI_TXT_PPCFPU,(CONST_STRPTR)SysI_TXT_PPCFPU_STR},
    {SysI_TXT_FPU_PPC,(CONST_STRPTR)SysI_TXT_FPU_PPC_STR},
    {SysI_TXT_MemInfo,(CONST_STRPTR)SysI_TXT_MemInfo_STR},
    {SysI_TXT_Available,(CONST_STRPTR)SysI_TXT_Available_STR},
    {SysI_TXT_AvailChip,(CONST_STRPTR)SysI_TXT_AvailChip_STR},
    {SysI_TXT_AvailFast,(CONST_STRPTR)SysI_TXT_AvailFast_STR},
    {SysI_TXT_LargestFree,(CONST_STRPTR)SysI_TXT_LargestFree_STR},
    {SysI_TXT_LargestChip,(CONST_STRPTR)SysI_TXT_LargestChip_STR},
    {SysI_TXT_LargestFast,(CONST_STRPTR)SysI_TXT_LargestFast_STR},
    {Coord_TXT_Coord,(CONST_STRPTR)Coord_TXT_Coord_STR},
    {Coord_TXT_TopValue,(CONST_STRPTR)Coord_TXT_TopValue_STR},
    {Coord_TXT_Top,(CONST_STRPTR)Coord_TXT_Top_STR},
    {Coord_TXT_LeftValue,(CONST_STRPTR)Coord_TXT_LeftValue_STR},
    {Coord_TXT_Left,(CONST_STRPTR)Coord_TXT_Left_STR},
    {Coord_TXT_RightValue,(CONST_STRPTR)Coord_TXT_RightValue_STR},
    {Coord_TXT_Right,(CONST_STRPTR)Coord_TXT_Right_STR},
    {Coord_TXT_BottomValue,(CONST_STRPTR)Coord_TXT_BottomValue_STR},
    {Coord_TXT_Bottom,(CONST_STRPTR)Coord_TXT_Bottom_STR},
    {Coord_TXT_Julia,(CONST_STRPTR)Coord_TXT_Julia_STR},
    {Coord_TXT_JuliaRealValue,(CONST_STRPTR)Coord_TXT_JuliaRealValue_STR},
    {Coord_TXT_JuliaReal,(CONST_STRPTR)Coord_TXT_JuliaReal_STR},
    {Coord_TXT_JuliaImagValue,(CONST_STRPTR)Coord_TXT_JuliaImagValue_STR},
    {Coord_TXT_JuliaImag,(CONST_STRPTR)Coord_TXT_JuliaImag_STR},
    {Coord_TXT_KeepAxis,(CONST_STRPTR)Coord_TXT_KeepAxis_STR},
    {Coord_TXT_Accept,(CONST_STRPTR)Coord_TXT_Accept_STR},
    {Coord_TXT_Ratio,(CONST_STRPTR)Coord_TXT_Ratio_STR},
    {Coord_TXT_Default,(CONST_STRPTR)Coord_TXT_Default_STR},
    {Coord_TXT_Cancel,(CONST_STRPTR)Coord_TXT_Cancel_STR},
    {Prt_TXT_AreYouSure,(CONST_STRPTR)Prt_TXT_AreYouSure_STR},
    {Prt_TXT_Yes,(CONST_STRPTR)Prt_TXT_Yes_STR},
    {Prt_TXT_No,(CONST_STRPTR)Prt_TXT_No_STR},
    {TITLE_WorkgroupReq,(CONST_STRPTR)TITLE_WorkgroupReq_STR},
    {TITLE_CycleDelayReq,(CONST_STRPTR)TITLE_CycleDelayReq_STR},
    {TITLE_IterationsReq,(CONST_STRPTR)TITLE_IterationsReq_STR},
    {TITLE_SysInfoReq,(CONST_STRPTR)TITLE_SysInfoReq_STR},
    {TITLE_CoordReq,(CONST_STRPTR)TITLE_CoordReq_STR},
    {TITLE_PrintReq,(CONST_STRPTR)TITLE_PrintReq_STR},
    {TITLE_ExitReq,(CONST_STRPTR)TITLE_ExitReq_STR},
    {TXT_SavePicture,(CONST_STRPTR)TXT_SavePicture_STR},
    {TXT_Project,(CONST_STRPTR)TXT_Project_STR},
    {TXT_About,(CONST_STRPTR)TXT_About_STR},
    {TXT_SystemInfo,(CONST_STRPTR)TXT_SystemInfo_STR},
    {TXT_Help,(CONST_STRPTR)TXT_Help_STR},
    {TXT_LoadPicture,(CONST_STRPTR)TXT_LoadPicture_STR},
    {TXT_LoadPalette,(CONST_STRPTR)TXT_LoadPalette_STR},
    {TXT_SavePalette,(CONST_STRPTR)TXT_SavePalette_STR},
    {TXT_Print,(CONST_STRPTR)TXT_Print_STR},
    {TXT_Quit,(CONST_STRPTR)TXT_Quit_STR},
    {TXT_Options,(CONST_STRPTR)TXT_Options_STR},
    {TXT_Title,(CONST_STRPTR)TXT_Title_STR},
    {TXT_TitleBar,(CONST_STRPTR)TXT_TitleBar_STR},
    {TXT_LastTime,(CONST_STRPTR)TXT_LastTime_STR},
    {TXT_Limits,(CONST_STRPTR)TXT_Limits_STR},
    {TXT_Iterations,(CONST_STRPTR)TXT_Iterations_STR},
    {TXT_Custom,(CONST_STRPTR)TXT_Custom_STR},
    {TXT_Priority,(CONST_STRPTR)TXT_Priority_STR},
    {TXT_ColorRemap,(CONST_STRPTR)TXT_ColorRemap_STR},
    {TXT_Linear,(CONST_STRPTR)TXT_Linear_STR},
    {TXT_Repeated,(CONST_STRPTR)TXT_Repeated_STR},
    {TXT_FractalType,(CONST_STRPTR)TXT_FractalType_STR},
    {TXT_Julia,(CONST_STRPTR)TXT_Julia_STR},
    {TXT_Mandelbrot,(CONST_STRPTR)TXT_Mandelbrot_STR},
    {TXT_Processor,(CONST_STRPTR)TXT_Processor_STR},
    {TXT_Calculate,(CONST_STRPTR)TXT_Calculate_STR},
    {TXT_Preview,(CONST_STRPTR)TXT_Preview_STR},
    {TXT_Recalculate,(CONST_STRPTR)TXT_Recalculate_STR},
    {NewScr_TXT_Question,(CONST_STRPTR)NewScr_TXT_Question_STR},
    {TITLE_RendReq,(CONST_STRPTR)TITLE_RendReq_STR},
    {TXT_Undo,(CONST_STRPTR)TXT_Undo_STR},
    {TXT_Zoom,(CONST_STRPTR)TXT_Zoom_STR},
    {TXT_Stop,(CONST_STRPTR)TXT_Stop_STR},
    {TXT_Video,(CONST_STRPTR)TXT_Video_STR},
    {TXT_Cycle,(CONST_STRPTR)TXT_Cycle_STR},
    {TXT_Forward,(CONST_STRPTR)TXT_Forward_STR},
    {TXT_Backward,(CONST_STRPTR)TXT_Backward_STR},
    {TXT_Delay,(CONST_STRPTR)TXT_Delay_STR},
    {TXT_Palette,(CONST_STRPTR)TXT_Palette_STR},
    {TXT_ScreenMode,(CONST_STRPTR)TXT_ScreenMode_STR},
    {TXT_FontSettings,(CONST_STRPTR)TXT_FontSettings_STR},
    {TXT_ERR_DiskFontLibrary,(CONST_STRPTR)TXT_ERR_DiskFontLibrary_STR},
    {TXT_ERR_IconLibrary,(CONST_STRPTR)TXT_ERR_IconLibrary_STR},
    {TXT_ERR_GadToolsLibrary,(CONST_STRPTR)TXT_ERR_GadToolsLibrary_STR},
    {TXT_ERR_AslLibrary,(CONST_STRPTR)TXT_ERR_AslLibrary_STR},
    {TXT_ERR_UtilityLibrary,(CONST_STRPTR)TXT_ERR_UtilityLibrary_STR},
    {TXT_ERR_IFFParseLibrary,(CONST_STRPTR)TXT_ERR_IFFParseLibrary_STR},
    {TXT_ERR_OpenDisplay,(CONST_STRPTR)TXT_ERR_OpenDisplay_STR},
    {TXT_ERR_NoMemForTempRastPort,(CONST_STRPTR)TXT_ERR_NoMemForTempRastPort_STR},
    {TXT_ERR_Font,(CONST_STRPTR)TXT_ERR_Font_STR},
    {TXT_ERR_NoMonitor,(CONST_STRPTR)TXT_ERR_NoMonitor_STR},
    {TXT_ERR_NoChips,(CONST_STRPTR)TXT_ERR_NoChips_STR},
    {TXT_ERR_NoMem,(CONST_STRPTR)TXT_ERR_NoMem_STR},
    {TXT_ERR_NoChipMem,(CONST_STRPTR)TXT_ERR_NoChipMem_STR},
    {TXT_ERR_PubNotUnique,(CONST_STRPTR)TXT_ERR_PubNotUnique_STR},
    {TXT_ERR_UnknownMode,(CONST_STRPTR)TXT_ERR_UnknownMode_STR},
    {TXT_ERR_ScreenToDeep,(CONST_STRPTR)TXT_ERR_ScreenToDeep_STR},
    {TXT_ERR_AttachScreen,(CONST_STRPTR)TXT_ERR_AttachScreen_STR},
    {TXT_ERR_ModeNotAvailable,(CONST_STRPTR)TXT_ERR_ModeNotAvailable_STR},
    {TXT_ERR_UnknownErr,(CONST_STRPTR)TXT_ERR_UnknownErr_STR},
    {TXT_ERR_VisualInfo,(CONST_STRPTR)TXT_ERR_VisualInfo_STR},
    {TXT_ERR_Window,(CONST_STRPTR)TXT_ERR_Window_STR},
    {TXT_ERR_Menu,(CONST_STRPTR)TXT_ERR_Menu_STR},
    {TXT_ERR_Gadget,(CONST_STRPTR)TXT_ERR_Gadget_STR},
    {TXT_ERR_WindowGadget,(CONST_STRPTR)TXT_ERR_WindowGadget_STR},
    {TXT_ERR_PreviewWindow,(CONST_STRPTR)TXT_ERR_PreviewWindow_STR},
    {TXT_ERR_CreateDisplay,(CONST_STRPTR)TXT_ERR_CreateDisplay_STR},
    {TXT_ERR_DimensionInfo,(CONST_STRPTR)TXT_ERR_DimensionInfo_STR},
    {TXT_ERR_PaletteRequester,(CONST_STRPTR)TXT_ERR_PaletteRequester_STR},
    {TXT_ERR_MakeDisplay,(CONST_STRPTR)TXT_ERR_MakeDisplay_STR},
    {TXT_ERR_QueryMandPic,(CONST_STRPTR)TXT_ERR_QueryMandPic_STR},
    {TXT_ERR_LoadMandPic,(CONST_STRPTR)TXT_ERR_LoadMandPic_STR},
    {TXT_ERR_SaveMandPic,(CONST_STRPTR)TXT_ERR_SaveMandPic_STR},
    {TXT_ERR_LoadMandPal,(CONST_STRPTR)TXT_ERR_LoadMandPal_STR},
    {TXT_ERR_SaveMandPal,(CONST_STRPTR)TXT_ERR_SaveMandPal_STR},
    {TXT_ERR_NoMemForPPC,(CONST_STRPTR)TXT_ERR_NoMemForPPC_STR},
    {TXT_CMD_Guide,(CONST_STRPTR)TXT_CMD_Guide_STR},
    {TXT_RenderTime,(CONST_STRPTR)TXT_RenderTime_STR},
    {TXT_AverageSpeed,(CONST_STRPTR)TXT_AverageSpeed_STR},
    {TXT_PreviewTime,(CONST_STRPTR)TXT_PreviewTime_STR},
    {TXT_LeftButtonForJulia,(CONST_STRPTR)TXT_LeftButtonForJulia_STR},
    {TXT_JuliaConstant,(CONST_STRPTR)TXT_JuliaConstant_STR},
    {TXT_LoadPictureTitle,(CONST_STRPTR)TXT_LoadPictureTitle_STR},
    {TXT_SavePictureTitle,(CONST_STRPTR)TXT_SavePictureTitle_STR},
    {TXT_LoadPaletteTitle,(CONST_STRPTR)TXT_LoadPaletteTitle_STR},
    {TXT_SavePaletteTitle,(CONST_STRPTR)TXT_SavePaletteTitle_STR},
    {TXT_ZoomTime,(CONST_STRPTR)TXT_ZoomTime_STR},
    {TITLE_RenderReq,(CONST_STRPTR)TITLE_RenderReq_STR},
    {TITLE_OverwriteReq,(CONST_STRPTR)TITLE_OverwriteReq_STR},
    {Rend_TXT_Question,(CONST_STRPTR)Rend_TXT_Question_STR},
    {OverWr_TXT_Question,(CONST_STRPTR)OverWr_TXT_Question_STR},
    {TXT_LastCalcTime,(CONST_STRPTR)TXT_LastCalcTime_STR},
    {TXT_OK,(CONST_STRPTR)TXT_OK_STR},
    {TXT_FMErrorTitle,(CONST_STRPTR)TXT_FMErrorTitle_STR},
    {TXT_ERR_NoSignal,(CONST_STRPTR)TXT_ERR_NoSignal_STR},
    {TXT_ERR_CantMakeScreenPrivate,(CONST_STRPTR)TXT_ERR_CantMakeScreenPrivate_STR},
    {TXT_ERR_LayoutMenu,(CONST_STRPTR)TXT_ERR_LayoutMenu_STR},
    {TXT_ScrTitle_Cyc,(CONST_STRPTR)TXT_ScrTitle_Cyc_STR},
    {TXT_ScrTitle_It,(CONST_STRPTR)TXT_ScrTitle_It_STR},
    {TXT_ERR_Displayinfo,(CONST_STRPTR)TXT_ERR_Displayinfo_STR},
    {TXT_LessThanOne,(CONST_STRPTR)TXT_LessThanOne_STR},
    {TXT_SysInfo,(CONST_STRPTR)TXT_SysInfo_STR},
    {TXT_PreviewTitle,(CONST_STRPTR)TXT_PreviewTitle_STR},
    {TXT_RecalculateTime,(CONST_STRPTR)TXT_RecalculateTime_STR},
    {TXT_YesNo,(CONST_STRPTR)TXT_YesNo_STR},
    {TXT_OKMore,(CONST_STRPTR)TXT_OKMore_STR},
    {TXT_Info,(CONST_STRPTR)TXT_Info_STR},
    {TXT_None,(CONST_STRPTR)TXT_None_STR},
    {TXT_ScrTitle_Coord,(CONST_STRPTR)TXT_ScrTitle_Coord_STR},
    {Pal_ActKey_Red,(CONST_STRPTR)Pal_ActKey_Red_STR},
    {Pal_ActKey_Green,(CONST_STRPTR)Pal_ActKey_Green_STR},
    {Pal_ActKey_Blue,(CONST_STRPTR)Pal_ActKey_Blue_STR},
    {TITLE_PaletteReq,(CONST_STRPTR)TITLE_PaletteReq_STR},
    {TITLE_InfoReq,(CONST_STRPTR)TITLE_InfoReq_STR},
    {TXT_ZPower,(CONST_STRPTR)TXT_ZPower_STR},
    {TXT_Unknown,(CONST_STRPTR)TXT_Unknown_STR},
    {TXT_Launch,(CONST_STRPTR)TXT_Launch_STR},
    {TXT_SelectRxScript,(CONST_STRPTR)TXT_SelectRxScript_STR},
    {TXT_Orbit,(CONST_STRPTR)TXT_Orbit_STR},
    {TXT_ERR_OrbitWindow,(CONST_STRPTR)TXT_ERR_OrbitWindow_STR},
    {TXT_OrbitTitle,(CONST_STRPTR)TXT_OrbitTitle_STR},
    {TXT_RenderingMode,(CONST_STRPTR)TXT_RenderingMode_STR},
    {TITLE_PrecisionReq,(CONST_STRPTR)TITLE_PrecisionReq_STR},
    {TXT_MathMode,(CONST_STRPTR)TXT_MathMode_STR},
    {TXT_MathMode_Fast,(CONST_STRPTR)TXT_MathMode_Fast_STR},
    {TXT_MathMode_HighPrec,(CONST_STRPTR)TXT_MathMode_HighPrec_STR},
    {TXT_Math_ChangePrec,(CONST_STRPTR)TXT_Math_ChangePrec_STR},
    {It_TXT_Accept,(CONST_STRPTR)It_TXT_Accept_STR},
    {It_TXT_Cancel,(CONST_STRPTR)It_TXT_Cancel_STR},
    {Prec_TXT_NumOfDigits,(CONST_STRPTR)Prec_TXT_NumOfDigits_STR},
    {Prec_TXT_Accept,(CONST_STRPTR)Prec_TXT_Accept_STR},
    {Prec_TXT_Cancel,(CONST_STRPTR)Prec_TXT_Cancel_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

STATIC CONST UBYTE CatCompBlock[] =
{
    "\x00\x00\x01\x01\x00\x02"
    Pal_TXT_R_STR "\x00"
    "\x00\x00\x01\x03\x00\x02"
    Pal_TXT_G_STR "\x00"
    "\x00\x00\x01\x04\x00\x02"
    Pal_TXT_B_STR "\x00"
    "\x00\x00\x01\x05\x00\x08"
    Pal_TXT_Accept_STR "\x00"
    "\x00\x00\x01\x06\x00\x06"
    Pal_TXT_Reset_STR "\x00"
    "\x00\x00\x01\x07\x00\x08"
    Pal_TXT_Cancel_STR "\x00"
    "\x00\x00\x01\x08\x00\x06"
    Pal_TXT_Copy_STR "\x00"
    "\x00\x00\x01\x09\x00\x06"
    Pal_TXT_Swap_STR "\x00"
    "\x00\x00\x01\x0A\x00\x08"
    Pal_TXT_Spread_STR "\x00"
    "\x00\x00\x01\x0B\x00\x08"
    Pal_TXT_Invert_STR "\x00"
    "\x00\x00\x01\x0C\x00\x04"
    Pal_TXT_ShiftLeft_STR "\x00"
    "\x00\x00\x01\x0D\x00\x04"
    Pal_TXT_ShiftRight_STR "\x00"
    "\x00\x00\x01\x0E\x00\x06"
    Pal_TXT_Undo_STR "\x00"
    "\x00\x00\x01\x14\x00\xF8"
    Info_TXT_Info_STR "\x00\x00"
    "\x00\x00\x01\x15\x00\x04"
    Info_TXT_OK_STR "\x00"
    "\x00\x00\x01\x16\x00\x0A"
    Info_TXT_More_STR "\x00\x00"
    "\x00\x00\x01\x17\x00\xAE"
    Work_TXT_Workgroup_STR "\x00"
    "\x00\x00\x01\x18\x00\x04"
    Work_TXT_OK_STR "\x00"
    "\x00\x00\x01\x19\x00\x0C"
    Cyc_TXT_DelayTime_STR "\x00"
    "\x00\x00\x01\x1A\x00\x08"
    Cyc_TXT_Accept_STR "\x00"
    "\x00\x00\x01\x1B\x00\x08"
    Cyc_TXT_Cancel_STR "\x00"
    "\x00\x00\x01\x1C\x00\x0C"
    It_TXT_Iterations_STR "\x00"
    "\x00\x00\x01\x1D\x00\x12"
    SysI_TXT_CpuInfo_STR "\x00\x00"
    "\x00\x00\x01\x21\x00\x06"
    SysI_TXT_PPCVER_STR "\x00\x00"
    "\x00\x00\x01\x24\x00\x06"
    SysI_TXT_PPCCPU_STR "\x00\x00"
    "\x00\x00\x01\x25\x00\x06"
    SysI_TXT_CPU_PPC_STR "\x00\x00"
    "\x00\x00\x01\x26\x00\x06"
    SysI_TXT_PPCFPU_STR "\x00\x00"
    "\x00\x00\x01\x27\x00\x06"
    SysI_TXT_FPU_PPC_STR "\x00\x00"
    "\x00\x00\x01\x28\x00\x14"
    SysI_TXT_MemInfo_STR "\x00"
    "\x00\x00\x01\x29\x00\x0A"
    SysI_TXT_Available_STR "\x00"
    "\x00\x00\x01\x2A\x00\x06"
    SysI_TXT_AvailChip_STR "\x00"
    "\x00\x00\x01\x2B\x00\x06"
    SysI_TXT_AvailFast_STR "\x00"
    "\x00\x00\x01\x2C\x00\x0E"
    SysI_TXT_LargestFree_STR "\x00\x00"
    "\x00\x00\x01\x2D\x00\x06"
    SysI_TXT_LargestChip_STR "\x00"
    "\x00\x00\x01\x2E\x00\x06"
    SysI_TXT_LargestFast_STR "\x00"
    "\x00\x00\x01\x2F\x00\x0C"
    Coord_TXT_Coord_STR "\x00"
    "\x00\x00\x01\x30\x00\x14"
    Coord_TXT_TopValue_STR "\x00"
    "\x00\x00\x01\x31\x00\x06"
    Coord_TXT_Top_STR "\x00\x00"
    "\x00\x00\x01\x32\x00\x04"
    Coord_TXT_LeftValue_STR "\x00"
    "\x00\x00\x01\x33\x00\x06"
    Coord_TXT_Left_STR "\x00"
    "\x00\x00\x01\x34\x00\x06"
    Coord_TXT_RightValue_STR "\x00\x00"
    "\x00\x00\x01\x35\x00\x08"
    Coord_TXT_Right_STR "\x00\x00"
    "\x00\x00\x01\x36\x00\x14"
    Coord_TXT_BottomValue_STR "\x00"
    "\x00\x00\x01\x37\x00\x08"
    Coord_TXT_Bottom_STR "\x00"
    "\x00\x00\x01\x38\x00\x06"
    Coord_TXT_Julia_STR "\x00"
    "\x00\x00\x01\x39\x00\x14"
    Coord_TXT_JuliaRealValue_STR "\x00"
    "\x00\x00\x01\x3A\x00\x0C"
    Coord_TXT_JuliaReal_STR "\x00"
    "\x00\x00\x01\x3B\x00\x14"
    Coord_TXT_JuliaImagValue_STR "\x00"
    "\x00\x00\x01\x3C\x00\x0C"
    Coord_TXT_JuliaImag_STR "\x00"
    "\x00\x00\x01\x3D\x00\x18"
    Coord_TXT_KeepAxis_STR "\x00\x00"
    "\x00\x00\x01\x3E\x00\x08"
    Coord_TXT_Accept_STR "\x00"
    "\x00\x00\x01\x3F\x00\x08"
    Coord_TXT_Ratio_STR "\x00\x00"
    "\x00\x00\x01\x40\x00\x0A"
    Coord_TXT_Default_STR "\x00\x00"
    "\x00\x00\x01\x41\x00\x08"
    Coord_TXT_Cancel_STR "\x00"
    "\x00\x00\x01\x42\x00\x0E"
    Prt_TXT_AreYouSure_STR "\x00"
    "\x00\x00\x01\x43\x00\x06"
    Prt_TXT_Yes_STR "\x00\x00"
    "\x00\x00\x01\x44\x00\x04"
    Prt_TXT_No_STR "\x00"
    "\x00\x00\x01\x46\x00\x14"
    TITLE_WorkgroupReq_STR "\x00"
    "\x00\x00\x01\x47\x00\x16"
    TITLE_CycleDelayReq_STR "\x00"
    "\x00\x00\x01\x48\x00\x16"
    TITLE_IterationsReq_STR "\x00\x00"
    "\x00\x00\x01\x49\x00\x16"
    TITLE_SysInfoReq_STR "\x00"
    "\x00\x00\x01\x4A\x00\x16"
    TITLE_CoordReq_STR "\x00"
    "\x00\x00\x01\x4B\x00\x10"
    TITLE_PrintReq_STR "\x00"
    "\x00\x00\x01\x4C\x00\x10"
    TITLE_ExitReq_STR "\x00\x00"
    "\x00\x00\x01\x4D\x00\x10"
    TXT_SavePicture_STR "\x00"
    "\x00\x00\x01\x4E\x00\x08"
    TXT_Project_STR "\x00"
    "\x00\x00\x01\x4F\x00\x0A"
    TXT_About_STR "\x00\x00"
    "\x00\x00\x01\x50\x00\x10"
    TXT_SystemInfo_STR "\x00\x00"
    "\x00\x00\x01\x51\x00\x08"
    TXT_Help_STR "\x00"
    "\x00\x00\x01\x52\x00\x10"
    TXT_LoadPicture_STR "\x00"
    "\x00\x00\x01\x53\x00\x10"
    TXT_LoadPalette_STR "\x00"
    "\x00\x00\x01\x54\x00\x10"
    TXT_SavePalette_STR "\x00"
    "\x00\x00\x01\x55\x00\x0A"
    TXT_Print_STR "\x00\x00"
    "\x00\x00\x01\x56\x00\x08"
    TXT_Quit_STR "\x00"
    "\x00\x00\x01\x5A\x00\x08"
    TXT_Options_STR "\x00"
    "\x00\x00\x01\x5B\x00\x06"
    TXT_Title_STR "\x00"
    "\x00\x00\x01\x5C\x00\x0A"
    TXT_TitleBar_STR "\x00"
    "\x00\x00\x01\x5D\x00\x0A"
    TXT_LastTime_STR "\x00"
    "\x00\x00\x01\x5E\x00\x0A"
    TXT_Limits_STR "\x00"
    "\x00\x00\x01\x5F\x00\x0C"
    TXT_Iterations_STR "\x00\x00"
    "\x00\x00\x01\x60\x00\x0A"
    TXT_Custom_STR "\x00"
    "\x00\x00\x01\x61\x00\x0A"
    TXT_Priority_STR "\x00\x00"
    "\x00\x00\x01\x62\x00\x0C"
    TXT_ColorRemap_STR "\x00"
    "\x00\x00\x01\x63\x00\x08"
    TXT_Linear_STR "\x00\x00"
    "\x00\x00\x01\x64\x00\x0A"
    TXT_Repeated_STR "\x00\x00"
    "\x00\x00\x01\x65\x00\x0E"
    TXT_FractalType_STR "\x00\x00"
    "\x00\x00\x01\x66\x00\x06"
    TXT_Julia_STR "\x00"
    "\x00\x00\x01\x67\x00\x0C"
    TXT_Mandelbrot_STR "\x00\x00"
    "\x00\x00\x01\x68\x00\x0A"
    TXT_Processor_STR "\x00"
    "\x00\x00\x01\x69\x00\x0A"
    TXT_Calculate_STR "\x00"
    "\x00\x00\x01\x6A\x00\x08"
    TXT_Preview_STR "\x00"
    "\x00\x00\x01\x6B\x00\x0C"
    TXT_Recalculate_STR "\x00"
    "\x00\x00\x01\x6C\x00\x54"
    NewScr_TXT_Question_STR "\x00\x00"
    "\x00\x00\x01\x6D\x00\x14"
    TITLE_RendReq_STR "\x00"
    "\x00\x00\x01\x6E\x00\x06"
    TXT_Undo_STR "\x00\x00"
    "\x00\x00\x01\x6F\x00\x06"
    TXT_Zoom_STR "\x00\x00"
    "\x00\x00\x01\x70\x00\x06"
    TXT_Stop_STR "\x00\x00"
    "\x00\x00\x01\x71\x00\x06"
    TXT_Video_STR "\x00"
    "\x00\x00\x01\x72\x00\x06"
    TXT_Cycle_STR "\x00"
    "\x00\x00\x01\x73\x00\x08"
    TXT_Forward_STR "\x00"
    "\x00\x00\x01\x74\x00\x0A"
    TXT_Backward_STR "\x00\x00"
    "\x00\x00\x01\x75\x00\x0A"
    TXT_Delay_STR "\x00\x00"
    "\x00\x00\x01\x76\x00\x0C"
    TXT_Palette_STR "\x00\x00"
    "\x00\x00\x01\x77\x00\x10"
    TXT_ScreenMode_STR "\x00\x00"
    "\x00\x00\x01\x78\x00\x12"
    TXT_FontSettings_STR "\x00\x00"
    "\x00\x00\x01\x79\x00\x38"
    TXT_ERR_DiskFontLibrary_STR "\x00"
    "\x00\x00\x01\x7A\x00\x34"
    TXT_ERR_IconLibrary_STR "\x00"
    "\x00\x00\x01\x7B\x00\x38"
    TXT_ERR_GadToolsLibrary_STR "\x00\x00"
    "\x00\x00\x01\x7C\x00\x34"
    TXT_ERR_AslLibrary_STR "\x00\x00"
    "\x00\x00\x01\x7D\x00\x38"
    TXT_ERR_UtilityLibrary_STR "\x00\x00"
    "\x00\x00\x01\x7E\x00\x38"
    TXT_ERR_IFFParseLibrary_STR "\x00"
    "\x00\x00\x01\x7F\x00\x22"
    TXT_ERR_OpenDisplay_STR "\x00"
    "\x00\x00\x01\x80\x00\x24"
    TXT_ERR_NoMemForTempRastPort_STR "\x00\x00"
    "\x00\x00\x01\x81\x00\x0E"
    TXT_ERR_Font_STR "\x00"
    "\x00\x00\x01\x82\x00\x0E"
    TXT_ERR_NoMonitor_STR "\x00\x00"
    "\x00\x00\x01\x83\x00\x0C"
    TXT_ERR_NoChips_STR "\x00\x00"
    "\x00\x00\x01\x84\x00\x0C"
    TXT_ERR_NoMem_STR "\x00"
    "\x00\x00\x01\x85\x00\x10"
    TXT_ERR_NoChipMem_STR "\x00"
    "\x00\x00\x01\x86\x00\x12"
    TXT_ERR_PubNotUnique_STR "\x00\x00"
    "\x00\x00\x01\x87\x00\x10"
    TXT_ERR_UnknownMode_STR "\x00\x00"
    "\x00\x00\x01\x88\x00\x12"
    TXT_ERR_ScreenToDeep_STR "\x00"
    "\x00\x00\x01\x89\x00\x18"
    TXT_ERR_AttachScreen_STR "\x00\x00"
    "\x00\x00\x01\x8A\x00\x16"
    TXT_ERR_ModeNotAvailable_STR "\x00\x00"
    "\x00\x00\x01\x8B\x00\x18"
    TXT_ERR_UnknownErr_STR "\x00\x00"
    "\x00\x00\x01\x8C\x00\x14"
    TXT_ERR_VisualInfo_STR "\x00"
    "\x00\x00\x01\x8D\x00\x14"
    TXT_ERR_Window_STR "\x00"
    "\x00\x00\x01\x8E\x00\x0E"
    TXT_ERR_Menu_STR "\x00"
    "\x00\x00\x01\x8F\x00\x10"
    TXT_ERR_Gadget_STR "\x00\x00"
    "\x00\x00\x01\x90\x00\x16"
    TXT_ERR_WindowGadget_STR "\x00"
    "\x00\x00\x01\x91\x00\x18"
    TXT_ERR_PreviewWindow_STR "\x00"
    "\x00\x00\x01\x92\x00\x16"
    TXT_ERR_CreateDisplay_STR "\x00"
    "\x00\x00\x01\x93\x00\x16"
    TXT_ERR_DimensionInfo_STR "\x00"
    "\x00\x00\x01\x94\x00\x1A"
    TXT_ERR_PaletteRequester_STR "\x00"
    "\x00\x00\x01\x95\x00\x10"
    TXT_ERR_MakeDisplay_STR "\x00"
    "\x00\x00\x01\x96\x00\x1E"
    TXT_ERR_QueryMandPic_STR "\x00\x00"
    "\x00\x00\x01\x97\x00\x16"
    TXT_ERR_LoadMandPic_STR "\x00\x00"
    "\x00\x00\x01\x98\x00\x16"
    TXT_ERR_SaveMandPic_STR "\x00\x00"
    "\x00\x00\x01\x99\x00\x16"
    TXT_ERR_LoadMandPal_STR "\x00\x00"
    "\x00\x00\x01\x9A\x00\x16"
    TXT_ERR_SaveMandPal_STR "\x00\x00"
    "\x00\x00\x01\x9B\x00\x3C"
    TXT_ERR_NoMemForPPC_STR "\x00\x00"
    "\x00\x00\x01\x9C\x00\x32"
    TXT_CMD_Guide_STR "\x00"
    "\x00\x00\x01\x9D\x00\x18"
    TXT_RenderTime_STR "\x00"
    "\x00\x00\x01\x9E\x00\x38"
    TXT_AverageSpeed_STR "\x00\x00"
    "\x00\x00\x01\x9F\x00\x16"
    TXT_PreviewTime_STR "\x00"
    "\x00\x00\x01\xA0\x00\x4A"
    TXT_LeftButtonForJulia_STR "\x00\x00"
    "\x00\x00\x01\xA1\x00\x2E"
    TXT_JuliaConstant_STR "\x00\x00"
    "\x00\x00\x01\xA2\x00\x12"
    TXT_LoadPictureTitle_STR "\x00\x00"
    "\x00\x00\x01\xA3\x00\x12"
    TXT_SavePictureTitle_STR "\x00\x00"
    "\x00\x00\x01\xA4\x00\x0E"
    TXT_LoadPaletteTitle_STR "\x00\x00"
    "\x00\x00\x01\xA5\x00\x0E"
    TXT_SavePaletteTitle_STR "\x00\x00"
    "\x00\x00\x01\xA6\x00\x14"
    TXT_ZoomTime_STR "\x00\x00"
    "\x00\x00\x01\xA7\x00\x14"
    TITLE_RenderReq_STR "\x00"
    "\x00\x00\x01\xA8\x00\x1A"
    TITLE_OverwriteReq_STR "\x00\x00"
    "\x00\x00\x01\xA9\x00\x10"
    Rend_TXT_Question_STR "\x00\x00"
    "\x00\x00\x01\xAA\x00\x34"
    OverWr_TXT_Question_STR "\x00\x00"
    "\x00\x00\x01\xAB\x00\x18"
    TXT_LastCalcTime_STR "\x00\x00"
    "\x00\x00\x01\xAC\x00\x04"
    TXT_OK_STR "\x00\x00"
    "\x00\x00\x01\xAD\x00\x12"
    TXT_FMErrorTitle_STR "\x00"
    "\x00\x00\x01\xAE\x00\x16"
    TXT_ERR_NoSignal_STR "\x00\x00"
    "\x00\x00\x01\xAF\x00\x36"
    TXT_ERR_CantMakeScreenPrivate_STR "\x00\x00"
    "\x00\x00\x01\xB0\x00\x16"
    TXT_ERR_LayoutMenu_STR "\x00\x00"
    "\x00\x00\x01\xB1\x00\x42"
    TXT_ScrTitle_Cyc_STR "\x00\x00"
    "\x00\x00\x01\xB2\x00\x40"
    TXT_ScrTitle_It_STR "\x00"
    "\x00\x00\x01\xB3\x00\x14"
    TXT_ERR_Displayinfo_STR "\x00"
    "\x00\x00\x01\xB4\x00\x32"
    TXT_LessThanOne_STR "\x00"
    "\x00\x00\x01\xB5\x00\x9A"
    TXT_SysInfo_STR "\x00\x00"
    "\x00\x00\x01\xB6\x00\x10"
    TXT_PreviewTitle_STR "\x00\x00"
    "\x00\x00\x01\xB7\x00\x1A"
    TXT_RecalculateTime_STR "\x00"
    "\x00\x00\x01\xB8\x00\x08"
    TXT_YesNo_STR "\x00\x00"
    "\x00\x00\x01\xB9\x00\x0C"
    TXT_OKMore_STR "\x00\x00"
    "\x00\x00\x01\xBA\x00\x90"
    TXT_Info_STR "\x00"
    "\x00\x00\x01\xBB\x00\x06"
    TXT_None_STR "\x00\x00"
    "\x00\x00\x01\xBC\x00\x14"
    TXT_ScrTitle_Coord_STR "\x00"
    "\x00\x00\x01\xBD\x00\x02"
    Pal_ActKey_Red_STR "\x00"
    "\x00\x00\x01\xBE\x00\x02"
    Pal_ActKey_Green_STR "\x00"
    "\x00\x00\x01\xBF\x00\x02"
    Pal_ActKey_Blue_STR "\x00"
    "\x00\x00\x01\xC0\x00\x12"
    TITLE_PaletteReq_STR "\x00"
    "\x00\x00\x01\xC1\x00\x0E"
    TITLE_InfoReq_STR "\x00"
    "\x00\x00\x01\xC2\x00\x06"
    TXT_ZPower_STR "\x00"
    "\x00\x00\x01\xC3\x00\x08"
    TXT_Unknown_STR "\x00"
    "\x00\x00\x01\xC4\x00\x0A"
    TXT_Launch_STR "\x00"
    "\x00\x00\x01\xC5\x00\x20"
    TXT_SelectRxScript_STR "\x00\x00"
    "\x00\x00\x01\xC6\x00\x06"
    TXT_Orbit_STR "\x00"
    "\x00\x00\x01\xC7\x00\x16"
    TXT_ERR_OrbitWindow_STR "\x00\x00"
    "\x00\x00\x01\xC8\x00\x0E"
    TXT_OrbitTitle_STR "\x00\x00"
    "\x00\x00\x01\xC9\x00\x10"
    TXT_RenderingMode_STR "\x00\x00"
    "\x00\x00\x01\xCA\x00\x12"
    TITLE_PrecisionReq_STR "\x00\x00"
    "\x00\x00\x01\xCB\x00\x0A"
    TXT_MathMode_STR "\x00"
    "\x00\x00\x01\xCC\x00\x06"
    TXT_MathMode_Fast_STR "\x00\x00"
    "\x00\x00\x01\xCD\x00\x1A"
    TXT_MathMode_HighPrec_STR "\x00"
    "\x00\x00\x01\xCE\x00\x14"
    TXT_Math_ChangePrec_STR "\x00"
    "\x00\x00\x01\xCF\x00\x08"
    It_TXT_Accept_STR "\x00"
    "\x00\x00\x01\xD0\x00\x08"
    It_TXT_Cancel_STR "\x00"
    "\x00\x00\x01\xD1\x00\x10"
    Prec_TXT_NumOfDigits_STR "\x00"
    "\x00\x00\x01\xD2\x00\x08"
    Prec_TXT_Accept_STR "\x00"
    "\x00\x00\x01\xD3\x00\x08"
    Prec_TXT_Cancel_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


#ifndef PROTO_LOCALE_H
 #ifndef __NOLIBBASE__
  #define _NLB_DEFINED_
  #define __NOLIBBASE__
 #endif
 #ifndef __NOGLOBALIFACE__
  #define _NGI_DEFINED_
  #define __NOGLOBALIFACE__
 #endif
 #include <proto/locale.h>
 #ifdef _NLB_DEFINED_
  #undef __NOLIBBASE__
  #undef _NLB_DEFINED_
 #endif
 #ifdef _NGI_DEFINED_
  #undef __NOGLOBALIFACE__
  #undef _NGI_DEFINED_
 #endif
#endif

struct LocaleInfo
{
#ifndef __amigaos4__
    struct Library     *li_LocaleBase;
#else
    struct LocaleIFace *li_ILocale;
#endif
    struct Catalog     *li_Catalog;
};


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CONST_STRPTR GetString(struct LocaleInfo *li, LONG stringNum);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifdef CATCOMP_CODE

CONST_STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
#ifndef __amigaos4__
    struct Library     *LocaleBase = li->li_LocaleBase;
#else
    struct LocaleIFace *ILocale    = li->li_ILocale;
#endif
    LONG         *l;
    UWORD        *w;
    CONST_STRPTR  builtIn = NULL;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum && l < (LONG *)(&CatCompBlock[sizeof(CatCompBlock)]))
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    if (*l == stringNum)
    {
        builtIn = (CONST_STRPTR)((ULONG)l + 6);
    }

#ifndef __amigaos4__
    if (LocaleBase)
    {
        return GetCatalogStr(li->li_Catalog, stringNum, builtIn);
    }
#else
    if (ILocale)
    {
#ifdef __USE_INLINE__
        return GetCatalogStr(li->li_Catalog, stringNum, builtIn);
#else
        return ILocale->GetCatalogStr(li->li_Catalog, stringNum, builtIn);
#endif
    }
#endif
    return builtIn;
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* FM_REACTIONCD_H */
