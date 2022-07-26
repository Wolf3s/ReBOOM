// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: i_video.c,v 1.12 1998/05/03 22:40:35 killough Exp $
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
//      DOOM graphics stuff
//
//-----------------------------------------------------------------------------

#ifdef UNIX
#include "SDL2/SDL.h"
#else
#include "SDL.h" // haleyjd
#endif

#include "z_zone.h"  /* memory allocation wrappers -- killough */
#include "doomstat.h"
#include "v_video.h"
#include "d_main.h"
#include "m_bbox.h"
#include "st_stuff.h"
#include "m_argv.h"
#include "w_wad.h"
#include "r_draw.h"
#include "am_map.h"
#include "m_menu.h"
#include "wi_stuff.h"
#include "i_video.h"

SDL_Surface *sdlscreen;

// [FG] rendering window, renderer, intermediate ARGB frame buffer and texture

SDL_Window *screen;
SDL_Renderer *renderer;
SDL_Surface *argbbuffer;
SDL_Texture *texture;

/////////////////////////////////////////////////////////////////////////////
//
// JOYSTICK                                                  // phares 4/3/98
//
/////////////////////////////////////////////////////////////////////////////

extern int usejoystick;
extern int joystickpresent;
extern int joy_x,joy_y;
extern int joy_b1,joy_b2,joy_b3,joy_b4;

// I_JoystickEvents() gathers joystick data and creates an event_t for
// later processing by G_Responder().

int joystickSens_x;
int joystickSens_y;

extern SDL_Joystick *sdlJoystick;

void I_JoystickEvents(void)
{
   // haleyjd 04/15/02: SDL joystick support

   event_t event;
   int joy_b1, joy_b2, joy_b3, joy_b4;
   Sint16 joy_x, joy_y;
   
   if(!joystickpresent || !usejoystick || !sdlJoystick)
      return;
   
   SDL_JoystickUpdate(); // read the current joystick settings
   event.type = ev_joystick;
   event.data1 = 0;
   
   // read the button settings
   if((joy_b1 = SDL_JoystickGetButton(sdlJoystick, 0)))
      event.data1 |= 1;
   if((joy_b2 = SDL_JoystickGetButton(sdlJoystick, 1)))
      event.data1 |= 2;
   if((joy_b3 = SDL_JoystickGetButton(sdlJoystick, 2)))
      event.data1 |= 4;
   if((joy_b4 = SDL_JoystickGetButton(sdlJoystick, 3)))
      event.data1 |= 8;
   
   // Read the x,y settings. Convert to -1 or 0 or +1.
   joy_x = SDL_JoystickGetAxis(sdlJoystick, 0);
   joy_y = SDL_JoystickGetAxis(sdlJoystick, 1);
   
   if(joy_x < -joystickSens_x)
      event.data2 = -1;
   else if(joy_x > joystickSens_x)
      event.data2 = 1;
   else
      event.data2 = 0;

   if(joy_y < -joystickSens_y)
      event.data3 = -1;
   else if(joy_y > joystickSens_y)
      event.data3 = 1;
   else
      event.data3 = 0;
   
   // post what you found
   
   D_PostEvent(&event);
}


//
// I_StartFrame
//
void I_StartFrame(void)
{
   static boolean firstframe = true;
   
   // haleyjd 02/23/04: turn mouse event processing on
   if(firstframe)
   {
      SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
      firstframe = false;
   }
   
   I_JoystickEvents(); // Obtain joystick data                 phares 4/3/98
}

/////////////////////////////////////////////////////////////////////////////
//
// END JOYSTICK                                              // phares 4/3/98
//
/////////////////////////////////////////////////////////////////////////////

// haleyjd 10/08/05: Chocolate DOOM application focus state code added

// Grab the mouse? (int type for config code)
int grabmouse = 1;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen
boolean screenvisible;
static boolean window_focused;
int fullscreen;
int page_flip;     // killough 8/15/98: enables page flipping
static int in_page_flip;

