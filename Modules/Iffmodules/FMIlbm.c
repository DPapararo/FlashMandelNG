/* Modified for ARexx-support by E. Schwan 23.1.2002 */
/* Modified for StormC-GCC-support by E. Schwan 08.06.2002 */
/* various fixes - dpapararo 19.03.2020 */
/* assume we have a GimmeZeroZero window - dpapararo 05.01.2021 */
/* fixes and enanchements - dpapararo 24-02-21 */
/* rewritten query, load and save functions for dataypes usage - dpapararo 18.04.2021 */
/* saved username and copyright infos inside coordinates file and palette ilbm - dpapararo 24.04.2021 */
/* fixed load 16bit pictures - 22 dec 2024 dpapararo */

#define __USE_INLINE__

#include <exec/types.h>

#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/iffparse.h>

#include <datatypes/pictureclass.h> /* for struct BitMapHeader */
#include <intuition/icclass.h>

#define NO_PROTOS
#include <iffp/Ilbmapp.h>
#undef NO_PROTOS

#include "headers/FlashMandel.h"

#define FROMBLACK 0

/* When used on global definitions, static means private.
 * This keeps these names, which are only referenced in this
 * module, from conficting with same-named objects inyour program.
 */
static int32 stdio_stream (struct Hook *, struct IFFHandle *, struct IFFStreamCmd *);

int32 getcolors (struct ILBMInfo *ilbm);
int32 alloccolortable (struct ILBMInfo *ilbm);
int32 loadcmap (struct ILBMInfo *ilbm);
int32 setcolors (struct ILBMInfo *ilbm, struct ViewPort *vp);
int32 putcmap (struct IFFHandle *iff, APTR colortable, uint16 ncolors, uint16 bitspergun);
int32 openifile (struct ParseInfo *pi, uint8 * filename, uint32 iffopenmode);
int32 parseifile (struct ParseInfo *pi, int32 groupid, int32 grouptype, int32 * propchks, int32 * collectchks, int32 * stopchks);
int32 chkcnt (int32 * taggedarray);
int32 currentchunkis (struct IFFHandle *iff, int32 type, int32 id);
int32 contextis (struct IFFHandle *iff, int32 type, int32 id);
int32 getcontext (struct IFFHandle *iff);
void initiffasstdio (struct IFFHandle *iff);
int32 PutCk (struct IFFHandle *iff, int32 id, int32 size, void *data);

extern struct Chunk COPYRIGHT_CHUNK;
extern struct Chunk USERNAME_CHUNK;

/******************************************************************************
**
**  Coded by Dino Papararo 18-Apr-2021
**
**
**  FUNCTION
**
**    QueryMandFile -- Examine a FlashMandel coordinates file.
**
**  SYNOPSIS
**
**    int32 QueryMandFile (struct ILBMInfo *,struct MandelChunk *,STRPTR);
**
**  DESCRIPTION
**
**    Passed an initialized ILBMInfo, a FlashMandel cordinates info structure
**
**    and a filename, QuesryMandFile function will test if coordinates infos
**
**    are present or not.
**
**    Returns 0 for success.
**
******************************************************************************/
int32 QueryMandFile (struct ILBMInfo *ilbm,struct LoadSaveFMChunk *LSFMChunk, STRPTR FileName)
{
BPTR InFile = NULL;
char MYPATH2[MAX_PATHLEN+5];
int32 Error = TRUE;

	Strlcpy (MYPATH2, FileName, sizeof (MYPATH2));
	Strlcat (MYPATH2, ".fmng", sizeof (MYPATH2));	
	if (InFile = Open (MYPATH2, MODE_OLDFILE))
	{
		if (Read (InFile, LSFMChunk, sizeof (struct LoadSaveFMChunk)) == -1) DisplayBeep (ilbm->win->WScreen);			
		else Error = FALSE;
	
		Close (InFile);
	}
	
	else // fallback to old method with coordinates inside ilbmpicture
	{
		if (ilbm->ParseInfo.iff = AllocIFF ())
		{
			if (QueryMandPic (ilbm, LSFMChunk, FileName)) DisplayBeep (ilbm->win->WScreen);			
    		else Error = FALSE;
			
			FreeIFF (ilbm->ParseInfo.iff);
		}
	}
	
	return Error;
}

/* findpropdata
 *
 * finds specified chunk parsed from IFF file, and
 *   returns pointer to its sp_Data (or 0 for not found)
 */
uint8 *findpropdata (struct IFFHandle * iff, int32 type, int32 id)
{
  register struct StoredProperty *sp = NULL;

  	if (sp = FindProp (iff, type, id)) return (sp->sp_Data);
  	return (0);
}

/*----------------------------------------------------------------------*/

/* queryilbm
 *
 * Passed an initilized ILBMInfo with a not-in-use IFFHandle,
 *   and a filename, will open an ILBM, looking for FMNG chunk
 *   to read saved coordinates and close the ILBM.
 *
 * Returns 0 for success or an IFFERR (libraries/iffparse.h)
 */
