// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: i_system.c,v 1.15 1998/09/07 20:06:44 jim Exp $
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

// haleyjd
#ifdef _MSC_VER
#include <conio.h>
#include <stdarg.h>
#endif

#ifdef UNIX
#include "SDL2/SDL.h"
#else
#include "SDL.h"
#endif

#include "z_zone.h"
#include "i_system.h"
#include "i_sound.h"
#include "doomstat.h"
#include "m_misc.h"
#include "g_game.h"
#include "w_wad.h"
#include "v_video.h"
#include "m_argv.h"
#include "i_video.h" //Adam - For BOOM_WINDOW_TEXT
#include "txt_main.h"

#define ENDOOM_W 80
#define ENDOOM_H 25

ticcmd_t *I_BaseTiccmd(void)
{
  static ticcmd_t emptycmd; // killough
  return &emptycmd;
}

void I_WaitVBL(int count)
{
   // haleyjd
   SDL_Delay((count*500)/TICRATE);
}

// Most of the following has been rewritten by Lee Killough
//
// I_GetTime
//

static Uint32 basetime=0;

int I_GetTime_RealTime(void)
{
   // haleyjd
   Uint32        ticks;
   
   // milliseconds since SDL initialization
   ticks = SDL_GetTicks();
   
   return ((ticks - basetime)*TICRATE)/1000;
}

// killough 4/13/98: Make clock rate adjustable by scale factor
int realtic_clock_rate = 100;
static Long64 I_GetTime_Scale = 1<<24;
int I_GetTime_Scaled(void)
{
   // haleyjd:
   return (int)((Long64) I_GetTime_RealTime() * I_GetTime_Scale >> 24);
}

static int  I_GetTime_FastDemo(void)
{
  static int fasttic;
  return fasttic++;
}

static int I_GetTime_Error()
{
  I_Error("Error: GetTime() used before initialization");
  return 0;
}

int (*I_GetTime)() = I_GetTime_Error;                           // killough

int mousepresent;
int joystickpresent;                                         // phares 4/3/98

static int orig_key_shifts;  // killough 3/6/98: original keyboard shift state
extern int autorun;          // Autorun state
int leds_always_off;         // Tells it not to update LEDs

// haleyjd: SDL joystick support

// current device number -- saved in config file
int i_SDLJoystickNum = -1;
 
// pointer to current joystick device information
SDL_Joystick *sdlJoystick = NULL;

static SDL_Keymod oldmod; // haleyjd: save old modifier key state

void I_Shutdown(void)
{
   SDL_SetModState(oldmod);
   
   // haleyjd 04/15/02: shutdown joystick
   if(joystickpresent && sdlJoystick && i_SDLJoystickNum >= 0)
   {
      if(SDL_JoystickGetAttached(sdlJoystick))
         SDL_JoystickClose(sdlJoystick);
      
      joystickpresent = false;
   }

   SDL_Quit();
}

/* set_leds:
 *  Overrides the state of the keyboard LED indicators.
 *  Set to -1 to return to default behavior.
 */
void set_leds(int leds)
{
    //Gibbon - stub, for historical purposes
}

void I_ResetLEDs(void)
{
    // Either keep the keyboard LEDs off all the time, or update them
    // right now, and in the future, with respect to key_shifts flag.
    //
    // killough 10/98: moved to here

    set_leds(leds_always_off ? 0 : -1);
}

//
// I_SetJoystickDevice
//
// haleyjd
//
boolean I_SetJoystickDevice(int deviceNum)
{
   if(deviceNum >= SDL_NumJoysticks())
      return false;
   else
   {
      sdlJoystick = SDL_JoystickOpen(deviceNum);

      if(!sdlJoystick)
	 return false;

      // check that the device has at least 2 axes and
      // 4 buttons
      if(SDL_JoystickNumAxes(sdlJoystick) < 2 ||
	 SDL_JoystickNumButtons(sdlJoystick) < 4)
	 return false;

      return true;
   }
}

