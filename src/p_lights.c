// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: p_lights.c,v 1.11 1998/05/18 09:04:41 jim Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
//
// DESCRIPTION:
//  Action routines for lighting thinkers
//  Spawn sector based lighting effects.
//  Handle lighting linedef types
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: p_lights.c,v 1.11 1998/05/18 09:04:41 jim Exp $";

#include "doomstat.h" //jff 5/18/98
#include "doomdef.h"
#include "m_random.h"
#include "r_main.h"
#include "p_spec.h"
#include "p_tick.h"

//////////////////////////////////////////////////////////
//
// Lighting action routines, called once per tick
//
//////////////////////////////////////////////////////////

//
// T_FireFlicker()
//
// Firelight flicker action routine, called once per tick
//
// Passed a fireflicker_t structure containing light levels and timing
// Returns nothing
//
void T_FireFlicker (fireflicker_t* flick)
{
  int amount;
  
  if (--flick->count)
    return;

  amount = (P_Random(pr_lights)&3)*16;

  if (flick->sector->lightlevel - amount < flick->minlight)
    flick->sector->lightlevel = flick->minlight;
  else
    flick->sector->lightlevel = flick->maxlight - amount;

  flick->count = 4;
}

//
// T_LightFlash()
//
// Broken light flashing action routine, called once per tick
//
// Passed a lightflash_t structure containing light levels and timing
// Returns nothing
//
void T_LightFlash (lightflash_t* flash)
{
  if (--flash->count)
    return;

  if (flash->sector->lightlevel == flash->maxlight)
  {
    flash-> sector->lightlevel = flash->minlight;
    flash->count = (P_Random(pr_lights)&flash->mintime)+1;
  }
  else
  {
    flash-> sector->lightlevel = flash->maxlight;
    flash->count = (P_Random(pr_lights)&flash->maxtime)+1;
  }

}

//
// T_StrobeFlash()
//
// Strobe light flashing action routine, called once per tick
//
// Passed a strobe_t structure containing light levels and timing
// Returns nothing
//
void T_StrobeFlash (strobe_t*   flash)
{
  if (--flash->count)
    return;

  if (flash->sector->lightlevel == flash->minlight)
  {
    flash-> sector->lightlevel = flash->maxlight;
    flash->count = flash->brighttime;
  }
  else
  {
    flash-> sector->lightlevel = flash->minlight;
    flash->count =flash->darktime;
  }
}

//
// T_Glow()
//
// Glowing light action routine, called once per tick
//
// Passed a glow_t structure containing light levels and timing
// Returns nothing
//

void T_Glow(glow_t* g)
{
  switch(g->direction)
  {
    case -1:
      // light dims
      g->sector->lightlevel -= GLOWSPEED;
      if (g->sector->lightlevel <= g->minlight)
      {
        g->sector->lightlevel += GLOWSPEED;
        g->direction = 1;
      }
      break;

    case 1:
      // light brightens
      g->sector->lightlevel += GLOWSPEED;
      if (g->sector->lightlevel >= g->maxlight)
      {
        g->sector->lightlevel -= GLOWSPEED;
        g->direction = -1;
      }
      break;
  }
}

//////////////////////////////////////////////////////////
//
// Sector lighting type spawners
//
// After the map has been loaded, each sector is scanned
// for specials that spawn thinkers
//
//////////////////////////////////////////////////////////

//
// P_SpawnFireFlicker()
//
// Spawns a fire flicker lighting thinker
//
// Passed the sector that spawned the thinker
// Returns nothing
//
void P_SpawnFireFlicker (sector_t*  sector)
{
  fireflicker_t*  flick;

  // Note that we are resetting sector attributes.
  // Nothing special about it during gameplay.
  sector->special &= ~31; //jff 3/14/98 clear non-generalized sector type

  flick = Z_Malloc ( sizeof(*flick), PU_LEVSPEC, 0);

  P_AddThinker (&flick->thinker);

  flick->thinker.function.acp1 = (actionf_p1) T_FireFlicker;
  flick->sector = sector;
  flick->maxlight = sector->lightlevel;
  flick->minlight = P_FindMinSurroundingLight(sector,sector->lightlevel)+16;
  flick->count = 4;
}

//
// P_SpawnLightFlash()
//
// Spawns a broken light flash lighting thinker
//
// Passed the sector that spawned the thinker
// Returns nothing
//
void P_SpawnLightFlash (sector_t* sector)
{
  lightflash_t* flash;

  // nothing special about it during gameplay
  sector->special &= ~31; //jff 3/14/98 clear non-generalized sector type

  flash = Z_Malloc ( sizeof(*flash), PU_LEVSPEC, 0);

  P_AddThinker (&flash->thinker);

  flash->thinker.function.acp1 = (actionf_p1) T_LightFlash;
  flash->sector = sector;
  flash->maxlight = sector->lightlevel;

  flash->minlight = P_FindMinSurroundingLight(sector,sector->lightlevel);
  flash->maxtime = 64;
  flash->mintime = 7;
  flash->count = (P_Random(pr_lights)&flash->maxtime)+1;
}