//
// MouseShouldBeGrabbed
//
// haleyjd 10/08/05: From Chocolate DOOM, fairly self-explanatory.
//
static boolean MouseShouldBeGrabbed(void)
{
   // if the window doesnt have focus, never grab it
   if(!window_focused)
      return false;
   
   // always grab the mouse when full screen (dont want to 
   // see the mouse pointer)
   if(fullscreen == 1)
      return true;
   
   // if we specify not to grab the mouse, never grab
   if(!grabmouse)
      return false;
   
   // when menu is active or game is paused, release the mouse 
   if(menuactive || paused)
      return false;
   
   // only grab mouse when playing levels (but not demos)
   return (gamestate == GS_LEVEL) && !demoplayback;
}

// [FG] mouse grabbing from Chocolate Doom 3.0

static void SetShowCursor(boolean show)
{
   // When the cursor is hidden, grab the input.
   // Relative mode implicitly hides the cursor.
   SDL_SetRelativeMouseMode(!show);
   SDL_GetRelativeMouseState(NULL, NULL);
}

// 
// UpdateGrab
//
// haleyjd 10/08/05: from Chocolate DOOM
//
static void UpdateGrab(void)
{
   static boolean currently_grabbed = false;
   boolean grab;
   
   grab = MouseShouldBeGrabbed();
   
   if(grab && !currently_grabbed)
   {
      SetShowCursor(false);
   }
   
   if(!grab && currently_grabbed)
   {
      int screen_w, screen_h;

      SetShowCursor(true);

      // When releasing the mouse from grab, warp the mouse cursor to
      // the bottom-right of the screen. This is a minimally distracting
      // place for it to appear - we may only have released the grab
      // because we're at an end of level intermission screen, for
      // example.

      SDL_GetWindowSize(screen, &screen_w, &screen_h);
      SDL_WarpMouseInWindow(screen, screen_w - 16, screen_h - 16);
      SDL_GetRelativeMouseState(NULL, NULL);
   }
   
   currently_grabbed = grab;   
}

//
// Keyboard routines
// By Lee Killough
// Based only a little bit on Chi's v0.2 code
//

extern void I_InitKeyboard();      // i_system.c

/////////////////////////////////////////////////////////////////////////////////
// Keyboard handling

//
//  Translates the key currently in key
//

