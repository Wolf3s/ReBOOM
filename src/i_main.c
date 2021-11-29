// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: i_main.c,v 1.9 1998/09/07 20:10:02 jim Exp $
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
//      Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

#ifdef UNIX
#include "SDL2/SDL.h" //Gibbon
#else
#include "SDL.h"
#endif

#include "z_zone.h"
#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "i_system.h"

void I_Quit(void);

// haleyjd: SDL init flags
#define BASE_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)

#ifdef _DEBUG
#define INIT_FLAGS (BASE_INIT_FLAGS | SDL_INIT_NOPARACHUTE)
#else
#define INIT_FLAGS BASE_INIT_FLAGS
#endif

int main(int argc, char **argv)
{
  myargc = argc;
  myargv = argv;

   // haleyjd: init SDL
   if(SDL_Init(INIT_FLAGS) == -1)
   {
      puts("Failed to initialize SDL library.\n");
      return -1;
   }

  /*
     killough 1/98:

     This fixes some problems with exit handling
     during abnormal situations.

     The old code called I_Quit() to end program,
     while now I_Quit() is installed as an exit
     handler and exit() is called to exit, either
     normally or abnormally. Seg faults are caught
     and the error handler is used, to prevent
     being left in graphics mode or having very
     loud SFX noise because the sound card is
     left in an unstable state.
  */

   atexit(I_Quit);
   
   // 2/2/98 Stan
   // Must call this here.  It's required by both netgames and i_video.c.
   
   D_DoomMain();
   
   return 0;
}
