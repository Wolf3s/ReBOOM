// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: i_system.h,v 1.7 1998/05/03 22:33:43 killough Exp $
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
//      System specific interface stuff.
//
//-----------------------------------------------------------------------------

#ifndef __I_SYSTEM__
#define __I_SYSTEM__

#ifndef __GNUC__
#define __attribute__(x)
#endif

#include "d_ticcmd.h"

// Called by DoomMain.
void I_Init(void);

// Called by D_DoomLoop,
// returns current time in tics.

extern int (*I_GetTime)();           // killough
int I_GetTime_RealTime();     // killough
int I_GetTime_Adaptive(void); // killough 4/10/98
extern int GetTime_Scale;

//
// Called by D_DoomLoop,
// called before processing any tics in a frame
// (just after displaying a frame).
// Time consuming syncronous operations
// are performed here (joystick reading).
// Can call D_PostEvent.
//

void I_StartFrame (void);

//
// Called by D_DoomLoop,
// called before processing each tic in a frame.
// Quick syncronous operations are performed here.
// Can call D_PostEvent.

void I_StartTic (void);

// Asynchronous interrupt functions should maintain private queues
// that are read by the synchronous functions
// to be converted into events.

// Either returns a null ticcmd,
// or calls a loadable driver to build it.
// This ticcmd will then be modified by the gameloop
// for normal input.

ticcmd_t* I_BaseTiccmd (void);

// atexit handler -- killough

void I_Quit (void);

// Allocates from low memory under dos, just mallocs under unix

#define I_AllocLow(length) calloc((length),1)            /* killough */
#define I_Tactile(on, off, total)

// killough 3/20/98: add const
// killough 4/25/98: add gcc attributes
void I_Error(const char *error, ...) __attribute__((format(printf,1,2)));

extern int mousepresent;                // killough

void I_EndDoom(byte *data);

// killough 3/21/98: keyboard queue

#define KQSIZE 256

extern struct keyboard_queue_s {
  volatile int head,tail,queue[KQSIZE];
} keyboard_queue;

#endif