static int I_TranslateKey(SDL_Keysym* key)
{
  int rc = 0;

  switch (key->sym) {
  case SDLK_LEFT: rc = KEYD_LEFTARROW;  break;
  case SDLK_RIGHT:  rc = KEYD_RIGHTARROW; break;
  case SDLK_DOWN: rc = KEYD_DOWNARROW;  break;
  case SDLK_UP:   rc = KEYD_UPARROW;  break;
  case SDLK_ESCAPE: rc = KEYD_ESCAPE; break;
  case SDLK_RETURN: rc = KEYD_ENTER;  break;
  case SDLK_TAB:  rc = KEYD_TAB;    break;
  case SDLK_F1:   rc = KEYD_F1;   break;
  case SDLK_F2:   rc = KEYD_F2;   break;
  case SDLK_F3:   rc = KEYD_F3;   break;
  case SDLK_F4:   rc = KEYD_F4;   break;
  case SDLK_F5:   rc = KEYD_F5;   break;
  case SDLK_F6:   rc = KEYD_F6;   break;
  case SDLK_F7:   rc = KEYD_F7;   break;
  case SDLK_F8:   rc = KEYD_F8;   break;
  case SDLK_F9:   rc = KEYD_F9;   break;
  case SDLK_F10:  rc = KEYD_F10;    break;
  case SDLK_F11:  rc = KEYD_F11;    break;
  case SDLK_F12:  rc = KEYD_F12;    break;
  case SDLK_BACKSPACE:  rc = KEYD_BACKSPACE;  break;
  case SDLK_DELETE: rc = KEYD_DEL;  break;
  case SDLK_INSERT: rc = KEYD_INSERT; break;
  case SDLK_PAGEUP: rc = KEYD_PAGEUP; break;
  case SDLK_PAGEDOWN: rc = KEYD_PAGEDOWN; break;
  case SDLK_HOME: rc = KEYD_HOME; break;
  case SDLK_END:  rc = KEYD_END;  break;
  case SDLK_PAUSE:  rc = KEYD_PAUSE;  break;
  case SDLK_EQUALS: rc = KEYD_EQUALS; break;
  case SDLK_MINUS:  rc = KEYD_MINUS;  break;
  case SDLK_KP_0:  rc = KEYD_KEYPAD0;  break;
  case SDLK_KP_1:  rc = KEYD_KEYPAD1;  break;
  case SDLK_KP_2:  rc = KEYD_KEYPAD2;  break;
  case SDLK_KP_3:  rc = KEYD_KEYPAD3;  break;
  case SDLK_KP_4:  rc = KEYD_KEYPAD4;  break;
  case SDLK_KP_5:  rc = KEYD_KEYPAD5;  break;
  case SDLK_KP_6:  rc = KEYD_KEYPAD6;  break;
  case SDLK_KP_7:  rc = KEYD_KEYPAD7;  break;
  case SDLK_KP_8:  rc = KEYD_KEYPAD8;  break;
  case SDLK_KP_9:  rc = KEYD_KEYPAD9;  break;
  case SDLK_KP_PLUS:  rc = KEYD_KEYPADPLUS; break;
  case SDLK_KP_MINUS: rc = KEYD_KEYPADMINUS;  break;
  case SDLK_KP_DIVIDE:  rc = KEYD_KEYPADDIVIDE; break;
  case SDLK_KP_MULTIPLY: rc = KEYD_KEYPADMULTIPLY; break;
  case SDLK_KP_ENTER: rc = KEYD_KEYPADENTER;  break;
  case SDLK_KP_PERIOD:  rc = KEYD_KEYPADPERIOD; break;
  case SDLK_LSHIFT:
  case SDLK_RSHIFT: rc = KEYD_RSHIFT; break;
  case SDLK_LCTRL:
  case SDLK_RCTRL:  rc = KEYD_RCTRL;  break;
  case SDLK_LALT:
  case SDLK_LGUI:
  case SDLK_RALT:
  case SDLK_RGUI:  rc = KEYD_RALT;   break;
  case SDLK_CAPSLOCK: rc = KEYD_CAPSLOCK; break;
  case SDLK_PRINTSCREEN: rc = KEYD_PRINTSC; break;
  default:    rc = key->sym;    break;
  }

  return rc;

}

int I_ScanCode2DoomCode (int a)
{
   // haleyjd
   return a;
}

// Automatic caching inverter, so you don't need to maintain two tables.
// By Lee Killough

int I_DoomCode2ScanCode (int a)
{
   // haleyjd
   return a;
}

// [FG] window event handling from Chocolate Doom 3.0

static void HandleWindowEvent(SDL_WindowEvent *event)
{
    switch (event->event)
    {
        // Don't render the screen when the window is minimized:

        case SDL_WINDOWEVENT_MINIMIZED:
            screenvisible = false;
            break;

        case SDL_WINDOWEVENT_MAXIMIZED:
        case SDL_WINDOWEVENT_RESTORED:
            screenvisible = true;
            break;

        // Update the value of window_focused when we get a focus event
        //
        // We try to make ourselves be well-behaved: the grab on the mouse
        // is removed if we lose focus (such as a popup window appearing),
        // and we dont move the mouse around if we aren't focused either.

        case SDL_WINDOWEVENT_FOCUS_GAINED:
            window_focused = true;
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            window_focused = false;
            break;

        default:
            break;
    }
}

// killough 3/22/98: rewritten to use interrupt-driven keyboard queue

extern int usemouse;

