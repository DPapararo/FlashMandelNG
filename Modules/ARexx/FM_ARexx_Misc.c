/*
 *  FlashMandel - FM_ARexx_Misc.c
 *
 *  Copyright (C) 2002 - 2022  Edgar Schwan
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
 *    $Id: FM_ARexx_Misc.c,v 1.5 2022/01/04 00:00:00 dpapararo Exp $
 *
 *    Functions for Reaction-ARexx.
 *
 *    $Log: FM_ARexx_Misc.c,v $
 *
 *    Revision 1.5  2022/01/04 00:00:00  dpapararo
 *	  Implemented OS4 API Calls where possible
 *	  Cleaned code
 *
 *    Revision 1.4  2020/03/19 00:00:00  dpapararo
 *    various fixes
 *
 *    Revision 1.3  2018/05/03 00:00:00  eschwan
 *    Modified directory handling
 *
 *    Revision 1.2  2004/10/02 00:00:00  eschwan
 *    Modified for AmigaOS4/GCC
 *
 *    Revision 1.1  2004/02/29 20:56:20  eschwan
 *    First tracked version
 *
 *    Revision 1.1  2002/02/19 00:00:00  eschwan
 *    Last no-cvs-release.
 */

#ifdef __amigaos4__
#define __USE_INLINE__
#define __USE_BASETYPE__
#define CurrentDir SetCurrentDir
#endif /* __amigaos4__ */

#include <exec/types.h>
#include <exec/tasks.h>
#include <libraries/asl.h>
#include <libraries/locale.h>
#define NO_PROTOS
#include <iffp/ilbmapp.h>
#undef NO_PROTOS
#include <graphics/text.h>
#include <intuition/classusr.h>
#include <reaction/reaction.h>
#include <classes/arexx.h>

#ifdef __GNUC__
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/gadtools.h>
#include <proto/locale.h>
#else /* __GNUC__ */
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/asl_protos.h>
#include <clib/icon_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/locale_protos.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/locale_pragmas.h>
#endif /* __GNUC__ */

#include <stdio.h>
#include <string.h>

#include "FM_ARexx_Misc.h"
#include "headers/FlashMandel.h"
#define CatCompArray FM_CatCompArray
#define CatCompArrayType FM_CatCompArrayType
#include "FM_ReactionCD.h"
#undef CatCompArray
#undef CatCompArrayType

#include "FM_ReactionBasics.h"

#ifdef FM_AREXX_SUPPORT

/* external vars */
extern Object *AREXXOBJ;
extern struct TextAttr MYFONTSTRUCT;
extern struct Catalog *CatalogPtr;

struct NewMenu *ARexxNewMenu = NULL;
struct Menu *ARexxMenu = NULL;

/* local prototypes */
//static void LaunchIt(struct ILBMInfo *Ilbm, char *cmd, char *dir);

/* LaunchARexxScript(): Launch an arexx-script.

        SYNOPSIS: int16  = LaunchARexx
                        (
                        struct ILBMInfo  *Ilbm;
                        );

        INPUTS:     Ilbm:
                            Pointer to the ILBMInfo-structure.

        RETURNS:        result:
                            Is TRUE if successfull.
*/
int16 LaunchARexxScript (struct ILBMInfo *Ilbm)
{
  static char ARexxDir[MAX_DIRLEN] = "arexx";
  static char ARexxFilename[MAX_FILELEN] = { 0 };
  struct FileRequester *ARexxFileReq;
  int16 Success = FALSE;

  	if (AREXXOBJ)
    {
      	if (ARexxFileReq = AllocAslRequest (ASL_FileRequest, 0))
		{
	  		if (AslRequestTags (ARexxFileReq, ASLFR_Window, Ilbm->win,
			      ASLFR_InitialLeftEdge, Ilbm->win->LeftEdge + 25,
			      ASLFR_InitialTopEdge, Ilbm->win->TopEdge + 35,
			      ASLFR_InitialWidth, 300,
			      ASLFR_InitialHeight,
			      ((Ilbm->win->Height) * 7) >> 3, ASLFR_SleepWindow,
			      TRUE, ASLFR_TextAttr, &MYFONTSTRUCT,
			      ASLFR_TitleText, CATSTR (TXT_SelectRxScript),
			      ASLFR_InitialDrawer, &ARexxDir, ASLFR_InitialFile,
			      &ARexxFilename, ASLFR_DoSaveMode, FALSE,
			      ASLFR_RejectIcons, TRUE, TAG_DONE))
	    	{
	      		Strlcpy (ARexxFilename, ARexxFileReq->fr_File, sizeof (ARexxFilename));
	      		Strlcpy (ARexxDir, ARexxFileReq->fr_Drawer, sizeof (ARexxDir));

	      		LaunchIt (Ilbm, ARexxFilename, ARexxDir);
	      		Success = TRUE;
	    	}
	  		
			FreeAslRequest (ARexxFileReq);
		}
    }
  	
	return (Success);
}