int32 QueryMandPic (struct ILBMInfo * ilbm, struct LoadSaveFMChunk * LSFMManChk, STRPTR filename)
{
  int32 error = 0L;
  struct LoadSaveFMChunk *TmpLSFMChk = NULL;

  	if (!(ilbm->ParseInfo.iff)) return (CLIENT_ERROR);

  	if (!(error = openifile ((struct ParseInfo *) ilbm, filename, IFFF_READ)))
    {
      	error =	parseifile ((struct ParseInfo *) ilbm, ID_FORM, ID_ILBM, ilbm->ParseInfo.propchks, ilbm->ParseInfo.collectchks, ilbm->ParseInfo.stopchks);

      	if ((error == 0L) || (error == IFFERR_EOC) || (error == IFFERR_EOF))
		{
	  		if (contextis (ilbm->ParseInfo.iff, ID_ILBM, ID_FORM))
	    	{
	      		if (TmpLSFMChk = (struct LoadSaveFMChunk *) findpropdata (ilbm->ParseInfo.iff, ID_ILBM, ID_FMNG))
				{
		  			CopyMem ((APTR) TmpLSFMChk, (APTR) LSFMManChk, (uint32) sizeof (struct LoadSaveFMChunk));  
				}

	      		else error = NOFILE;
	    	}

	  		else
	    	{
	      		message (SI (MSG_ILBM_NOILBM));
	      		error = NOFILE;
	    	}
		}

      	closeifile ((struct ParseInfo *) ilbm);
    }

  	return (error);
}

/* LoadMandPic
 * Passed an initialized ILBMInfo and a filename, 
 * will load an ILBM into your already opened ilbm->win, 
 * Function uses datatypes library. 
 *
 * Returns 0 for success.
 */
int32 LoadMandPic (struct ILBMInfo *ilbm, STRPTR filename)
{
  int32 Error;
  uint32 Width, Height, Depth;
  Object *dto = NULL;
  struct BitMapHeader *bmhd = NULL;
  uint32 *cregs = NULL;
  uint32 num_colors = 0;
  uint32 colortable32 [3*256+2];

  	Error = TRUE;
  	Width = ilbm->Bmhd.w;
  	Height = ilbm->Bmhd.h;
  	Depth = ilbm->Bmhd.nPlanes;

  	if ((Depth >= MIN_DEPTH) || (Depth <= MAX_DEPTH))
  	{
     	if((dto = NewDTObject (filename,
                           		DTA_SourceType, DTST_FILE,
                           		DTA_GroupID, GID_PICTURE,
     					   		PDTA_Screen,ilbm->scr,
						   		PDTA_Remap,FALSE,
						   		PDTA_UseFriendBitMap,TRUE,						   
                           		PDTA_DestMode, PMODE_V43,   
                           		TAG_DONE)))
     	{
	 		GetDTAttrs (dto,
						PDTA_BitMapHeader,&bmhd,
						PDTA_CRegs,&cregs,
						PDTA_NumColors,&num_colors,
						TAG_DONE);
		
			if (bmhd)
			{		
				SetDTAttrs (dto,
							NULL, NULL,
		    				GA_Left, 0,
		    				GA_Top, 0,
		    				GA_Width, Width,
		    				GA_Height, Height,
		    				ICA_TARGET, ICTARGET_IDCMP,
		    				TAG_DONE);
			
		    		AddDTObject (ilbm->win, NULL, dto, -1);
		    		RefreshDTObjects (dto, ilbm->win, NULL, TAG_DONE);
					RemoveDTObject (ilbm->win, dto);
				
				if (Depth <= MIN_DEPTH)
				{
					colortable32 [0] = num_colors << 16 + 0;
					CopyMem ((APTR) (cregs),(APTR) (&colortable32 [1]),(uint32) (sizeof (Color32) * num_colors));
					colortable32 [3 * num_colors + 1] = 0;
					LoadRGB32 (ViewPortAddress (ilbm->win), colortable32);		
				}
			}
		
			DisposeDTObject (dto);
			Error = FALSE;
	 	}		
  	}			

  	return Error;
}

/*----------------------------------------------------------------------------*/
/* Alloc a new bitmap and copy the window contents into it                    */
/*----------------------------------------------------------------------------*/
struct BitMap *gzzarea (struct Window *win)
{
int32 w,h,d;
struct BitMap *winbm = win->RPort->BitMap;
struct BitMap *bm = NULL;
struct RastPort rp;

	w = win->GZZWidth;
	h = win->GZZHeight;
	d = GetBitMapAttr (winbm,BMA_DEPTH);

	if (d > 8) bm = AllocBitMap (w,h,24,BMF_CLEAR|BMF_DISPLAYABLE|BMF_INTERLEAVED,winbm);
	else bm = AllocBitMap (w,h,d,0,winbm);

	if (bm)
	{
		ClearMem (&rp, sizeof (struct RastPort));
		InitRastPort (&rp);
		rp.BitMap = bm;
		ClipBlit (win->RPort,0,0,&rp,0,0,w,h,0xc0);
	}

	return (bm);
}