void I_GetEvent()
{
   SDL_Event event;
   event_t   d_event;
   
   event_t mouseevent = { ev_mouse, 0, 0, 0 };
   static int buttons = 0;
   int sendmouseevent = 0;
   
   while(SDL_PollEvent(&event))
   {
      // haleyjd 10/08/05: from Chocolate DOOM
      if(!window_focused && 
         (event.type == SDL_MOUSEMOTION || 
          event.type == SDL_MOUSEBUTTONDOWN || 
          event.type == SDL_MOUSEBUTTONUP))
      {
         continue;
      }

      switch(event.type)
      {
      case SDL_KEYDOWN:
         d_event.type = ev_keydown;
         d_event.data1 = I_TranslateKey(&event.key.keysym);
         // haleyjd 08/29/03: don't post out-of-range keys
         if(d_event.data1 > 0 && d_event.data1 < 256)
            D_PostEvent(&d_event);
         break;
      case SDL_KEYUP:
         d_event.type = ev_keyup;
         d_event.data1 = I_TranslateKey(&event.key.keysym);
         // haleyjd 08/29/03: don't post out-of-range keys
         if(d_event.data1 > 0 && d_event.data1 < 256)
            D_PostEvent(&d_event);
         break;
      case SDL_MOUSEMOTION:       
         if(!usemouse)
            continue;

         // SoM 1-20-04 Ok, use xrel/yrel for mouse movement because most people like it the most.
         mouseevent.data3 -= event.motion.yrel;
         mouseevent.data2 += event.motion.xrel;
         sendmouseevent = 1;
         break;
      case SDL_MOUSEBUTTONUP:
         if(!usemouse)
            continue;
         sendmouseevent = 1;
         d_event.type = ev_keyup;
         if(event.button.button == SDL_BUTTON_LEFT)
         {
            buttons &= ~1;
            d_event.data1 = buttons;
         }
         else if(event.button.button == SDL_BUTTON_MIDDLE)
         {
            buttons &= ~4;
            d_event.data1 = buttons;
         }
         else
         {
            buttons &= ~2;
            d_event.data1 = buttons;
         }
         D_PostEvent(&d_event);
         break;
      case SDL_MOUSEBUTTONDOWN:
         if(!usemouse)
            continue;
         sendmouseevent = 1;
         d_event.type = ev_keydown;
         if(event.button.button == SDL_BUTTON_LEFT)
         {
            buttons |= 1;
            d_event.data1 = buttons;
         }
         else if(event.button.button == SDL_BUTTON_MIDDLE)
         {
            buttons |= 4;
            d_event.data1 = buttons;
         }
         else
         {
            buttons |= 2;
            d_event.data1 = buttons;
         }
         D_PostEvent(&d_event);
         break;
        
      case SDL_QUIT:
         exit(0);
         break;

      case SDL_WINDOWEVENT:
         if (event.window.windowID == SDL_GetWindowID(screen))
         {
            HandleWindowEvent(&event.window);
         }
         break;

      default:
         break;
      }
   }

   if(sendmouseevent)
   {
      mouseevent.data1 = buttons;
      D_PostEvent(&mouseevent);
   }

   if(paused || !window_focused)
      SDL_Delay(1);
}

//
// I_StartTic
//

void I_StartTic()
{
  I_GetEvent();
}

//
// I_UpdateNoBlit
//

void I_UpdateNoBlit (void)
{
}


int use_vsync;     // killough 2/8/98: controls whether vsync is called
static int in_graphics_mode;

