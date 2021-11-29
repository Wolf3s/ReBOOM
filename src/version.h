//-----------------------------------------------------------------------------
//
// $Id: version.h,v 1.3 1998/04/20 13:29:58 jim Exp $
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
//-----------------------------------------------------------------------------

#ifndef __VERSION_H__
#define __VERSION_H__

#include "z_zone.h"  /* memory allocation wrappers -- killough */

// DOOM version
enum { VERSION =  202 };

extern const char version_date[];

/** Lots of different version numbers **/

#define DOTVERSIONSTR_NOREV "2.0.6um"

// The version string the user actually sees.
#define DOTVERSIONSTR DOTVERSIONSTR_NOREV " (r"")"

// The version as seen in the Windows resource
#define RC_FILEVERSION 2,0,6
#define RC_PRODUCTVERSION 2,0,6,0
#define RC_FILEVERSION2 DOTVERSIONSTR
#define RC_PRODUCTVERSION2 "2.6um"

#ifdef UNIX
#define HOME_DIR "~/.reboom"
#endif

// The maximum length of one save game description for the menus.
#define SAVESTRINGSIZE		24

#endif //__VERSION_H__