/*----------------------------------------------------------------------------*/
/* Save the inner contents of a GZZ window as IFF picture using datatypes     */
/*----------------------------------------------------------------------------*/
int32 savewindow (char *name,struct Window *win)
{
  struct BitMap *bm = NULL;
  Object *o = NULL;
  struct BitMapHeader *bmhd = NULL;
  uint8 *cmap = NULL;
  uint32 *cregs = NULL;
  int32 i;
  int32 w,h,d;
  int32 ncols;
  BPTR fhand;
  struct ViewPort *vp = NULL;
  int32 Error;

    Error = TRUE;

	if (bm = gzzarea (win))
	{
		w = win->GZZWidth;
		h = win->GZZHeight;
		d = GetBitMapAttr (bm,BMA_DEPTH);

		ncols = (d > 8 ? 0 : 1 << d);

		vp = ViewPortAddress (win); 
		
		if (o = NewDTObject (NULL,
				DTA_SourceType,DTST_RAM,
				DTA_GroupID,GID_PICTURE,
				PDTA_BitMap,bm,
				PDTA_ModeID,GetVPModeID(vp),
				(ncols ? PDTA_NumColors : PDTA_DestMode),(ncols ? ncols : PMODE_V43),
				TAG_DONE))
		{

			//SetDTAttrs (o,NULL,NULL,DTA_ObjCopyright,&COPYRIGHT_CHUNK,DTA_ObjAuthor,&USERNAME_CHUNK,TAG_DONE);

			GetDTAttrs (o,PDTA_BitMapHeader,&bmhd,TAG_DONE);

			bmhd->bmh_Width  = w;
			bmhd->bmh_Height = h;
			bmhd->bmh_Depth  = d;
			bmhd->bmh_XAspect = 22;
			bmhd->bmh_YAspect = 22;
			bmhd->bmh_PageWidth = (w <= 320 ? 320 : w <= 640 ? 640 : w <= 1024 ? 1024 : w <= 1280 ? 1280 : 1600);
			bmhd->bmh_PageHeight = bmhd->bmh_PageWidth * 3 / 4;

			if (ncols)
			{
				GetDTAttrs (o, PDTA_ColorRegisters, &cmap, PDTA_CRegs, &cregs, TAG_DONE);
				GetRGB32 (vp->ColorMap, 0, ncols, cregs);
				for (i = 3*ncols; i; i--)
					*cmap++ = (*cregs++) >> 24;
			}

			if (fhand = Open (name,MODE_NEWFILE))
			{
				i = DoDTMethod (o, NULL, NULL, DTM_WRITE, NULL, fhand, DTWM_IFF, NULL);
				Close (fhand);
				if (i)
					Error = FALSE;
				else
					Delete (name);
			}

			DisposeDTObject (o);
		}
		else FreeBitMap (bm);
	}

	return Error;
}

/******************************************************************************
**  FUNCTION
**
**    SaveMandPic -- save a window as IFF picture.
**
**  SYNOPSIS
**
**    int32 SaveMandPic (struct ILBMInfo *,struct LoadSaveFMChunk *,STRPTR);
**
**  DESCRIPTION
**
**    Function uses an initialized  ILBMInfo structure, an initialized  
**
**    LoadSaveFMChunk structure, and a pointer to a FileName to save a window
**
**	  bitmap into a file using datatypes library.
**
**    Returns 0 for success.
******************************************************************************/

int32 SaveMandPic (struct ILBMInfo *ilbm, struct LoadSaveFMChunk *LSFMChunk,STRPTR UserName,STRPTR Copyright,STRPTR FileName)
{
BPTR OutFile = NULL;
char MYPATH2[MAX_PATHLEN+5];
int32 Error;

	Error = TRUE;
	Strlcpy (MYPATH2, FileName, sizeof (MYPATH2));
	Strlcat (MYPATH2, ".fmng", sizeof (MYPATH2));

	if (OutFile = Open (MYPATH2, MODE_NEWFILE))
	{	
		if (Write (OutFile, LSFMChunk, sizeof (struct LoadSaveFMChunk)) != -1)		
			if (Write (OutFile, UserName, MAX_FILELEN) != -1)					
				if (Write (OutFile, Copyright, BARLEN) != -1)					
					Error = FALSE;
		
		if (Error) DisplayBeep (ilbm->win->WScreen);		
		
		Close (OutFile);
	}
	
	else DisplayBeep (ilbm->win->WScreen);
	
	Error = savewindow (FileName,ilbm->win);

	return Error;
}	
	
int32 LoadPalette (struct ILBMInfo * ilbm, STRPTR filename)
{
  int32 error;

  	error = openifile ((struct ParseInfo *) ilbm, filename, IFFF_READ);

  	if (!error)
    {
      	error =	parseifile ((struct ParseInfo *) ilbm, ID_FORM, ID_ILBM, ilbm->ParseInfo.propchks, ilbm->ParseInfo.collectchks, ilbm->ParseInfo.stopchks);

      	if ((!error) || (error == IFFERR_EOC) || (error == IFFERR_EOF))
		{
	  		if (!(error = getcolors (ilbm)))
	    	{
	      		setcolors (ilbm, ilbm->vp);

				freecolors (ilbm);
	    	}
		}

      	closeifile ((struct ParseInfo *) ilbm);
    }

  	return (error);
}

