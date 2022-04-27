/*
 *  FlashMandel - FM_ReactionBasics.c
 *
 *  Copyright (C) 2002 - 2020  Edgar Schwan
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
 *    $Id: FM_ReactionBasics.c,v 1.3 2005/02/26 00:00:00 eschwan Exp $
 *
 *    Code for palette-prefs with reaction-support
 *
 *    $Log: FM_ReactionBasics.c,v $
 *    Revision 1.5  2020/12/12 00:00:00  eschwan
 *    Modified for "no reactor" code
 *
 *    Revision 1.4  2020/03/19 00:00:00  dpapararo
 *    Various fixes.
 *
 *    Revision 1.3  2005/02/26 00:00:00  eschwan
 *    MANDChunk is now a pointer.
 *
 *    Revision 1.2  2004/07/01 20:56:20  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/03/01 17:24:28  eschwan
 *    First tracked version
 *
 *    Revision 2.2  2002/02/07 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifdef __amigaos4__
#define __USE_INLINE__
#define __USE_BASETYPE__
#endif /* __amigaos4__ */

#include <libraries/gadtools.h>
#include <classes/window.h>

#define NO_PROTOS
#include <iffp/ilbmapp.h>
#undef NO_PROTOS

#ifdef __GNUC__
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <clib/alib_protos.h>
#else /* __GNUC__ */
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>
#include <clib/locale_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/alib_protos.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/datatypes_pragmas.h>
#endif /* __GNUC__ */

#include "FM_ReactionBasics.h"
#include "FM_Reaction.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include "FM_ARexx_React.h"
#include "headers/FlashMandel.h"

extern struct Library *LocaleBase;
extern struct Catalog *CatalogPtr;
extern uint32 RETURNVALUE;
extern struct TextAttr MYFONTSTRUCT;
extern struct MandelChunk *MANDChunk;
#ifdef FM_AREXX_SUPPORT
extern Object *SoundObject;
#endif /* FM_AREXX_SUPPORT */

#ifdef FM_REACT_SUPPORT
struct MsgPort *IDCMPortPtr = NULL;
struct MsgPort *AppPortPtr = NULL;
struct Screen *LockedScreen = NULL;
#endif /* FM_REACT_SUPPORT */

/* InitReaction() */
int16 InitReaction (STRPTR ScrPubName)
{
#ifdef FM_REACT_SUPPORT

  	if ( /*ResourceBase = OpenLibrary("resource.library", 0L) */ 1)
    {
      	if (LockedScreen = LockPubScreen ((STRPTR) ScrPubName))
		{
	  		if (IDCMPortPtr = CreateMsgPort ())
	    	{
	      		if (AppPortPtr = CreateMsgPort ())
				{
#ifdef FM_AREXX_SUPPORT
		  			InitARexx ();
#endif /* FM_AREXX_SUPPORT */
		  			return (TRUE);
				}
	    	}
		}
    }

  	FreeReaction ();
#else /* FM_REACT_SUPPORT */
#ifdef FM_AREXX_SUPPORT
  	InitARexx ();
	// PutStr ("Called InitARexx()\n"); //DEBUG
#endif /* FM_AREXX_SUPPORT */
#endif /* FM_REACT_SUPPORT */

  	return (FALSE);
}

/* FreeReaction() */
void FreeReaction (void)
{
#ifdef FM_REACT_SUPPORT
#ifdef FM_AREXX_SUPPORT

  	FreeARexx ();

  	if (SoundObject)
    {
      	DisposeDTObject (SoundObject);
      	SoundObject = NULL;
    }
#endif /*FM_AREXX_SUPPORT */

  	if (AppPortPtr)
    {
      	DeleteMsgPort (AppPortPtr);
      	AppPortPtr = NULL;
    }
  
  	if (IDCMPortPtr)
    {
      	DeleteMsgPort (IDCMPortPtr);
      	IDCMPortPtr = NULL;
    }
  
  	if (LockedScreen)
    {
      	UnlockPubScreen (NULL, LockedScreen);
      	LockedScreen = NULL;
    }
#else /* FM_REACT_SUPPORT */
#ifdef FM_AREXX_SUPPORT
  	FreeARexx ();

  	if (SoundObject)
    {
      	DisposeDTObject (SoundObject);
      	SoundObject = NULL;
    }
#endif /*FM_AREXX_SUPPORT */
#endif /* FM_REACT_SUPPORT */
}

/* OpenReactionWindow() */
struct ReactWinData *OpenReactionWindow (RESOURCEID WinResID, RESOURCEID GadResID, char *ScrPubName)
{
#ifdef FM_REACT_SUPPORT
  struct ReactWinData *rwd = NULL;

  	CloseReactionWindow (rwd);
#endif /* FM_REACT_SUPPORT */
  	return (NULL);
}

/* CloseReactionWindow */
void CloseReactionWindow (struct ReactWinData *rwd)
{
#ifdef FM_REACT_SUPPORT
  	if (rwd != NULL)
    {
      	if (rwd->RWD_WindowObject)
		{
	  		DoMethod (rwd->RWD_WindowObject, WM_CLOSE);
		}
      
	  	if (rwd) FreeVec (rwd);
    }
#endif /* FM_REACT_SUPPORT */
}


