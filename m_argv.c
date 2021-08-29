// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: m_argv.c,v 1.5 1998/05/03 22:51:40 killough Exp $
//
//  BOOM, a modified and improved DOOM engine
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//  02111-1307, USA.
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "m_argv.h"

int  myargc;
const char** myargv;

//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
//

// Adam - cleaned up and made C++ compliant (still compiled as C though)

int M_CheckParm(const char* check)
{
	for (signed i = 1; i < myargc; ++i) // fixed a signed/unsigned comparison mismatch, for args signed is fine
	{
#ifdef WINDOWS
		if (NULL == _stricmp(check, myargv[i]))
#else
		if (NULL == stricmp(check, myargv[i]))
#endif
		{
			return i;
		}
	}
	return 0;
}