/* CreateARexxMenu(): Create ARexx-menu.

        SYNOPSIS: int16  = CreateARexxMenu
                        (
                        struct DiskObject *dobj;
                        );

        INPUTS:     dobj:
                            Pointer to the DiskObject-structure or NULL.

        RETURNS:        result:
                            Is TRUE if successfull.
*/
int16 CreateARexxMenu (struct DiskObject *dobj)
{
  int16 success = FALSE;
  char ttnamebuf[50];
  const char title_txt[] = "ARexx";
  const STRPTR launch_txt = (const STRPTR) CATSTR (TXT_Launch);
  int32 numcmds = 0, i;
  size_t nm_size = 0, string_size = 0;
  struct DiskObject *cli_dobj = NULL, *act_dobj = NULL;
  struct Process *own_proc = (struct Process *) FindTask (NULL);
  struct CommandLineInterface *own_CLI = BADDR (own_proc->pr_CLI);
  STRPTR progname = NULL;

	//  printf("Called CreateARexxMenu(). AREXXOBJ: 0x%08X\n", AREXXOBJ); //DEBUG

  	if (own_CLI != ZERO)
    {
      	progname = BADDR (own_CLI->cli_CommandName) + 1;
      	//  printf("  CLI command name: \"%s\"\n", progname); //DEBUG
    }
	
  	else
    {
      	struct Node *own_node = (struct Node *) &own_proc->pr_Task.tc_Node;
      	progname = own_node->ln_Name;
      	//  printf("  Taskname (node name): \"%s\"\n", progname); //DEBUG
    }

  	if (AREXXOBJ)
    {
      	if (dobj) act_dobj = dobj;
      	else
		{
	  		if (cli_dobj = GetDiskObject (progname)) act_dobj = cli_dobj;
	  		else
	    	{
	      		//  printf("  No Diskobject\n"); //DEBUG
				goto ExitCreateArexxMenu;
	    	}
		}

		string_size = (Strlen ((const STRPTR) &title_txt) + 1) + (Strlen (launch_txt) + 1);

      	while (1)
		{
	  		STRPTR value;
	  		SNPrintf ((STRPTR) &ttnamebuf, sizeof (ttnamebuf), "REXXCMD%ld", numcmds+1);
	  		if (! (value = FindToolType (act_dobj->do_ToolTypes, (STRPTR) &ttnamebuf))) break;
	  		string_size += (Strlen (value) + 1);
			numcmds++;
		}
      	
       	nm_size = ((numcmds + 4) * sizeof (struct NewMenu));

      	if (ARexxNewMenu = (struct NewMenu *) AllocVec ((uint32) (nm_size + string_size), MEMF_PRIVATE | MEMF_CLEAR))
		{
	  		STRPTR string = (STRPTR) (((uint32) ARexxNewMenu) + nm_size), tmp;
	  		struct NewMenu *act_nm = ARexxNewMenu;

	  		// Strlcpy (string, title_txt, sizeof (title_txt));	/* initialize menu-title */
	  		strcpy (string, title_txt);	/* initialize menu-title */	
	  		act_nm->nm_Type = NM_TITLE;
	  		act_nm->nm_Label = string;
	  		act_nm->nm_CommKey = NULL;
	  		act_nm->nm_Flags = 0;
	  		act_nm->nm_MutualExclude = 0;
	  		act_nm->nm_UserData = (APTR) (REXX_MENU_ID);
	  		string = (STRPTR) ((uint32) string + Strlen (string) + 1);
	  		act_nm++;

	  		if (numcmds)
	    	{			/* initialize command-items */
	      		for (i = 1; i <= numcmds; i++)
				{
		  			SNPrintf ((STRPTR) &ttnamebuf, sizeof (ttnamebuf), "REXXCMD%ld", i);
		  			tmp = (STRPTR) FindToolType (act_dobj->do_ToolTypes,(STRPTR) &ttnamebuf);
					// Strlcpy (string, tmp, sizeof (string) * (Strlen (tmp) + 1));
					strcpy (string, tmp);

					
		  			tmp = string;
		  			string = (STRPTR) ((uint32) (string + Strlen (string) + 1));
		  			if (tmp[Strlen (tmp) - 1] == '"') tmp[Strlen (tmp) - 1] = 0;
		  			if (tmp[0] == '"')
		    		{
		      			tmp[0] = 0;
		      			tmp++;
		    		}
		  
		  			act_nm->nm_Type = NM_ITEM;
		  			act_nm->nm_Label = tmp;
		  			act_nm->nm_CommKey = NULL;
		  			act_nm->nm_Flags = 0;
		  			act_nm->nm_MutualExclude = 0;
		  			act_nm->nm_UserData = (APTR) (REXX_MENU_ID + i);

		  			act_nm++;
				}
				
	      		act_nm->nm_Type = NM_ITEM;	/* initialize barlabel */
	      		act_nm->nm_Label = NM_BARLABEL;
	      		act_nm->nm_CommKey = NULL;
	      		act_nm->nm_Flags = 0;
	      		act_nm->nm_MutualExclude = 0;
	      		act_nm->nm_UserData = NULL;
	      		act_nm++;
			}

	  		// Strlcpy (string, launch_txt, sizeof (string) * (Strlen (launch_txt) + 1));	/* initialize launch-item */
	  		strcpy (string, launch_txt);	/* initialize launch-item */		
	  		act_nm->nm_Type = NM_ITEM;
	  		act_nm->nm_Label = string;
	  		act_nm->nm_CommKey = NULL;
	  		act_nm->nm_Flags = 0;
	  		act_nm->nm_MutualExclude = 0;
		  	act_nm->nm_UserData = (APTR) REXX_MENUITEM_LAUNCH;
	  		act_nm++;
	  		act_nm->nm_Type = NM_END;	/* initialize menu-end */
	  		act_nm->nm_Label = NULL;
	  		act_nm->nm_CommKey = NULL;
	  		act_nm->nm_Flags = 0;
	  		act_nm->nm_MutualExclude = 0;
	  		act_nm->nm_UserData = NULL;
	  		success = TRUE;
		}

     	if (cli_dobj)
		{
	  		FreeDiskObject (cli_dobj);
	  		cli_dobj = NULL;
		}
    }

ExitCreateArexxMenu:	
  	return (success);
}