void I_FinishUpdate(void)
{
   if (noblit || !in_graphics_mode)
      return;

   // haleyjd 10/08/05: from Chocolate DOOM:

   UpdateGrab();

   // draws little dots on the bottom of the screen
   if(devparm)
   {
      static int lasttic;
      byte *s = screens[0];
      
      int i = I_GetTime();
      int tics = i - lasttic;
      lasttic = i;
      if (tics > 20)
      {
         tics = 20;
      }
         for (i=0 ; i<tics*2 ; i+=2)
            s[(SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
         for ( ; i<20*2 ; i+=2)
            s[(SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
   }

   SDL_BlitSurface(sdlscreen, NULL, argbbuffer, NULL);

   SDL_UpdateTexture(texture, NULL, argbbuffer->pixels, argbbuffer->pitch);

   SDL_RenderClear(renderer);
   SDL_RenderCopy(renderer, texture, NULL, NULL);
   SDL_RenderPresent(renderer);
}

//
// I_ReadScreen
//

void I_ReadScreen(byte *scr)
{
   int size = SCREENWIDTH*SCREENHEIGHT;

   // haleyjd
   memcpy(scr, *screens, size);
}

//
// killough 10/98: init disk icon
//

int disk_icon;

static void I_InitDiskFlash(void)
{
}

//
// killough 10/98: draw disk icon
//

void I_BeginRead(void)
{
}

//
// killough 10/98: erase disk icon
//

void I_EndRead(void)
{
}

void I_SetPalette(byte *palette)
{
   // haleyjd
   int i;
   SDL_Color colors[256];
   
   if(!in_graphics_mode)             // killough 8/11/98
      return;
   
   for(i = 0; i < 256; ++i)
   {
      colors[i].r = gammatable[usegamma][*palette++];
      colors[i].g = gammatable[usegamma][*palette++];
      colors[i].b = gammatable[usegamma][*palette++];
   }
   
   SDL_SetPaletteColors(sdlscreen->format->palette, colors, 0, 256);
}

void I_ShutdownGraphics(void)
{
   if(in_graphics_mode)  // killough 10/98
   {
      UpdateGrab();      
      in_graphics_mode = false;
   }
}

extern boolean setsizeneeded;

extern void I_InitKeyboard();

int cfg_scalefactor; // haleyjd 05/11/09: scale factor in config
int cfg_aspectratio; // haleyjd 05/11/09: aspect ratio correction

// haleyjd 05/11/09: true if called from I_ResetScreen
static boolean changeres = false;

static void I_InitGraphicsMode(void)
{
   static boolean firsttime = true;
   
   // haleyjd
   int v_w = SCREENWIDTH;
   int v_h = SCREENHEIGHT;
   int flags = 0;
   int scalefactor = cfg_scalefactor;
   int useaspect = cfg_aspectratio;

   // [FG] SDL2
   int actualheight;
   uint32_t pixel_format;
   int video_display;
   SDL_DisplayMode mode;

   if(firsttime)
   {
      I_InitKeyboard();
      firsttime = false;
   }

   // haleyjd 10/09/05: from Chocolate DOOM
   // mouse grabbing   
   if(M_CheckParm("-grabmouse"))
      grabmouse = 1;
   else if(M_CheckParm("-nograbmouse"))
      grabmouse = 0;

   // [FG] window flags
   flags |= SDL_WINDOW_ALLOW_HIGHDPI;

   fullscreen = 1; // 5/11/09: forgotten O_O
   flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

   if(M_CheckParm("-aspect"))
      useaspect = true;

   actualheight = useaspect ? (6 * v_h / 5) : v_h;

   // [FG] create rendering window

   if (screen == NULL)
   {
      screen = SDL_CreateWindow(NULL,
                                // centered window
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                v_w, v_h, flags);

      SDL_SetWindowMinimumSize(screen, v_w, actualheight);

      if (screen == NULL)
      {
         I_Error("Error creating window for video startup: %s",
                 SDL_GetError());
      }
   }

   pixel_format = SDL_GetWindowPixelFormat(screen);
   video_display = SDL_GetWindowDisplayIndex(screen);

   flags = 0;

   if (SDL_GetCurrentDisplayMode(video_display, &mode) != 0)
   {
      I_Error("Could not get display mode for video display #%d: %s",
              video_display, SDL_GetError());
   }

   if (page_flip && use_vsync && !timingdemo && mode.refresh_rate > 0)
   {
       flags |= SDL_RENDERER_PRESENTVSYNC;
   }

   // [FG] page_flip = !force_software_renderer
   if (!page_flip)
   {
       flags |= SDL_RENDERER_SOFTWARE;
       flags &= ~SDL_RENDERER_PRESENTVSYNC;
       use_vsync = false;
   }

   if (renderer != NULL)
   {
      SDL_DestroyRenderer(renderer);
      texture = NULL;
   }

   renderer = SDL_CreateRenderer(screen, -1, flags);

   if (renderer == NULL && page_flip)
   {
       flags |= SDL_RENDERER_SOFTWARE;
       flags &= ~SDL_RENDERER_PRESENTVSYNC;

       renderer = SDL_CreateRenderer(screen, -1, flags);

       if (renderer != NULL)
       {
           // remove any special flags
           use_vsync = page_flip = false;
       }
   }

   if (renderer == NULL)
   {
      I_Error("Error creating renderer for screen window: %s",
              SDL_GetError());
   }

   SDL_RenderSetLogicalSize(renderer, v_w, actualheight);

   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);
   SDL_RenderPresent(renderer);

   // [FG] create paletted frame buffer

   if (sdlscreen != NULL)
   {
      SDL_FreeSurface(sdlscreen);
      sdlscreen = NULL;
   }

   if (sdlscreen == NULL)
   {
      sdlscreen = SDL_CreateRGBSurface(0,
                                       v_w, v_h, 8,
                                       0, 0, 0, 0);
      SDL_FillRect(sdlscreen, NULL, 0);

      // [FG] screen buffer
      screens[0] = sdlscreen->pixels;
   }

   // [FG] create intermediate ARGB frame buffer

   if (argbbuffer != NULL)
   {
      SDL_FreeSurface(argbbuffer);
      argbbuffer = NULL;
   }

   if (argbbuffer == NULL)
   {
      unsigned int rmask, gmask, bmask, amask;
      int unused_bpp;

      SDL_PixelFormatEnumToMasks(pixel_format, &unused_bpp,
                                 &rmask, &gmask, &bmask, &amask);
      argbbuffer = SDL_CreateRGBSurface(0,
                                        v_w, v_h, 32,
                                        rmask, gmask, bmask, amask);
      SDL_FillRect(argbbuffer, NULL, 0);
   }

   // [FG] create texture

   if (texture != NULL)
   {
      SDL_DestroyTexture(texture);
   }

   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

   texture = SDL_CreateTexture(renderer,
                               pixel_format,
                               SDL_TEXTUREACCESS_STREAMING,
                               v_w, v_h);

   V_Init();

   SDL_SetWindowTitle(screen, BOOM_WINDOW_TEXT);

   UpdateGrab();

   in_graphics_mode = 1;
   in_page_flip = true;
   setsizeneeded = true;
   
   I_InitDiskFlash();        // Initialize disk icon 
     
   I_SetPalette(W_CacheLumpName("PLAYPAL",PU_CACHE));
}

void I_ResetScreen(void)
{
   if(!in_graphics_mode)
   {
      setsizeneeded = true;
      V_Init();
      return;
   }

   I_ShutdownGraphics();     // Switch out of old graphics mode
   
   changeres = true; // haleyjd 05/11/09
   in_page_flip = page_flip;

   I_InitGraphicsMode();     // Switch to new graphics mode
   
   changeres = false;
   
   if(automapactive)
      AM_Start();             // Reset automap dimensions
   
   ST_Start();               // Reset palette
   
   if(gamestate == GS_INTERMISSION)
   {
      WI_DrawBackground();
      V_CopyRect(0, 0, 1, SCREENWIDTH, SCREENHEIGHT, 0, 0, 0);
   }
}

void I_InitGraphics(void)
{
  static int firsttime = 1;

  if(!firsttime)
    return;

  firsttime = 0;

  //
  // enter graphics mode
  //

  atexit(I_ShutdownGraphics);

  I_InitGraphicsMode();    // killough 10/98
}
