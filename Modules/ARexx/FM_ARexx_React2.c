/*
 *  FlashMandel - FM_ARexx_React2.c
 *
 *  Copyright (C) 2005 - 2022  Edgar Schwan
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted,
 *  provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed
 *  as patches to released version.
 *
 *  This software is provided "as is" without express or implied warranty.
 */
/*
 *    $Id: FM_ARexx_React2.c,v 2.0  dpapararo Exp $
 *
 *    Functions for Reaction-ARexx (Part 2).
 *
 *    $Log: FM_ARexx_React.c,v $
 *
 *    Revision 2.0  2022/1/04 00:00:00  dpapararo
 *	  Implemented OS4 API Calls where possible
 *	  Cleaned code
 *    Revision 1.9  2021/1/17 00:00:00  dpapararo
 *    Modified DrawFractal function parameters
 *
 *    Revision 1.4  2020/12/12 00:00:00  eschwan
 *    Modified for "no reactor" code
 *
 *    Revision 1.3  2020/03/19 00:00:00  dpapararo
 *    Minor fixes.
 *
 *    Revision 1.2  2005/02/26 00:00:00  eschwan
 *    MANDChunk is now a pointer.
 *
 *    Revision 1.1  2004/10/22 00:00:00  eschwan
 *    Bugfixes in ARexxFunc_SetCoordinates(), ARexxFunc_SetPalette(), ARexxFunc_SetScreenAttrs(), ARexxFunc_SetFont()
 *
 *    Revision 1.0  2004/10/05 00:00:00  eschwan
 *    First version.
 *
 */

#ifdef __amigaos4__
#define __USE_INLINE__
#endif /* __amigaos4__ */

#define NEW_REXX_SYNTAX

#include <exec/types.h>
#include <exec/libraries.h>
#include <rexx/errors.h>
#include <rexx/storage.h>
#include <classes/arexx.h>

#define NO_PROTOS
#include <iffp/ilbmapp.h>
#undef NO_PROTOS

#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#ifdef __GNUC__
#include <proto/intuition.h>
#include <proto/rexxsyslib.h>
#include <proto/locale.h>
#ifndef __amigaos4__
#include <clib/alib_protos.h>
#endif /* !__amigaos4__ */
#else /* __GNUC__ */
#include <clib/intuition_protos.h>
#include <clib/rexxsyslib_protos.h>
#include <clib/locale_protos.h>
#include <clib/alib_protos.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/rexxsyslib_pragmas.h>
#include <pragmas/locale_pragmas.h>
#endif /* __GNUC__ */

#ifndef __amigaos4__
#include "Compilerspecific.h"
#else /* !__amigaos4__ */
#include <amiga_compiler.h>
#define GetRexxVar(a,b,c) GetRexxVarFromMsg(b,*(c),a)	/* Is now part of rexxsyslib */
#define SetRexxVar(a,b,c,d) SetRexxVarFromMsg(b,c,a)
#endif /* !__amigaos4__ */

#include <GMP/gmp.h>

#include "FM_ARexx_React.h"
#include "FM_ReactionBasics.h"

#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include "FM_PalettePref_React.h"
#include "FM_CoordReq_React.h"
#include "FM_ConfirmReq_React.h"

#include "Headers/FlashMandel.h"

#ifdef FM_AREXX_SUPPORT

extern uint8 *PIXMEM, *GFXMEM, *ARGBMEM, *RGBMEM;
extern uint32 *RNDMEM, PIXELVECTOR[4];
extern int16 UNDOCOUNTER;
extern struct Catalog *CatalogPtr;
extern int16 ZOOMLINE[5 * 2];
extern struct ILBMInfo MYILBM;
extern char FMSCREENNAME [MAXPUBSCREENNAME + 1];
extern struct Menu *MAINMENU;
extern uint32 ELAPSEDTIME, MASK;
extern struct Border MYBORDER;
extern uint16 ZOOMPOINTER [];
extern int16 res;
extern struct MandelChunk *MANDChunk;
extern uint32 *PALETTE;
extern int32 PRIORITY, DELAY;
extern int16 MX1, MY1, MX2, MY2, W, H;
extern struct TextAttr MYFONTSTRUCT;