//
// P_SpawnStrobeFlash
//
// Spawns a blinking light thinker
//
// Passed the sector that spawned the thinker, speed of blinking
// and whether blinking is to by syncrhonous with other sectors
//
// Returns nothing
//
void P_SpawnStrobeFlash
( sector_t* sector,
  int   fastOrSlow,
  int   inSync )
{
  strobe_t* flash;

  flash = Z_Malloc ( sizeof(*flash), PU_LEVSPEC, 0);

  P_AddThinker (&flash->thinker);

  flash->sector = sector;
  flash->darktime = fastOrSlow;
  flash->brighttime = STROBEBRIGHT;
  flash->thinker.function.acp1 = (actionf_p1) T_StrobeFlash;
  flash->maxlight = sector->lightlevel;
  flash->minlight = P_FindMinSurroundingLight(sector, sector->lightlevel);
  
  if (flash->minlight == flash->maxlight)
    flash->minlight = 0;

  // nothing special about it during gameplay
  sector->special &= ~31; //jff 3/14/98 clear non-generalized sector type

  if (!inSync)
    flash->count = (P_Random(pr_lights)&7)+1;
  else
    flash->count = 1;
}

//
// P_SpawnGlowingLight()
//
// Spawns a glowing light (smooth oscillation from min to max) thinker
//
// Passed the sector that spawned the thinker
// Returns nothing
//
void P_SpawnGlowingLight(sector_t*  sector)
{
  glow_t* g;

  g = Z_Malloc( sizeof(*g), PU_LEVSPEC, 0);

  P_AddThinker(&g->thinker);

  g->sector = sector;
  g->minlight = P_FindMinSurroundingLight(sector,sector->lightlevel);
  g->maxlight = sector->lightlevel;
  g->thinker.function.acp1 = (actionf_p1) T_Glow;
  g->direction = -1;

  sector->special &= ~31; //jff 3/14/98 clear non-generalized sector type
}

//////////////////////////////////////////////////////////
//
// Linedef lighting function handlers
//
//////////////////////////////////////////////////////////

//
// EV_StartLightStrobing()
//
// Start strobing lights (usually from a trigger)
//
// Passed the line that activated the strobing
// Returns true
//
// jff 2/12/98 added int return value, fixed return
//
int EV_StartLightStrobing(line_t* line)
{
  int   secnum;
  sector_t* sec;

  secnum = -1;
  // start lights strobing in all sectors tagged same as line
  while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
  {
    sec = &sectors[secnum];
    // if already doing a lighting function, don't start a second
    if (P_SectorActive(lighting_special,sec)) //jff 2/22/98
      continue;

    P_SpawnStrobeFlash (sec,SLOWDARK, 0);
  }
  return 1;
}

//
// EV_TurnTagLightsOff()
//
// Turn line's tagged sector's lights to min adjacent neighbor level
//
// Passed the line that activated the lights being turned off
// Returns true
//
// jff 2/12/98 added int return value, fixed return
//
int EV_TurnTagLightsOff(line_t* line)
{
  int     i;
  int     j;
  int     min;
  sector_t*   sector;
  sector_t*   tsec;
  line_t*   templine;

  sector = sectors;

  // search sectors for those with same tag as activating line
  for (j = 0;j < numsectors; j++, sector++)
  {
    if (sector->tag == line->tag)
    {
      min = sector->lightlevel;
      // find min neighbor light level
      for (i = 0;i < sector->linecount; i++)
      {
        templine = sector->lines[i];
        tsec = getNextSector(templine,sector);
        if (!tsec)
          continue;
        if (tsec->lightlevel < min)
          min = tsec->lightlevel;
      }
      sector->lightlevel = min;
    }
  }
  return 1;
}

//
// EV_LightTurnOn()
//
// Turn sectors tagged to line lights on to specified or max neighbor level
//
// Passed the activating line, and a level to set the light to
// If level passed is 0, the maximum neighbor lighting is used
// Returns true
//
// jff 2/12/98 added int return value, fixed return
//
int EV_LightTurnOn
( line_t* line,
  int   bright )
{
  int   i;
  int   j;
  sector_t* sector;
  sector_t* temp;
  line_t* templine;

  sector = sectors;

  // search all sectors for ones with same tag as activating line
  for (i=0;i<numsectors;i++, sector++)
  {
    int tbright = bright; //jff 5/17/98 search for maximum PER sector
    if (sector->tag == line->tag)
    {
      // bright = 0 means to search
      // for highest light level
      // surrounding sector
      if (!bright)
      {
        for (j = 0;j < sector->linecount; j++)
        {
          templine = sector->lines[j];
          temp = getNextSector(templine,sector);

          if (!temp)
            continue;

          if (temp->lightlevel > tbright)
            tbright = temp->lightlevel;
        }
      }
      sector-> lightlevel = tbright;
      //jff 5/17/98 unless compatibility optioned 
      //then maximum near ANY tagged sector
      if (compatibility)
        bright = tbright;
    }
  }
  return 1;
}

//----------------------------------------------------------------------------
//
// $Log: p_lights.c,v $
// Revision 1.11  1998/05/18  09:04:41  jim
// fix compatibility decl
//
// Revision 1.10  1998/05/17  11:31:36  jim
// fixed bug in lights to max neighbor
//
// Revision 1.9  1998/05/09  18:57:50  jim
// formatted/documented p_lights
//
// Revision 1.8  1998/05/03  23:17:23  killough
// Fix #includes at the top, nothing else
//
// Revision 1.7  1998/03/15  14:40:10  jim
// added pure texture change linedefs & generalized sector types
//
// Revision 1.6  1998/02/23  23:46:56  jim
// Compatibility flagged multiple thinker support
//
// Revision 1.5  1998/02/23  00:41:51  jim
// Implemented elevators
//
// Revision 1.4  1998/02/17  06:07:11  killough
// Change RNG calling sequence
//
// Revision 1.3  1998/02/13  03:28:42  jim
// Fixed W1,G1 linedefs clearing untriggered special, cosmetic changes
//
// Revision 1.2  1998/01/26  19:24:07  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:59  rand
// Lee's Jan 19 sources
//
//
//----------------------------------------------------------------------------