int32 SavePalette (struct ILBMInfo *ilbm, struct Chunk *chunk1, struct Chunk *chunk2, STRPTR filename)
{
  struct IFFHandle *iff = NULL;
  Color32 *colortable32 = NULL;
  uint16 ncolors;
  int32 size, error;

  	iff = ilbm->ParseInfo.iff;
  	ncolors = ilbm->vp->ColorMap->Count;
  	error = openifile ((struct ParseInfo *) ilbm, filename, IFFF_WRITE);

  	if (!error)
    {
      	error = PushChunk (iff, ID_ILBM, ID_FORM, IFFSIZE_UNKNOWN);

      	if (colortable32 = (Color32 *) AllocVec ((uint32) (sizeof (Color32) * ncolors), MEMF_PRIVATE | MEMF_CLEAR))
		{
	  		GetRGB32 (ilbm->vp->ColorMap, 0, ncolors, (uint32 *) colortable32);
	  		CkErr (putcmap (iff, colortable32, ncolors, 32));
	  		FreeVec (colortable32);
		}

 	  	size = ((chunk1->ch_Size == IFFSIZE_UNKNOWN) ? strlen (chunk1->ch_Data) : chunk1->ch_Size);
		CkErr (PutCk (iff, chunk1->ch_ID, size, chunk1->ch_Data));

	  	size = ((chunk2->ch_Size == IFFSIZE_UNKNOWN) ? strlen (chunk2->ch_Data) : chunk2->ch_Size);
	    CkErr (PutCk (iff, chunk2->ch_ID, size, chunk2->ch_Data));
		
      	CkErr (PopChunk (iff)); /* close out the FORM */
      	closeifile ((struct ParseInfo *) ilbm); /* and the file */
    }

  	return (error);
}

/* ----------- getcolors ------------- */
/* getcolors - allocates a ilbm->colortable for at least 32 registers
 *      and loads CMAP colors into it, setting ilbm->ncolors to number
 *      of colors actually loaded.
 *
 * V39 and above: unless ilbm->IFFPFlags & IFFPF_NOCOLOR32, will also
 *  allocate and build a 32-bit per gun colortable (ilbm->colortable32)
 *  and ilbm->colorrecord for LoadRGB32().  
 */
int32 getcolors (struct ILBMInfo * ilbm)
{
  struct IFFHandle *iff = NULL;
  int32 error;

  	if (!(iff = ilbm->ParseInfo.iff)) return (CLIENT_ERROR);

  	if (!(error = alloccolortable (ilbm))) error = loadcmap (ilbm);
  	if (error) freecolors (ilbm);
  	D (bug ("getcolors: error = %ld\n", error));
  	return (error);
}

/* alloccolortable - allocates ilbm->colortable and sets ilbm->ncolors
 *  to the number of colors we have room for in the table.
 *
 * V39 and above: unless ilbm->IFFPFlags & IFFPF_NOCOLOR32, will also
 *  allocate and build a 32-bit per gun colortable (ilbm->colortable32)
 *  and ilbm->colorrecord for LoadRGB32()
 */
int32 alloccolortable (struct ILBMInfo * ilbm)
{
  struct IFFHandle *iff = NULL;
  struct StoredProperty *sp = NULL;

  int32 error = CLIENT_ERROR;
  uint32 ctabsize;
  USHORT ncolors;

  	if (!(iff = ilbm->ParseInfo.iff)) return (CLIENT_ERROR);

  	if (sp = FindProp (iff, ID_ILBM, ID_CMAP))
    {
      	/*
      	 * Compute the size table we need
      	 */
      	ncolors = sp->sp_Size / 3; /* how many in CMAP */
      	ncolors = MAX (ncolors, 32); /* alloc at least 32 */

      	ctabsize = ncolors * sizeof (Color4);
      	if (ilbm->colortable = (Color4 *) AllocVecTags (ctabsize,AVT_Type,MEMF_PRIVATE,TAG_DONE))
		{
	  		ilbm->ncolors = ncolors;
	  		ilbm->ctabsize = ctabsize;
	  		error = 0L;

	  		if ((!(ilbm->IFFPFlags & IFFPF_NOCOLOR32)))
	    	{
	      		ctabsize = (ncolors * sizeof (Color32)) + (2 * sizeof (int32));
	      		if (ilbm->colorrecord = (uint16 *) AllocVecTags (ctabsize,
												AVT_Type, MEMF_PRIVATE,
												AVT_Contiguous, TRUE, 
												AVT_Lock, TRUE,  
												AVT_Alignment, 16,
                        						AVT_ClearWithValue, 0, TAG_DONE))
				{
		  			ilbm->crecsize = ctabsize;
		  			ilbm->colortable32 = (Color32 *) (&ilbm->colorrecord [2]);
		  			ilbm->colorrecord [0] = ncolors; /* For LoadRGB32 */
		  			ilbm->colorrecord [1] = 0;
				}
	      		
				else error = IFFERR_NOMEM;
	    	}
		}
      
	  	else error = IFFERR_NOMEM;
    }
  	
	D (bug ("alloccolortable for %ld colors: error = %ld\n", ncolors, error));
  	if (error) freecolors (ilbm);
  
  	return (error);
}

void freecolors (struct ILBMInfo *ilbm)
{
  	if (ilbm->colortable)
    {
      	FreeVec (ilbm->colortable);
    }
  
  	ilbm->colortable = NULL;
  	ilbm->ctabsize = 0;

  	if (ilbm->colorrecord)
    {
      	FreeVec (ilbm->colorrecord);
    }
  
  	ilbm->colorrecord = NULL;
  	ilbm->colortable32 = NULL;
  	ilbm->crecsize = 0;
}