/* ARexxFunc_SetCoordinates(): Handle ARexx-Command 'SET_COORDINATES'.   Template: "STEM" */
void ARexxFunc_SetCoordinates (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
#ifdef __amigaos4__
  char valbuf[100] = { 0 };
  STRPTR value = &valbuf;
#else /* __amigaos4__ */
  STRPTR value = NULL;
#endif /* __amigaos4__ */
  float64 tmp_RMIN, tmp_RMAX, tmp_IMIN, tmp_IMAX;
  float64 tmp_JKRE, tmp_JKIM;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */

  	if (stem)
    {
      	if (IsNumber (stem))
		{
	  		cmd->ac_RC = RC_ERROR;
	  		cmd->ac_RC2 = ERR10_018;
	  		return;
		}			/* invalid argument to function */

      	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.RMIN", stem);
      	if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
		{
	  		//if (!(GetRexxVarFromMsg(&varbuf, value, rm))) {
	  		tmp_RMIN = atof ((const STRPTR) value);
	  		if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
	    	{
	      		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.RMAX", stem);

	      		if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
				{
		  			tmp_RMAX = atof ((const STRPTR) value);
		  			if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
		    		{
		      			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.IMIN", stem);

		      			if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
						{
			  				tmp_IMIN = atof ((const STRPTR) value);
			  				if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
			    			{
			      				SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.IMAX", stem);

			      				if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
								{
				  					tmp_IMAX = atof ((const STRPTR) value);
				  					if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
				    				{
				      					SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.JKRE", stem);

				      					if (! (GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
										{
					  						tmp_JKRE = atof ((const STRPTR) value);
					  						if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
					    					{
					      						SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.JKIM", stem);

					      						if (! (GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
												{
						  							tmp_JKIM = atof ((const STRPTR) value);
						  							if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
						    						{
						      							if ((tmp_RMIN >= tmp_RMAX) || (tmp_IMIN >= tmp_IMAX))
														{
							  								cmd->ac_RC = RC_ERROR;
							  								cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
														}
														
						      							else
														{
							  								SaveCoords (MYILBM.win);
							  								MANDChunk->RMin = tmp_RMIN;
							  								MANDChunk->RMax = tmp_RMAX;
							  								MANDChunk->IMin = tmp_IMIN;
							  								MANDChunk->IMax = tmp_IMAX;
							  								MANDChunk->JKre = tmp_JKRE;
							  								MANDChunk->JKim = tmp_JKIM;
															
              												mpf_set_d (MANDChunk->GRMin, MANDChunk->RMin);
              												mpf_set_d (MANDChunk->GRMax, MANDChunk->RMax);
              												mpf_set_d (MANDChunk->GIMin, MANDChunk->IMin);
															mpf_set_d (MANDChunk->GIMax, MANDChunk->IMax);													
              												mpf_set_d (MANDChunk->GJKre, MANDChunk->JKre);
              												mpf_set_d (MANDChunk->GJKim, MANDChunk->JKim);
															
							  								cmd->ac_RC = RC_OK;
							  								cmd->ac_RC2 = 0;
														}
						    						}
													
						  							else
						    						{
						      							cmd->ac_RC = RC_ERROR;
						      							cmd->ac_RC2 = ERR10_018;
						    						}	/* invalid argument to function */
												}
					    					}
											
					  						else
					    					{
					      						cmd->ac_RC = RC_ERROR;
					      						cmd->ac_RC2 = ERR10_018;
					    					}	/* invalid argument to function */
										}
				    				}
									
				  					else
				    				{
				      					cmd->ac_RC = RC_ERROR;
				      					cmd->ac_RC2 = ERR10_018;
				    				}	/* invalid argument to function */
								}
			    			}
							
			  				else
			    			{
			      				cmd->ac_RC = RC_ERROR;
			      				cmd->ac_RC2 = ERR10_018;
			    			}	/* invalid argument to function */
						}
		    		}
					
		  			else
		    		{
		      			cmd->ac_RC = RC_ERROR;
		      			cmd->ac_RC2 = ERR10_018;
		    		}		/* invalid argument to function */
				}
	    	}
			
	  		else
	    	{
	      		cmd->ac_RC = RC_ERROR;
	      		cmd->ac_RC2 = ERR10_018;
	    	}			/* invalid argument to function */
		}
    }
	
	else
    {
		int16 res1 = FALSE, res2 = FALSE;
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;

      	ModifyIDCMP (MYILBM.win, NULL);
      	ClearMenuStrip (MYILBM.win);
#ifdef FM_REACT_SUPPORT
		res1 = Do_CoordinatesRequest (MYILBM.win, FMSCREENNAME, 0, 0);
		
		if (res1)
		{
	  		res2 = Do_RenderRequest (MYILBM.win, FMSCREENNAME, 0, 0);
		}
#else /* FM_REACT_SUPPORT */
      	res1 = ShowCoords (MYILBM.win);
		
      	if (res1)
		{
	  		res2 = Choice (MYILBM.win, CATSTR (TITLE_RenderReq), CATSTR (Rend_TXT_Question));
		}
#endif /* FM_REACT_SUPPORT */
      	else cmd->ac_RC = RC_WARN;
      	ResetMenuStrip (MYILBM.win, MAINMENU);
      	ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);

      	if (res2)
		{
	  		if (ZMASK &MASK)
	    	{
	      		DrawBorder (MYILBM.wrp, &MYBORDER, 0, 0);
	      		MASK &= ~ZMASK;
	    	}
			
	  		SetMenuStop (&MYILBM);
	  		PutPointer (MYILBM.win, 0, 0, 0, 0, 0, BUSY_POINTER);
	  		ELAPSEDTIME = DrawFractal (MANDChunk, MYILBM.win, ARGBMEM, RGBMEM, PIXMEM, GFXMEM, PIXELVECTOR, RNDMEM, TRUE);
	  		PutPointer (MYILBM.win, &ZOOMPOINTER, ZPW, ZPH, ZPXO, ZPYO, ZOOM_POINTER);
	  		SetMenuStart (&MYILBM, UNDOCOUNTER);
	  		ShowTime (MYILBM.win, CATSTR (TXT_RecalculateTime), ELAPSEDTIME, FALSE);
		}
		
      	else cmd->ac_RC = RC_WARN;
    }
}

void ARexxFunc_GetCoordinates (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
  char valbuf[100] = { 0 };

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */
  	
	if (IsNumber (stem))
    {
      	cmd->ac_RC = RC_ERROR;
      	cmd->ac_RC2 = ERR10_018;
      	return;
    }				/* invalid argument to function */

  	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.RMIN", stem);
  	sprintf ((STRPTR) &valbuf, "%f", MANDChunk->RMin);
  	if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.RMAX", stem);
      	sprintf ((STRPTR) &valbuf, "%f", MANDChunk->RMax);
      	if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
		{
	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.IMIN", stem);
	  		sprintf ((STRPTR) &valbuf, "%f", MANDChunk->IMin);
	  		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
	    	{
	      		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.IMAX", stem);
	      		sprintf ((STRPTR) &valbuf, "%f", MANDChunk->IMax);
	      		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
				{
		  			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.JKRE", stem);
		  			sprintf ((STRPTR) &valbuf, "%f", MANDChunk->JKre);
		  			if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
		    		{
		      			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.JKIM", stem);
		      			sprintf ((STRPTR) &valbuf, "%f", MANDChunk->JKim);
		      			if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
						{
			  				cmd->ac_RC = RC_OK;
			  				cmd->ac_RC2 = 0;
						}
		    		}
				}
	    	}
		}
    }
}

/* ARexxFunc_GetIterations(): Handle ARexx-Command 'GET_MANDChunk->Iterations'.  Template: "VAR/A" */

void ARexxFunc_GetIterations (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (var)) return;
  
  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) MANDChunk->Iterations);

  	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_GetPower(): Handle ARexx-Command 'GET_POWER'.  Template: "VAR/A" */