// haleyjd
void I_InitKeyboard(void)
{
   SDL_Keymod   mod;
      
   oldmod = SDL_GetModState();
   switch(key_autorun)
   {
   case KEYD_CAPSLOCK:
      mod = KMOD_CAPS;
      break;
   case KEYD_NUMLOCK:
      mod = KMOD_NUM;
      break;
   case KEYD_SCROLLLOCK:
      mod = KMOD_MODE;
      break;
   default:
      mod = KMOD_NONE;
   }
   
   if(autorun)
      SDL_SetModState(mod);
   else
      SDL_SetModState(KMOD_NONE);
}


void I_Init(void)
{
   extern int key_autorun;
   int clock_rate = realtic_clock_rate, p;
   
   if((p = M_CheckParm("-speed")) && p < myargc-1 &&
      (p = atoll(myargv[p+1])) >= 10 && p <= 1000)
      clock_rate = p;
   
   // init timer
   basetime = SDL_GetTicks();

   // killough 4/14/98: Adjustable speedup based on realtic_clock_rate
   if(fastdemo)
      I_GetTime = I_GetTime_FastDemo;
   else
      if(clock_rate != 100)
      {
         I_GetTime_Scale = ((Long64) clock_rate << 24) / 100;
         I_GetTime = I_GetTime_Scaled;
      }
      else
         I_GetTime = I_GetTime_RealTime;

   // haleyjd
   if(i_SDLJoystickNum != -1)
   {
      joystickpresent = I_SetJoystickDevice(i_SDLJoystickNum);
   }
   else
   {
      joystickpresent = false;
   }

  // killough 3/6/98: save keyboard state, initialize shift state and LEDs:

  // killough 3/6/98: end of keyboard / autorun state changes

   atexit(I_Shutdown);
   
   // killough 2/21/98: avoid sound initialization if no sound & no music
   { 
      extern boolean nomusicparm, nosfxparm;
      if(!(nomusicparm && nosfxparm))
	 I_InitSound();
   }
}

int waitAtExit;

//
// I_Quit
//

static char errmsg[2048];    // buffer of error message -- killough

static int has_exited;

void I_Quit (void)
{
   has_exited=1;   /* Prevent infinitely recursive exits -- killough */
   
   if (*errmsg)
      puts(errmsg);   // killough 8/8/98
   else
      //I_EndDoom(endoom);

   if (demorecording)
      G_CheckDemoStatus();
   M_SaveDefaults();
}

//
// I_Error
//

void I_Error(const char *error, ...) // killough 3/20/98: add const
{
   if(!*errmsg)   // ignore all but the first message -- killough
   {
      va_list argptr;
      va_start(argptr,error);
      vsprintf(errmsg,error,argptr);
      va_end(argptr);
   }
   
   if(!has_exited)    // If it hasn't exited yet, exit now -- killough
   {
      has_exited=1;   // Prevent infinitely recursive exits -- killough
      exit(-1);
   }
}

// Adam - Adding back ENDBOOM lump
// Adam - Created by using the SDL2 textscreen code from Chocolate Doom (Thanks Fraggle!)

// 
// Displays the text mode ending screen after the game quits
//

void I_EndDoom(byte *endoom)
{
    unsigned char *screendata;
    int y;
    int indent;

    I_Quit();

    // Set up text mode screen

    TXT_Init();

    TXT_SetWindowTitle(BOOM_WINDOW_TEXT);

    // Write the data to the screen memory

    screendata = TXT_GetScreenData();

    indent = (ENDOOM_W - TXT_SCREEN_W) / 2;

    for (y=0; y<TXT_SCREEN_H; ++y)
    {
        memcpy(screendata + (y * TXT_SCREEN_W * 2),
               endoom + (y * ENDOOM_W + indent) * 2,
               TXT_SCREEN_W * 2);
    }

    // Wait for a keypress

    while (true)
    {
        TXT_UpdateScreen();

        if (TXT_GetChar() > 0)
        {
            break;
        }

        TXT_Sleep(0);
    }

    // Shut down text mode screen

    TXT_Shutdown();
}