/* loadcmap - note interface change for V39
 *
 * Passed ILBMInfo
 *
 * Sets ncolors (and colorrecord if using it) to the number actually read.
 *
 *  New for V39 and above: If bmhd->flags BMHDF_CMAPOK is set,
 *  or if ILBMInfo->IFFPFlags IFFPF_CMAPOK is set, the 32-bit gun code
 *  will assume the CMAP contains 8-bit significant guns (R,G,B)
 *  and will not scale apparent 4-bit nibbles to 8 bits prior to
 *  scaling to 32 bits.  In the absence of either of these flags,
 *  if whole usable CMAP contains RGB values whose low nibbles are all 0,
 *  this code will first scale the RGB values to 8 bits ($30 becomes $33, etc)
 */
int32 loadcmap (struct ILBMInfo *ilbm)
{
  struct StoredProperty *sp = NULL;
  int32 k;
  uint32 ncolors, gun, ncheck;
  uint8 *rgb= NULL, rb, gb, bb;
  uint32 nc, r, g, b;
  struct IFFHandle *iff = NULL;
  int16 AllShifted;

  	if (!(iff = ilbm->ParseInfo.iff)) return (CLIENT_ERROR);

  	if (!(ilbm->colortable))
    {
      	message (SI (MSG_ILBM_NOCOLORS));
      	return (1);
    }

  	if (!(sp = FindProp (iff, ID_ILBM, ID_CMAP))) return (1L);

  	rgb = sp->sp_Data;

  	/* file has this many colors */
  	nc = sp->sp_Size / sizeofColorRegister;
  	ncolors = nc;

  	/* if ILBMInfo can't hold that many, we'll load less */
  	if (ilbm->ncolors < ncolors) ncolors = ilbm->ncolors;
  	/* set to how many we are loading */
  	ilbm->ncolors = ncolors;

  	/* how many colors to check for shifted nibbles (i.e. used colors) */
  	ncheck = 1 << ilbm->Bmhd.nPlanes;
  	if (ncheck > ncolors) ncheck = ncolors;

  	if ((!(ilbm->IFFPFlags & IFFPF_NOCOLOR32)) && (ilbm->colorrecord))
    {
      	ilbm->colorrecord[0] = ncolors;

      	/* Assign to 32-bit table, examine for all-shifted nibbles at same time */
      	AllShifted = TRUE;
      	k = 0;
      	while (ncheck--)
		{
	  		ilbm->colortable32[k].r = rb = *rgb++;
	  		ilbm->colortable32[k].g = gb = *rgb++;
	  		ilbm->colortable32[k].b = bb = *rgb++;
	  		if (((rb & 0x0F) || (gb & 0x0F) || (bb & 0x0F))) AllShifted = FALSE;
	  		k++;
		}

      	/* If no file/user indication that this is an 8-bit significant CMAP... */
      	if ((!(ilbm->IFFPFlags & IFFPF_CMAPOK)) && (!(ilbm->Bmhd.flags & BMHDF_CMAPOK)))
		{
	  		/* If all nibbles appear shifted (4 bit), duplicate the nibbles */
	  		if (AllShifted)
	    	{
	      		for (k = 0; k < nc; k++)
				{
		  			ilbm->colortable32[k].r |= (ilbm->colortable32[k].r >> 4);
		  			ilbm->colortable32[k].g |= (ilbm->colortable32[k].g >> 4);
		  			ilbm->colortable32[k].b |= (ilbm->colortable32[k].b >> 4);
				}
	    	}
		}

      	/* Now scale to 32 bits */
      	for (k = 0; k < nc; k++)
		{
	  		gun = ilbm->colortable32[k].r;
	  		ilbm->colortable32[k].r |= ((gun << 24) | (gun << 16) | (gun << 8));
	  		gun = ilbm->colortable32[k].g;
	  		ilbm->colortable32[k].g |= ((gun << 24) | (gun << 16) | (gun << 8));
	  		gun = ilbm->colortable32[k].b;
	  		ilbm->colortable32[k].b |= ((gun << 24) | (gun << 16) | (gun << 8));
		}
    }

  	/* always make old-style table */
  	rgb = sp->sp_Data;
  	ncolors = nc;
  	k = 0;
  	while (ncolors--)
    {
      	r = (*rgb++ & 0xF0) << 4;
      	g = *rgb++ & 0xF0;
      	b = *rgb++ >> 4;
      	ilbm->colortable[k] = r | g | b;
      	k++;
    }
  
  	return (0L);
}

/* setcolors - sets vp to ilbm->colortable or ilbm->colortable32
 *
 * V39 and above: unless ilbm->IFFPFlags & IFFPF_NOCOLOR32, will
 *  use 32-bit per gun colortable (ilbm->colortable32) and functions
 *
 * Returns client error if there is no ilbm->vp
 */