void ARexxFunc_GetPower (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (var)) return;

  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) (1 << MANDChunk->Power));

   	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_GetPriority(): Handle ARexx-Command 'GET_PRIORITY'.  Template: "VAR/A" */

void ARexxFunc_GetPriority (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (var)) return;

  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) PRIORITY);
  	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_GetColorRemap(): Handle ARexx-Command 'GET_COLOR_REMAP'.  Template: "VAR/A" */
void ARexxFunc_GetColorRemap (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };
  int s;
  uint32 Index = 0;
  CONST uint32 bit_idx[MAX_COLOR_REMAP][2] = { {LINEAR_BIT, 1}, {LOG_BIT, 2}, {REPEATED_BIT, 3}, {SQUARE_BIT, 4}, {ONE_BIT, 5}, {TWO_BIT, 6}, {THREE_BIT, 7}, {FOUR_BIT, 8} };

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (var)) return;

  	for (s = MIN_COLOR_REMAP - 1; s < MAX_COLOR_REMAP; s++)
    {
      	if (MANDChunk->Flags & bit_idx[s][0])
		{
	  		Index = bit_idx[s][1];
	  		break;
		}
    }

  	if (!(Index))
    {
      	cmd->ac_RC2 = 0;
      	return;
    }

  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", Index);

  	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_GetFractalType(): Handle ARexx-Command 'GET_FRACTAL_TYPE'.  Template: "VAR/A" */
void ARexxFunc_GetFractalType (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };
  int32 Index = 0;

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (var)) return;

  	if (MANDChunk->Flags & MANDEL_BIT) Index = 2;
  	else if (MANDChunk->Flags & JULIA_BIT) Index = 1;
  		 else return;

  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", Index);

  	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_GetCPUType(): Handle ARexx-Command 'GET_CPU_TYPE'.  Template: "VAR/A" */
void ARexxFunc_GetCPUType (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };
  int32 Index = 0;

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (var)) return;

  	if (MANDChunk->Flags & MC68K_BIT) Index = 1;
  	else if (MANDChunk->Flags & PPC_BIT) Index = 2;
  		 else return;

  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", Index);
  	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_SetFrame(): Handle ARexx-Command 'SET_FRAME'.   Template: "STEM/A" */