/* SetARexxMenu(): Activate menu-items for ARexx-menu in main-window.

        SYNOPSIS: int16  = SetARexxMenu
                        (
                        struct ILBMInfo  *Ilbm;
                        );

        INPUTS:     Ilbm:
                            Pointer to the ILBMInfo-structure.

        RETURNS:        result:
                            Is TRUE if successfull.
*/
int16 SetARexxMenu (struct ILBMInfo * Ilbm)
{
  int16 success = FALSE;
  struct Menu *win_menu = Ilbm->win->MenuStrip, *last_menu = NULL;
  APTR vi = NULL;

	// printf("Called SetARexxMenu()\n"); //DEBUG

  	if (AREXXOBJ)
    {
      	if (vi = GetVisualInfoA (Ilbm->scr, NULL))
		{
	  		if (ARexxNewMenu)
	    	{
	      		if (ARexxMenu = CreateMenusA (ARexxNewMenu, NULL))
				{
		  			last_menu = win_menu;
					
		  			while (last_menu->NextMenu)
		    		{
		      			last_menu = last_menu->NextMenu;
		    		}
		  
		  			ModifyIDCMP (Ilbm->win, NULL);
		  			ClearMenuStrip (Ilbm->win);
		  			last_menu->NextMenu = ARexxMenu;
		  			LayoutMenus (win_menu, vi, GTMN_TextAttr, &MYFONTSTRUCT,
			       				GTMN_NewLookMenus, TRUE, TAG_DONE);
		  			SetMenuStrip (Ilbm->win, win_menu);
		  			ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
		  			success = TRUE;
				}
	    	}
	  
	  		FreeVisualInfo (vi);
		}
    }
	
  	return (success);
}

/* RemoveARexxMenu(): Remove and free menu-items for ARexx-menu.

        SYNOPSIS: void  = RemoveARexxMenu
                        (
                        struct ILBMInfo  *Ilbm;
                        );

        INPUTS:     Ilbm:
                            Pointer to the ILBMInfo-structure.

        RETURNS:        result: -
*/
void RemoveARexxMenu (struct ILBMInfo *Ilbm)
{
  struct Menu *win_menu = Ilbm->win->MenuStrip, *act_menu = NULL, *prev_menu = NULL;
  APTR vi = NULL;

  	if (ARexxMenu)
    {
      	if (vi = GetVisualInfoA (Ilbm->scr, NULL))
		{
	  		act_menu = win_menu;
	  		while (act_menu)
	    	{
	      		if (act_menu == ARexxMenu)
				{
		  			ModifyIDCMP (Ilbm->win, NULL);
		  			ClearMenuStrip (Ilbm->win);
		  			if (prev_menu) prev_menu->NextMenu = ARexxMenu->NextMenu;
		  			LayoutMenus (win_menu, vi, GTMN_TextAttr, &MYFONTSTRUCT,
			       				GTMN_NewLookMenus, TRUE, TAG_DONE);
		  			SetMenuStrip (Ilbm->win, win_menu);
		  			ModifyIDCMP (Ilbm->win, IDCMP_STANDARD);
		  			FreeMenus (ARexxMenu);
		  			ARexxMenu = NULL;
		  			break;
				}
	      
		  		prev_menu = act_menu;
	      		act_menu = act_menu->NextMenu;
	    	}
	  
	  		FreeVisualInfo (vi);
		}
    }
}