int32 setcolors (struct ILBMInfo *ilbm, struct ViewPort *vp)
{  
  int32 nc;
  int32 error = 0L;

  	if (!(vp)) return (CLIENT_ERROR);

  	nc = MIN (ilbm->ncolors, vp->ColorMap->Count);
  	
	if ((!(ilbm->IFFPFlags & IFFPF_NOCOLOR32)) && (ilbm->colorrecord))
    { 
      	LoadRGB32 (vp, (uint32 *) ilbm->colorrecord);
    }
  
  	else if (ilbm->colortable)
    {
      	LoadRGB4 (vp, (uint16 *) ilbm->colortable, nc);
    }
  
  	error = CLIENT_ERROR;
  	return (error);
}

/*---------- putcmap ---------------------------------------------------*/
/* This function will accept a table of color values in one of the
 * following forms:
 *  if bitspergun=4,  colortable is words, each with nibbles 0RGB
 *  if bitspergun=8,  colortable is bytes of RGBRGB etc. (like a CMAP)
 *  if bitspergun=32, colortable is longs of RGBRGB etc.
 *     (only the high eight bits of each gun will be written to CMAP)
 */
int32 putcmap (struct IFFHandle *iff, APTR colortable, uint16 ncolors, uint16 bitspergun)
{
  int32 error, offs;
  uint16 *tabw = NULL;
  uint8 *tab8 = NULL;
  ColorRegister cmapReg;

  	D (bug ("In PutCMAP\n"));

  	if ((!iff) || (!colortable)) return (CLIENT_ERROR);

  	/* size of CMAP is 3 bytes * ncolors */
  	if (error = PushChunk (iff, NULL, ID_CMAP, ncolors * sizeofColorRegister)) return (error);

  	D (bug ("Pushed ID_CMAP, error = %ld\n", error));

  	if (bitspergun == 4)
    {
      	/* Store each 4-bit value n as nn */
      	tabw = (uint16 *) colortable;
      	for (; ncolors; --ncolors)
		{
	  		cmapReg.red = (*tabw >> 4) & 0xf0;
	  		cmapReg.red |= (cmapReg.red >> 4);

	  		cmapReg.green = (*tabw) & 0xf0;
	  		cmapReg.green |= (cmapReg.green >> 4);
		
	  		cmapReg.blue = (*tabw << 4) & 0xf0;
	  		cmapReg.blue |= (cmapReg.blue >> 4);

	  		if ((WriteChunkBytes (iff, (BYTE *) & cmapReg, sizeofColorRegister)) != sizeofColorRegister) return (IFFERR_WRITE);
	  		++tabw;
		}
    }
  	
	else if ((bitspergun == 8) || (bitspergun == 32))
    {
      	tab8 = (uint8 *) colortable;
      	offs = (bitspergun == 8) ? 1 : 4;
      	for (; ncolors; --ncolors)
		{
	  		cmapReg.red = *tab8;
	  		tab8 += offs;
	  		cmapReg.green = *tab8;
	  		tab8 += offs;
	  		cmapReg.blue = *tab8;
	  		tab8 += offs;
	  		
			if ((WriteChunkBytes (iff, (BYTE *) & cmapReg, sizeofColorRegister)) != sizeofColorRegister) return (IFFERR_WRITE);
		}
    }
  
  	else (error = CLIENT_ERROR);

  	D (bug ("Wrote registers, error = %ld\n", error));

  	error = PopChunk (iff);
  	return (error);
}

/* openifile
 *
 * Passed a ParseInfo structure with a not-in-use IFFHandle, filename
 *   ("-c" or -cUnit like "-c1" for clipboard), and IFF open mode
 *   (IFFF_READ or IFFF_WRITE) opens file or clipboard for use with
 *   iffparse.library support modules.
 *
 * Returns 0 for success or an IFFERR (libraries/iffparse.h)
 */
int32 openifile (struct ParseInfo * pi, uint8 * filename, uint32 iffopenmode)
{
  struct IFFHandle *iff = NULL;
  int16 cboard;
  uint32 unit = PRIMARY_CLIP;
  int32 error;
  uint8 *omodes[2] = { "r", "w" };

  	if (!pi) return (CLIENT_ERROR);
  	if (!(iff = pi->iff)) return (CLIENT_ERROR);

  	cboard = (*filename == '-' && filename[1] == 'c');
  	if (cboard && filename[2]) unit = atoi (&filename[2]);

  	if (cboard)
    {
      	/*
      	 * Set up IFFHandle for Clipboard I/O.
      	 */
      	pi->clipboard = TRUE;
      	if (!(iff->iff_Stream = (uint32) OpenClipboard (unit)))
		{
	  		message (SI (MSG_IFFP_NOCLIP_D), unit);
	  		return (NOFILE);
		}
      	
		InitIFFasClip (iff);
    }
  	
	else
    {
      	pi->clipboard = FALSE;
      	/*
      	 * Set up IFFHandle for buffered stdio I/O.
      	 */
      	if (!(iff->iff_Stream = (uint32) fopen (filename, omodes[iffopenmode & 1])))
		{
	  		message (SI (MSG_IFFP_NOFILE_S), filename);
	  		return (NOFILE);
		}
      	
		else initiffasstdio (iff);
    }

  	D (bug ("%s file opened: \n", cboard ? "[Clipboard]" : (char *) filename));
  	pi->filename = filename;
  	error = OpenIFF (iff, iffopenmode);
  	pi->opened = error ? FALSE : TRUE; /* currently open handle */
  	D (bug ("OpenIFF error = %ld\n", error));
  	return (error);
}