void ARexxFunc_SetFrame (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
  int16 tmp_left, tmp_top, tmp_right, tmp_bottom;
#ifdef __amigaos4__
  char valbuf[100] = { 0 };
  STRPTR value = &valbuf;
#else /* __amigaos4__ */
  STRPTR value = NULL;
#endif /* __amigaos4__ */
  struct Screen *scr = NULL;

  	cmd->ac_RC = RC_ERROR;
  	cmd->ac_RC2 = ERR10_018;	/* invalid argument to function */
  	if (IsNumber (stem)) return;

  	if (scr = LockPubScreen (FMSCREENNAME))
    {
      	if (stem)
		{
	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.LEFT", stem);	/* frame-left */
	  		if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
	    	{
	      		tmp_left = atol ((const STRPTR) value);
	      		if ((errno == ERANGE) || (!(IsNumber ((STRPTR) value))) || (tmp_left < 0)) goto Error;

	      		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.TOP", stem);	/* frame-top */
	      		if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
				{
		  			tmp_top = atol ((const STRPTR) value);
		  			if ((errno == ERANGE) || (!(IsNumber ((STRPTR) value))) || (tmp_top < 0)) goto Error;

		  			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.RIGHT", stem);	/* frame-right */
		  			if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
		    		{
		      			tmp_right = atol ((const STRPTR) value);
		      			if ((errno == ERANGE) || (!(IsNumber ((STRPTR) value))) || (tmp_right < 0))	goto Error;

		      			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.BOTTOM", stem);	/* frame-bottom */
		      			if (! (GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
						{
			  				tmp_bottom = atol ((const STRPTR) value);
			  				if ((errno == ERANGE) || (!(IsNumber ((STRPTR) value))) || (tmp_bottom < 0)) goto Error;

			  				if (tmp_right > (scr->Width - 1)) tmp_right = scr->Width - 1;
			  				if (tmp_bottom > (scr->Height - 1)) tmp_bottom = scr->Height - 1;
			  				if ((tmp_right > tmp_left) && (tmp_bottom > tmp_top) && (tmp_right < scr->Width) && (tmp_bottom < scr->Height))
			    			{
			      				int16 width = tmp_right - tmp_left + 1;
			      				int16 height = tmp_bottom - tmp_top + 1;

			      				if (ZMASK & MASK)
								{
				  					DrawBorder (MYILBM.wrp, &MYBORDER, 0, 0);
				  					MASK &= ~ZMASK;
								}

			      				if (width > height) (width = ScalerDiv ((int32) height, (uint32) MYILBM.win->Width, (uint32) MYILBM.win->Height));
			      				else (height = ScalerDiv ((int32) width, (uint32) MYILBM.win->Height, (uint32) MYILBM.win->Width));

			      				ZOOMLINE[8] = ZOOMLINE[0] = ZOOMLINE[6] = tmp_left;
			      				ZOOMLINE[1] = ZOOMLINE[3] = ZOOMLINE[9] = tmp_top;
			      				ZOOMLINE[2] = ZOOMLINE[4] = tmp_left + width - 1;
			      				ZOOMLINE[5] = ZOOMLINE[7] = tmp_top + height - 1;
			      				MX1 = tmp_left;
			      				MY1 = tmp_top;
			      				W = width, H = height;

			      				DrawBorder (MYILBM.wrp, &MYBORDER, 0, 0);
			      				MASK |= ZMASK;

			      				cmd->ac_RC = RC_OK;
			      				cmd->ac_RC2 = 0;
			    			}
			  
			  				else  MASK &= ~ZMASK;

			  				if (ZMASK & MASK)
			    			{
			      				OnMenu (MYILBM.win, FULLMENUNUM (2, 0, NOSUB));
			      				OnMenu (MYILBM.win, FULLMENUNUM (2, 6, NOSUB));
			    			}
			  				
							else
			    			{
			      				OffMenu (MYILBM.win, FULLMENUNUM (2, 0, NOSUB));
			      				OffMenu (MYILBM.win, FULLMENUNUM (2, 6, NOSUB));
			    			}
						}
		    		}
				}
	    	}
		}
    
Error:
      	UnlockPubScreen (FMSCREENNAME, scr);
      	return;
    }
}

void ARexxFunc_GetFrame (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
  char valbuf[100] = { 0 };

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */

  	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.LEFT", stem);
  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) ZOOMLINE[6]);
  	if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.TOP", stem);
      	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) ZOOMLINE[3]);
      	if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
		{
	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.RIGHT", stem);
	  		SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) ZOOMLINE[4]);
	  		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
	    	{
	      		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.BOTTOM", stem);
	      		SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) ZOOMLINE[5]);
	      		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
				{
		  			cmd->ac_RC = RC_OK;
		  			cmd->ac_RC2 = 0;
				}
	    	}
		}
    }
}

