/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *      Zone Memory Allocation. Neat.
 *
 * Neat enough to be rewritten by Lee Killough...
 *
 * Must not have been real neat :)
 *
 * Made faster and more general, and added wrappers for all of Doom's
 * memory allocation functions, including malloc() and similar functions.
 * Added line and file numbers, in case of error. Added performance
 * statistics and tunables.
 *-----------------------------------------------------------------------------
 */
 
#include "z_zone.h"
#include "doomstat.h"
 
 typedef struct memblock {

#ifdef ZONEIDCHECK
  unsigned id;
#endif

  struct memblock *next,*prev;
  size_t size;
  void **user;
  unsigned char tag;

#ifdef INSTRUMENTED
  const char *file;
  int line;
#endif

} memblock_t;