/* closeifile
 *
 * closes file or clipboard opened with openifile, and frees all
 *   iffparse context parsed by parseifile.
 *
 * Note - You should closeifile as soon as possible if using clipboard
 *   ("-c[n]").  You also need to closeifile if, for example, you wish to
 *   reopen the file to write changes back out.  See the copychunks.c
 *   module for routines which allow you clone the chunks iffparse has
 *   gathered so that you can closeifile and still be able to modify and
 *   write back out gathered chunks.
 *   
 */
void closeifile (struct ParseInfo *pi)
{
  struct IFFHandle *iff = NULL;

  	D (bug ("closeifile:\n"));

  	if (!pi) return;
  	if (!(iff = pi->iff)) return;

  	DD (bug ("closeifile: About to CloseIFF if open, iff=$%lx, opened=%ld\n", iff, pi->opened));

  	if (pi->opened) CloseIFF (iff);

  	DD (bug ("closeifile: About to close %s, stream=$%lx\n", pi->clipboard ? "clipboard" : "file", iff->iff_Stream));
  	
	if (iff->iff_Stream)
    {
      	if (pi->clipboard) CloseClipboard ((struct ClipboardHandle *) (iff->iff_Stream));
      	else fclose ((FILE *) (iff->iff_Stream));
    }

  	iff->iff_Stream = NULL;
  	pi->clipboard = NULL;
  	pi->opened = NULL;
}

/* parseifile
 *
 * Passed a ParseInfo with an initialized and open IFFHandle,
 *  grouptype (like ID_FORM), groupid (like ID_ILBM),
 *  and TAG_DONE terminated longword arrays of type,id
 *  for chunks to be grabbed, gathered, and stopped on
 *  (like { ID_ILBM, ID_BMHD, ID_ILBM, ID_CAMG, TAG_DONE })
 *  will parse an IFF file, grabbing/gathering and stopping
 *  on specified chunk.
 *
 * Note - you can call getcontext() (to continue after a stop chunk) or
 *  nextcontext() (after IFFERR_EOC, to parse next form in the same file)
 *  if you wish to continue parsing the same IFF file.  If parseifile()
 *  has to delve into a complex format to find your desired FORM, the
 *  pi->hunt flag will be set.  This should be a signal to you that
 *  you may not have the capability to simply modify and rewrite
 *  the data you have gathered.
 *
 * Returns 0 for success else and IFFERR (libraries/iffparse.h)
 */
int32 parseifile (struct ParseInfo *pi, int32 groupid, int32 grouptype, int32 * propchks, int32 * collectchks, int32 * stopchks)
{
  struct IFFHandle *iff= NULL;
  register struct ContextNode *cn = NULL;
  int32 error = 0L;

  	D (bug ("parseifile:\n"));

  	if (!(iff = pi->iff)) return (CLIENT_ERROR);

  	if (!iff->iff_Stream) return (IFFERR_READ);
  	pi->hunt = FALSE;

  	/*
  	 * Declare property, collection and stop chunks.
  	 */
  	if (propchks) 
		if (error = PropChunks (iff, propchks, chkcnt (propchks))) return (error);
  
  	if (collectchks)
    	if (error = CollectionChunks (iff, collectchks, chkcnt (collectchks))) return (error);
  
  	if (stopchks)
    	if (error = StopChunks (iff, stopchks, chkcnt (stopchks))) return (error);

  	/*
  	 * We want to stop at the end of an ILBM context.
  	 */
  	if (grouptype)
    	if (error = StopOnExit (iff, grouptype, groupid)) return (error);

  	/*
  	 * Take first parse step to enter main chunk.
  	 */
  	if (error = ParseIFF (iff, IFFPARSE_STEP)) return (error);

  	/*
  	 * Test the chunk info to see if simple form of type we want (ILBM).
  	 */
  	if (!(cn = CurrentChunk (iff)))
    {
      	/*
      	 * This really should never happen.  If it does, it means
      	 * our parser is broken.
      	 */
      	message (SI (MSG_IFFP_NOTOP));
      	return (NOFILE);
    }

  	if (cn->cn_ID != groupid || cn->cn_Type != grouptype)
    {
      	D (bug ("This is a(n) %.4s.%.4s.  Looking for embedded %.4s's...\n", &cn->cn_Type, &cn->cn_ID, &grouptype));
      	pi->hunt = TRUE; /* Warning - this is a complex file */
    }

  	if (!error) error = getcontext (iff);
  	return (error);
}

/* chkcnt
 *
 * simply counts the number of chunk pairs (type,id) in array
 */
int32 chkcnt (int32 * taggedarray)
{
  int32 k = 0L;

  	while (taggedarray[k] != TAG_DONE) k++;
  	return (k >> 1);
}

/* currentchunkis
 *
 * returns the ID of the current chunk (like ID_CAMG)
 */
int32 currentchunkis (struct IFFHandle * iff, int32 type, int32 id)
{
  register struct ContextNode *cn = NULL;
  int32 result = 0L;

  	if (cn = CurrentChunk (iff))
    {
      	if ((cn->cn_Type == type) && (cn->cn_ID == id))	result = 1L;
    }
  
  	return (result);
}