/* ARexxFunc_GetDelay(): Handle ARexx-Command 'GET_DELAY'.  Template: "VAR/A" */
void ARexxFunc_GetDelay (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR var = (STRPTR) cmd->ac_ArgList[0];
  char valbuf[100] = { 0 };

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */

  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) DELAY);
  	if (! (SetRexxVar (rm, (STRPTR) var, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	cmd->ac_RC = RC_OK;
      	cmd->ac_RC2 = 0;
    }
}

/* ARexxFunc_SetPalette(): Handle ARexx-Command 'SET_PALETTE'.  Template: "STEM,FAST/S" */
void ARexxFunc_SetPalette (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  int16 fast = (uint32) cmd->ac_ArgList[1];
  char varbuf[100] = { 0 };
  uint32 pal_length, num_colors;
  struct loadrgb *TempPal = NULL;
  struct RGB (*TempPalColors)[1] = NULL;
  struct RGB (*PaletteColors)[1] = (struct RGB (*)[1]) (((uint32) * (&PALETTE)) + sizeof (struct loadrgb_head));
  struct DrawInfo *dri = NULL;
  uint32 pen;
  int16 success = FALSE;
#ifdef __amigaos4__
  char valbuf[100] = { 0 };
  STRPTR value = &valbuf;
#else /* __amigaos4__ */
#ifdef NEW_REXX_SYNTAX
  char valbuf[100] = { 0 };
#endif /* NEW_REXX_SYNTAX */
  uint32 *value = NULL;
#endif /* __amigaos4__ */
  uint32 cval, rgb8 = 0L;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */

  	if (stem)
    {
      	if (IsNumber (stem))
		{	
	  		cmd->ac_RC = RC_ERROR;
	  		cmd->ac_RC2 = ERR10_018;
	  		return;
		}			/* invalid argument to function */

      	if (dri = GetScreenDrawInfo (MYILBM.win->WScreen))
		{
	  		num_colors = 1L << dri->dri_Depth;
	  		pal_length = sizeof (struct loadrgb_head) + ((num_colors + 1) * sizeof (struct RGB));
	  		if (TempPal = (struct loadrgb *) AllocVec (pal_length, MEMF_ANY | MEMF_CLEAR))
	    	{
	      		TempPal->loadrgb_lrgb.lrgb_count = num_colors;
	      		TempPal->loadrgb_lrgb.lrgb_first = 0;
	      		TempPalColors =	(struct RGB (*)[1]) (((uint32) * (&TempPal)) + sizeof (struct loadrgb_head));
	      		GetRGB32 (((struct ViewPort *) ViewPortAddress (MYILBM.win))->ColorMap, 0L, num_colors, (uint32 *) TempPalColors);

	      		for (pen = 0; pen < num_colors; pen++)
				{
		  			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.%ld", stem, pen);
				  	if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
		    		{
#ifdef __amigaos4__
		      			if (Stricmp (&varbuf, &valbuf))
						{
			  				sscanf (value, "%lx", &rgb8);
#else /* __amigaos4__ */
#ifndef NEW_REXX_SYNTAX
		      				if (value)
							{
			  					rgb8 = *value;
#else /* !NEW_REXX_SYNTAX */
		      					if (value)
								{
			  						sscanf ((STRPTR) value, "%lx", &rgb8);
#endif /* !NEW_REXX_SYNTAX */
#endif /* __amigaos4__ */
			  						cval = rgb8 & 0xFF;
			  						rgb8 >>= 8;
			  						TempPalColors[pen]->RGB_Blue = cval | (cval << 8) | (cval << 16) | (cval << 24);
			  						cval = rgb8 & 0xFF;
			  						rgb8 >>= 8;
			  						TempPalColors[pen]->RGB_Green = cval | (cval << 8) | (cval << 16) | (cval << 24);
			  						cval = rgb8 & 0xFF;
			  						TempPalColors[pen]->RGB_Red = cval | (cval << 8) | (cval << 16) | (cval << 24);
			  						success = TRUE;
								}
		    				}
		  					
							else
		    				{
		      					success = FALSE;
		      					break;
		    				}
						}

	      				if (success == TRUE)
						{
		  					if (fast) LoadRGB32 (ViewPortAddress (MYILBM.win), (APTR) TempPal);
		  					else Fade (MYILBM.win, ARGBMEM, RGBMEM, (uint32 *) TempPal, 25L, 1L, FROMBLACK);
		  					GetRGB32 (((struct ViewPort *) ViewPortAddress (MYILBM.win))->ColorMap, 0L, num_colors, (uint32 *) PaletteColors);
		  					cmd->ac_RC = RC_OK;
		  					cmd->ac_RC2 = 0;
						}
	      				
						else
						{
		  					cmd->ac_RC = RC_ERROR;
		  					cmd->ac_RC2 = ERR10_018;
						}		/* invalid argument to function */
	      				FreeVec (TempPal);
	    			}
	  				
					else
	    			{
	      				cmd->ac_RC = RC_ERROR;
	      				cmd->ac_RC2 = ERR10_003;
	    			}			/* no memory available   */
	  
	  				FreeScreenDrawInfo (MYILBM.win->WScreen, dri);
				}
    		}

  			else
    		{
      			ModifyIDCMP (MYILBM.win, NULL);
      			ClearMenuStrip (MYILBM.win);
#ifdef FM_REACT_SUPPORT
      			res = Do_PalettePrefRequest (MYILBM.win, FMSCREENNAME, 0, 0, (struct loadrgb *) PALETTE);
#else /* FM_REACT_SUPPORT */
      			res = ModifyPalette (MYILBM.win, MYILBM.win->LeftEdge + 25, MYILBM.win->TopEdge + 35, PALETTE);
#endif /* FM_REACT_SUPPORT */
      			if (!res)
				{
	  				DisplayError (MYILBM.win, TXT_ERR_PaletteRequester, 15L);
	  				cmd->ac_RC = RC_ERROR;
	  				cmd->ac_RC2 = ERR10_012;	/* error return from function */
				}
      			
				else
				{
	  				cmd->ac_RC = RC_OK;
	  				cmd->ac_RC2 = 0;
				}
      			
				ResetMenuStrip (MYILBM.win, MAINMENU);
      			ModifyIDCMP (MYILBM.win, IDCMP_STANDARD);
    		}
}

/* ARexxFunc_GetPalette(): Handle ARexx-Command 'GET_PALETTE'.  Template: "STEM/A" */
void ARexxFunc_GetPalette (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
#if defined(__amigaos4__) || defined(NEW_REXX_SYNTAX)
  char valbuf[100] = { 0 };
#endif /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
  struct RGB (*PaletteColors)[1] = (struct RGB (*)[1]) (((uint32) * (&PALETTE)) + sizeof (struct loadrgb_head));
  struct DrawInfo *dri = NULL;
  uint32 pen;
  uint32 value = 0L, num_colors;

  int16 success = TRUE;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */
  	if (IsNumber (stem))
    {
      	cmd->ac_RC = RC_ERROR;
      	cmd->ac_RC2 = ERR10_018;
      	return;
    }				/* invalid argument to function */

  	if (dri = GetScreenDrawInfo (MYILBM.win->WScreen))
    {
      	num_colors = 1L << dri->dri_Depth;

      	for (pen = 0; pen < num_colors; pen++)
		{
	  		value = (PaletteColors[pen]->RGB_Blue & 0xFF) | (PaletteColors[pen]->RGB_Green & 0xFF00) | (PaletteColors[pen]->RGB_Red & 0xFF0000);
	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.%ld", stem, pen);
		
#if defined(__amigaos4__) || defined(NEW_REXX_SYNTAX)
	  		SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%lx", value);

	  		if (SetRexxVar (rm, (CONST_STRPTR) &varbuf, (CONST_STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf)))
	    	{
#else /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
	  			if (SetRexxVar (rm, (CONST_STRPTR) &varbuf, (CONST_STRPTR) &value, sizeof (uint32)))
	    		{
#endif /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
	      			success = FALSE;
	      			break;
	    		}
			}
      
	  		FreeScreenDrawInfo (MYILBM.win->WScreen, dri);
    	}
  
  		if (success != FALSE)
    	{
      		cmd->ac_RC = RC_OK;
      		cmd->ac_RC2 = 0;
    	}
}

/* ARexxFunc_SetScreenAttrs(): Handle ARexx-Command 'SET_SCREENATTRS'.   Template: "STEM,FAST/S" */
void ARexxFunc_SetScreenAttrs (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  int16 fast = (uint32) cmd->ac_ArgList[1];
  char varbuf[100] = { 0 };
  struct FM_RxCmd_SET_SCREENATTRS *args = NULL;
  uint32 tmp_mode;
  int16 tmp_width, tmp_height, tmp_depth, tmp_oscan;
#ifdef __amigaos4__
  char valbuf[100] = { 0 };
  STRPTR value = &valbuf;
#else /* __amigaos4__ */
  uint32 *value = NULL;
#endif /* __amigaos4__ */

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */

  	if (stem)
    {
      	if (IsNumber (stem))
		{
	  		cmd->ac_RC = RC_ERROR;
	  		cmd->ac_RC2 = ERR10_018;
	  		return;
		}			/* invalid argument to function */

      	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.MODE", stem);	/* screen-mode */
      	if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
		{
#if defined(__amigaos4__) || defined(NEW_REXX_SYNTAX)
	  		sscanf ((STRPTR) value, "%lx", &tmp_mode);
#else /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
	  		tmp_mode = *value;
#endif /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
	  		if (!(ModeNotAvailable (tmp_mode)))
	    	{
	      		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.WIDTH", stem);	/* screen-width */
	      		if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
				{
		  			tmp_width = atol ((const STRPTR) value);
		  			if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
		    		{
		      			if (!tmp_width) tmp_width = STDSCREENWIDTH;
		      			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.HEIGHT", stem);	/* screen-height */
		      			if (! (GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
						{
			  				tmp_height = atol ((const STRPTR) value);
			  				if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
			    			{
			      				if (!tmp_height) tmp_height = STDSCREENHEIGHT;
			      				SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.DEPTH", stem);	/* screen-depth */
			      				if (! (GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
								{
				  					tmp_depth = atol ((const STRPTR) value);
				  					if ((errno != ERANGE) && (IsNumber ((STRPTR) value)))
				    				{
				      					if (!tmp_depth)	tmp_depth = 8;
				      					SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.OSCANTYPE", stem);	/* overscan-type */
				      					if (! (GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))					
										{
					  						STRPTR tmpstr = (STRPTR) value;
					  						if (!(Stricmp (tmpstr, "TEXT"))) tmp_oscan = OSCAN_TEXT;
					  						else if (!(Stricmp (tmpstr, "STANDARD"))) tmp_oscan = OSCAN_STANDARD;
					  							else if (!(Stricmp (tmpstr, "MAX"))) tmp_oscan = OSCAN_MAX;
					  								else if (!(Stricmp (tmpstr, "VIDEO"))) tmp_oscan = OSCAN_VIDEO;
					  									else tmp_oscan = OSCAN_TEXT;
										}
				      					
										else return;
				    				}
				  				
									else
					    			{
					      				cmd->ac_RC = RC_ERROR;
					      				cmd->ac_RC2 = ERR10_018;
					      				return;
					    			}	/* invalid argument to function */
								}
			      			
								else return;
				    		}
			  
				  			else
				    		{
				      			cmd->ac_RC = RC_ERROR;
				      			cmd->ac_RC2 = ERR10_018;
				      			return;
				    		}	/* invalid argument to function */
						}
		      		
						else return;
			    	}
		  
			  		else
			    	{
			      		cmd->ac_RC = RC_ERROR;
			      		cmd->ac_RC2 = ERR10_018;
			      		return;
			    	}		/* invalid argument to function */
				}
	      
			  	else return;
		    }
	  
		  	else
		    {
	      		cmd->ac_RC = RC_ERROR;
	      		cmd->ac_RC2 = ERR10_018;
	      		return;
	    	}			/* invalid argument to function */
		}
      
		else return;
    }
  	
	else
    {
      tmp_mode = INVALID_ID;
      tmp_width = STDSCREENWIDTH;
      tmp_height = STDSCREENHEIGHT, tmp_depth = 8;
      tmp_oscan = OSCAN_TEXT;
    }

#if 1				/* external */
  	if (args = (struct FM_RxCmd_SET_SCREENATTRS *) AllocVec (sizeof (struct FM_RxCmd_SET_SCREENATTRS), MEMF_ANY | MEMF_CLEAR))
	{
      	args->FMRC_SETSCR_ModeID = tmp_mode;
      	args->FMRC_SETSCR_Width = tmp_width;
      	args->FMRC_SETSCR_Height = tmp_height;
      	args->FMRC_SETSCR_Depth = tmp_depth;
      	args->FMRC_SETSCR_OScanType = tmp_oscan;
      	args->FMRC_SETSCR_Fast = fast;
      	if (AddARexxEvent (NEWDISPLAY_MSG, (APTR) args))
		{
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
      	
		else FreeVec (args);
    }
#else /* internal */
  	continue;
#endif
}

/* ARexxFunc_GetScreenAttrs(): Handle ARexx-Command 'GET_SCREENATTRS'.   Template: "STEM/A" */
void ARexxFunc_GetScreenAttrs (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
#if defined(__amigaos4__) || defined(NEW_REXX_SYNTAX)
  char valbuf[100] = { 0 };
#endif /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
  struct Screen *scr = NULL;
  struct DrawInfo *di = NULL;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */
  	if (IsNumber (stem))
    {
      	cmd->ac_RC = RC_ERROR;
      	cmd->ac_RC2 = ERR10_018;
      	return;
    }				/* invalid argument to function */

  	if (scr = LockPubScreen (FMSCREENNAME))
    {
      	if (di = GetScreenDrawInfo (scr))
		{
	  		uint32 modeid = GetVPModeID ((CONST struct ViewPort *) &scr->ViewPort);
	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.MODE", stem);

#if defined(__amigaos4__) || defined(NEW_REXX_SYNTAX)
	  		SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%08X", modeid);

	  		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf,(int32) Strlen ((const STRPTR) &valbuf))))
	    	{
#else /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
	  		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) & modeid, sizeof (uint32))))
	    	{
#endif /* defined(__amigaos4__) || defined(NEW_REXX_SYNTAX) */
				SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.WIDTH", stem);
	      		SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) scr->Width);
		      	if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
				{
		  			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.HEIGHT", stem);
		  			SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) scr->Height);
			  		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
		    		{
		      			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.DEPTH", stem);
		      			SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) di->dri_Depth);	     
      					if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
						{
			  				cmd->ac_RC = RC_OK;
			  				cmd->ac_RC2 = 0;
						}
		    		}
				}
	    	}
	  
	  		FreeScreenDrawInfo (scr, di);
		}
      
	  	UnlockPubScreen (FMSCREENNAME, scr);
    }
}