/* FreeARexxMenu(): Free menu-items of arexx-menu.

        SYNOPSIS: void  = FreeARexxMenu
                        (
                        void
                        );

        INPUTS:     -

        RETURNS:        result: -
*/
void FreeARexxMenu (void)
{
  	if (ARexxNewMenu)
    {
      	FreeVec ((APTR) ARexxNewMenu);
      	ARexxNewMenu = NULL;
    }
}

/* HandleARexxMenu(): Handle all events on the arexx-menu.

        SYNOPSIS: void  = HandleARexxMenu
                        (
                        struct ILBMInfo *Ilbm;
                        uint16 MenuCode;
                        );

        INPUTS:     Ilbm:
                            Pointer to the ILBMInfo-structure.

                        MenuCode:
                            Code, that specifies the selected menu-item.

        RETURNS:        result: -
*/
void HandleARexxMenu (struct ILBMInfo *Ilbm, uint16 MenuCode)
{
  struct MenuItem *SelectedItem = ItemAddress (Ilbm->win->MenuStrip, MenuCode);
  uint32 id = (uint32) GTMENUITEM_USERDATA (SelectedItem);

	//  printf("In HandleARexxMenu()\n"); //DEBUG
	//  printf("  id: %lu\n", id); //DEBUG

  	if (AREXXOBJ)
    {
      	if (id)
		{
	  		if (id & REXX_MENU_CMD_MASK)
	    	{
	      		struct IntuiText *it = (struct IntuiText *) SelectedItem->ItemFill;
	      		char *command = it->IText;

	      		LaunchIt (Ilbm, command, "arexx");
	    	}
	  		
			else
	    	{
	      		switch (id)
				{
					case REXX_MENUITEM_LAUNCH:
		  				LaunchARexxScript (Ilbm);
		  			break;
					default:
		  				DisplayBeep (Ilbm->scr);
				}
	    	}
		}
    }
}

/* LaunchIt(): Launch an arexx-script.

        SYNOPSIS: static void  = LaunchIt
                        (
                        struct ILBMInfo *Ilbm;
                        char *cmd;
                        char *dir;
                        );

        INPUTS:     Ilbm:
                            Pointer to the ILBMInfo-structure.

                        cmd:
                            Pointer to the command-string.

                        dir:
                            Pointer to the directory-string.

        RETURNS:        result: -
*/
void LaunchIt (struct ILBMInfo *Ilbm, char *cmd, char *dir)
{
  STRPTR hostname;
  char cmd_string[300];
  BPTR OldDir, NewDir, Script;

	//  printf("Called LaunchIt(). cmd: %s, dir: %s\n", cmd, dir); //DEBUG

  	if (AREXXOBJ)
    {
      	GetAttr (AREXX_HostName, AREXXOBJ, (uint32 *) & hostname);

      	//  ((STRPTR) &cmd_string, "%s/%s %s", dir, cmd, hostname); // assign FLASHMANDEL: was removed
      	SNPrintf ((STRPTR) &cmd_string, sizeof (cmd_string), "%s %s", cmd, hostname);
      	if (NewDir = Lock (dir, SHARED_LOCK))
		{
	  		OldDir = CurrentDir (NewDir);
	  		if (Script = Lock (cmd, SHARED_LOCK))
	    	{
	      		int32 result1, result2, result;
	      		STRPTR result_str;
	      		struct apExecute ape = {AM_EXECUTE, (STRPTR) cmd_string, NULL, &result1, &result2, &result_str,	NULL};
	      		UnLock (Script);
#ifndef __amigaos4__
	      		result = DoMethodA (AREXXOBJ, (Msg) & ape);
#else /* __amigaos4__ */
	      		result = IDoMethodA (AREXXOBJ, (Msg) & ape);
#endif /* __amigaos4__ */
	    	}
			//  else DisplayBeep(Ilbm->scr);
	  		CurrentDir (OldDir);
	  		UnLock (NewDir);
		}
    }
}

#else /* FM_AREXX_SUPPORT */
	extern struct Library *ResourceBase;	/* dummy */
#endif /* FM_AREXX_SUPPORT */