/* contextis
 *
 * returns the enclosing context of the current chunk (like ID_ILBM)
 */
int32 contextis (struct IFFHandle * iff, int32 type, int32 id)
{
  register struct ContextNode *cn = NULL;
  int32 result = 0L;

  	if (cn = (CurrentChunk (iff)))
    {
      	if (cn = (ParentChunk (cn)))
		{
	  		if ((cn->cn_Type == type) && (cn->cn_ID == id)) result = 1L;
		}
    }

  	D (bug ("This is a %.4s %.4s\n", &cn->cn_Type, &cn->cn_ID));
  	return (result);
}

/* getcontext()
 *
 * Continues to gather the context which was specified to parseifile(),
 *  stopping at specified stop chunk, or end of context, or EOF
 *
 * Returns 0 (stopped on a stop chunk)
 *      or IFFERR_EOC (end of context, not an error)
 *      or IFFER_EOF (end of file)
 */
int32 getcontext (struct IFFHandle * iff)
{
  int32 error = 0L;

  	/* Based on our parse initialization,
   	* ParseIFF() will return on a stop chunk (error = 0)
   	* or end of context for an ILBM FORM (error = IFFERR_EOC)
   	* or end of file (error = IFFERR_EOF)
   	*/
  
  	return (error = ParseIFF (iff, IFFPARSE_SCAN));
}

/* nextcontext
 *
 * If you have finished parsing and reading your context (IFFERR_EOC),
 *   nextcontext will enter the next context contained in the file
 *   and parse it.
 *
 * Returns 0 or an IFFERR such as IFFERR_EOF (end of file)
 */
int32 nextcontext (struct IFFHandle * iff)
{
  int32 error = 0L;

  	error = ParseIFF (iff, IFFPARSE_STEP);
  	D (bug ("nextcontext: Got through next step\n"));

  	return (error);
}


/*
 * File I/O hook functions which the IFF library will call.
 * A return of 0 indicates success (no error).
 *
 * Iffparse.library calls this code via struct Hook and Hook.asm
 */
static int32 stdio_stream (struct Hook *hook, struct IFFHandle *iff, struct IFFStreamCmd *actionpkt)
{
  register FILE *stream = NULL;
  register int32 nbytes;
  register int actual;
  register uint8 *buf;
  int32 len;

  	stream = (FILE *) iff->iff_Stream;
  	if (!stream) return (1L);

  	nbytes = actionpkt->sc_NBytes;
  	buf = (uint8 *) actionpkt->sc_Buf;

  	switch (actionpkt->sc_Command)
    {
    	case IFFCMD_READ:
      		do
			{
	  			actual = nbytes > 32767 ? 32767 : nbytes;
	  			if ((len = fread (buf, 1, actual, stream)) != actual) break;
	  			nbytes -= actual;
	  			buf += actual;
			}
      		while (nbytes > 0);
      
	  		return (nbytes ? IFFERR_READ : 0L);

    	case IFFCMD_WRITE:
      		do
			{
	  			actual = nbytes > 32767 ? 32767 : nbytes;
	  			if ((len = fwrite (buf, 1, actual, stream)) != actual) break;
	  			nbytes -= actual;
	  			buf += actual;
			}
      		while (nbytes > 0);
      
	  		return (nbytes ? IFFERR_WRITE : 0);

    	case IFFCMD_SEEK:
      		return ((fseek (stream, nbytes, 1) == -1) ? IFFERR_SEEK : 0L);

    	default:
      		/*  No _INIT or _CLEANUP required.  */
      		return (0L);
    }
}

/* initiffasstdio (ie. init iff as stdio)
 *
 * sets up hook callback for the file stream handler above
 */
void initiffasstdio (struct IFFHandle *iff)
{

  static struct Hook stdiohook = { {NULL}, (uint32 (*)())stdio_stream, (uint32 (*)())NULL, NULL };

  	/*
  	 * Initialize the IFF structure to point to the buffered I/O
  	 * routines.  Unbuffered I/O is terribly slow.
  	 */
  	InitIFF (iff, IFFF_FSEEK | IFFF_RSEEK, &stdiohook);
}

/*
 * PutCk
 *
 * Writes one chunk of data to an iffhandle
 *
 */
int32 PutCk (struct IFFHandle *iff, int32 id, int32 size, void *data)
{
  int32 error = 0, wlen;

  	D (bug ("PutCk: asked to push chunk \"%.4s\" ($%lx) length %ld\n", &id, id, size));

  	if (error = PushChunk (iff, 0, id, size))
    {
      	D (bug ("PutCk: PushChunk of %.4s, error = %s, size = %ld\n", id, IFFerr (error), id));
    }
  	
	else
    {
     	D (bug ("PutCk: PushChunk of %.4s, error = %ld\n", &id, error));

      	/* Write the actual data */
      	if ((wlen = WriteChunkBytes (iff, data, size)) != size)
		{
	  		D (bug ("WriteChunkBytes error: size = %ld, wrote %ld\n", size, wlen));
	  		error = IFFERR_WRITE;
		}
      
	  	else error = PopChunk (iff);
      
	  	D (bug ("PutCk: After PopChunk - error = %ld\n", error));
    }
  
  	return (error);
}