/* ARexxFunc_SetFont(): Handle ARexx-Command 'SET_FONT'.  Template: "STEM,FAST/S" */
void ARexxFunc_SetFont (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  int16 fast = (uint32) (cmd->ac_ArgList[1]);
  char varbuf[100] = { 0 };
  struct FM_RxCmd_SET_FONT *args = NULL;
  STRPTR tmp_fontname = DEF_FONTNAMESTR;
  uint16 tmp_size = DEF_FONTSIZE, size1 = 0, temp;
  uint16 tmp_style = MYFONTSTRUCT.ta_Style & ~(FSF_UNDERLINED | FSF_BOLD | FSF_ITALIC | FSF_EXTENDED);
  STRPTR tmp_style_str = NULL;
#ifdef __amigaos4__
  char valbuf[100] = { 0 };
  STRPTR value = &valbuf;
#else /* __amigaos4__ */
  uint32 *value = NULL;
#endif /* __amigaos4__ */

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */

  	if (stem)
    {
      	if (IsNumber (stem))
		{
	  		cmd->ac_RC = RC_ERROR;
	  		cmd->ac_RC2 = ERR10_018;
	  		return;
		}			/* invalid argument to function */

      	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.FONTNAME", stem);	/* font-name */

      	if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
		{
	  		if (value) tmp_fontname = (STRPTR) value;

	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.SIZE", stem);	/* font-size */

	  		if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
	    	{
	      		temp = atol ((const STRPTR) value);
	      		if ((errno |= ERANGE) && (IsNumber ((STRPTR) value)))
				{
		  			if (temp) tmp_size = temp;

		  			SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.STYLE", stem);	/* font-style */

		  			if (!(GetRexxVar (rm, (STRPTR) &varbuf, (STRPTR *) &value)))
		    		{
		      			tmp_style_str = (STRPTR) value;
		      			if (!(tmp_style_str)) tmp_style_str = "NORMAL";

		      			if (Stricmp (tmp_style_str, "NORMAL"))
						{
			  				STRPTR result = NULL;
			  				STRPTR templ = tmp_style_str;

			  				while (1L)
			    			{
			      				if (!(result = strtok (templ, ","))) break;
			      				if (!(Stricmp (result, "ITALIC"))) tmp_style |= FSF_ITALIC;
			      				else if (!(Stricmp (result, "BOLD"))) tmp_style |= FSF_BOLD;
			      					else if (!(Stricmp (result, "UNDERLINED")))	tmp_style |= FSF_UNDERLINED;
			      						else if (!(Stricmp (result, "EXTENDED"))) tmp_style |= FSF_EXTENDED;
			      				templ = NULL;
			    			}

						}
		    		}
		  
		  			else
		    		{
		      			cmd->ac_RC = RC_ERROR;
		      			cmd->ac_RC2 = ERR10_018;
		      			return;
		    		}		/* invalid argument to function */
				}
	      		
				else return;
	    	}
	  
	  		else
	    	{
	      		cmd->ac_RC = RC_ERROR;
	      		cmd->ac_RC2 = ERR10_018;
	      		return;
	    	}			/* invalid argument to function */
	
		}
      	
		else
		{
	  		cmd->ac_RC = RC_ERROR;
	  		cmd->ac_RC2 = ERR10_018;
	  		return;
		}			/* invalid argument to function */
    }
  
  	else tmp_fontname = NULL;

#if 1				/* external */
  	if (tmp_fontname) size1 = Strlen ((const STRPTR) tmp_fontname) + 1;
  	if (args = (struct FM_RxCmd_SET_FONT *) AllocVec (sizeof (struct FM_RxCmd_SET_FONT) + size1, MEMF_ANY | MEMF_CLEAR))
    {
      	if (size1)
		{
	  		STRPTR Str = (STRPTR) ((uint32) args + sizeof (struct FM_RxCmd_SET_FONT));
	  		// Strlcpy (Str, (const STRPTR) tmp_fontname, sizeof (Str));	  
	  		strcpy (Str, (const STRPTR) tmp_fontname);
	  		args->FMRC_SETFNT_FontName = (STRPTR) Str;
		}
      
	  	else args->FMRC_SETFNT_FontName = NULL;
      	
		args->FMRC_SETFNT_Size = tmp_size;
      	args->FMRC_SETFNT_Style = tmp_style;
      	if (AddARexxEvent (FONTREQ_MSG, (APTR) args))
		{
	  		cmd->ac_RC = RC_OK;
	  		cmd->ac_RC2 = 0;
		}
      	
		else FreeVec (args);
    }
#else /* internal */
  continue;
#endif
}

