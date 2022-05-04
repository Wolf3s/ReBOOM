// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
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
//      Put all global state variables here.
//
//-----------------------------------------------------------------------------

//static const char rcsid[] = "$Id: doomstat.c,v 1.5 1998/05/12 12:46:12 phares Exp $";

#include "doomstat.h"

// Game Mode - identify IWAD as shareware, retail etc.
GameMode_t gamemode = indetermined;
GameMission_t   gamemission = doom;

// Language.
Language_t   language = english;

// Set if homebrew PWAD stuff has been added.
boolean modifiedgame;

//-----------------------------------------------------------------------------

// More Gibs - With or Without Hyper Shotgun
int more_gibs;

// Hyper Berserk Shotgun
int hyper_berserk_shotgun;

// Accessibility colours
int accessibility_colours;

// Accessibility effects
int accessibility_effects;

// No horizontal autoaim
int disable_horizontal_autoaim;

// compatibility with old engines (monster behavior, metrics, etc.)
int compatibility, default_compatibility;          // killough 1/31/98

int demo_version;           // killough 7/19/98: Boom version of demo

// v1.1-like pitched sounds
int pitched_sounds;  // killough 10/98

int general_translucency;    // killough 10/98

int demo_insurance, default_demo_insurance;        // killough 1/16/98

int  allow_pushers = 1;      // MT_PUSH Things              // phares 3/10/98
int  default_allow_pushers;  // killough 3/1/98: make local to each game

int  variable_friction = 1;      // ice & mud               // phares 3/10/98
int  default_variable_friction;  // killough 3/1/98: make local to each game

int  weapon_recoil;              // weapon recoil                   // phares
int  default_weapon_recoil;      // killough 3/1/98: make local to each game

int player_bobbing;  // whether player bobs or not          // phares 2/25/98
int default_player_bobbing;  // killough 3/1/98: make local to each game

int monsters_remember;          // killough 3/1/98
int default_monsters_remember;

char *MAPNAME(int e, int m)
{
  static char name[9];

  if (gamemode == commercial)
    snprintf(name, sizeof(name), "MAP%02d", m);
  else
    snprintf(name, sizeof(name), "E%dM%d", e, m);

  return name;
}