/* ARexxFunc_GetFont(): Handle ARexx-Command 'GET_FONT'.  Template: "STEM/A" */
void ARexxFunc_GetFont (REG (a0, struct ARexxCmd *cmd), REG (a1, struct RexxMsg *rm))
{
  STRPTR stem = (STRPTR) cmd->ac_ArgList[0];
  char varbuf[100] = { 0 };
  char valbuf[100] = { 0 };
  uint32 offset = 0;

  	cmd->ac_RC = RC_FATAL;
  	cmd->ac_RC2 = ERR10_012;	/* error return from function */
  	if (IsNumber (stem))
    {
      	cmd->ac_RC = RC_ERROR;
      	cmd->ac_RC2 = ERR10_018;
      	return;
    }				/* invalid argument to function */

  	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.FONTNAME", stem);
  	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%s", MYFONTSTRUCT.ta_Name);
 
  	if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
    {
      	SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.SIZE", stem);
      	SNPrintf ((STRPTR) &valbuf, sizeof (valbuf), "%ld", (int32) MYFONTSTRUCT.ta_YSize);
      
		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
		{
	  		uint16 style = MYFONTSTRUCT.ta_Style;
	  		if (style & FSF_ITALIC)
	    		offset += SNPrintf ((STRPTR) ((uint32) &valbuf + offset), sizeof ((STRPTR) ((uint32) &valbuf + offset)), "ITALIC,");
	  		if (style & FSF_BOLD)
				offset += SNPrintf ((STRPTR) ((uint32) &valbuf + offset), sizeof ((STRPTR) ((uint32) &valbuf + offset)), "BOLD,");
	  		if (style & FSF_UNDERLINED)
	    		offset += SNPrintf ((STRPTR) ((uint32) &valbuf + offset), sizeof ((STRPTR) ((uint32) &valbuf + offset)), "UNDERLINED,");
	  		if (style & FSF_EXTENDED)
	    		offset += SNPrintf ((STRPTR) ((uint32) &valbuf + offset), sizeof ((STRPTR) ((uint32) &valbuf + offset)), "EXTENDED,");
	  		if (offset)
	    		valbuf[--offset] = 0;
	  		else
    			Strlcpy ((STRPTR) &valbuf, "NORMAL", sizeof (valbuf));
	  
	  		SNPrintf ((STRPTR) &varbuf, sizeof (varbuf), "%s.STYLE", stem);

	  		if (! (SetRexxVar (rm, (STRPTR) &varbuf, (STRPTR) &valbuf, (int32) Strlen ((const STRPTR) &valbuf))))
	    	{
	      		cmd->ac_RC = RC_OK;
	      		cmd->ac_RC2 = 0;
	    	}	
		}
    }
}

#else /* FM_AREXX_SUPPORT */
	extern struct Library *ResourceBase;	/* dummy */
#endif /* FM_AREXX_SUPPORT */
