// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: doomtype.h,v 1.3 1998/05/03 23:24:33 killough Exp $
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
//      Simple basic typedefs, isolated here to make it easier
//       separating modules.
//
//-----------------------------------------------------------------------------

#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdint.h> // [FG] include for intptr_t types

#ifndef __BYTEBOOL__
#define __BYTEBOOL__
// Fixed to use builtin bool type with C++.
#ifdef __cplusplus
typedef bool boolean;
#else
typedef enum {false, true} boolean;
#endif
typedef unsigned char byte;
#endif

#define REBOOM_PATH_MAX 4096

#ifdef WINDOWS
  #define DIR_SEPARATOR '\\'
  #define DIR_SEPARATOR_S "\\"
  #define PATH_SEPARATOR ';'
#else
  #define DIR_SEPARATOR '/'
  #define DIR_SEPARATOR_S "/"
  #define PATH_SEPARATOR ':'
#endif

/* Adam - These new inline functions were translated
* from C++ templates that I wrote previously in another project
*
template<typename T>
T TMathMin(const T& a, const T& b) {
	return a < b ? a : b;
}
template<typename T>
T TMathMax(const T& a, const T& b) {
	return a > b ? a : b;
}
   inline's are safer than macros for mathematics as macros
   can lead to unsafe calculations
*/

static inline int ReBOOMMathMin(int a, int b) {
	return a < b ? a : b;
}

static inline int ReBOOMMathMax(int a, int b) {
	return a > b ? a : b;
}

/* cph - Wrapper for the long long type, as Win32 used a different name.
 * Except I don't know what to test as it's compiler specific
 * Proff - I fixed it */
#ifndef WINDOWS
typedef signed long long int_64_t;
typedef unsigned long long uint_64_t;
// define compiled-specific long-long contstant notation here
#define LONGLONG(num)   (uint_64_t)num ## ll
#else
typedef __int64 int_64_t;
typedef unsigned __int64 uint_64_t;
#endif

#ifdef WINDOWS
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

// haleyjd: resolve platform-specific range symbol issues

#include <limits.h>
#define D_MAXINT INT_MAX
#define D_MININT INT_MIN
#define D_MAXSHORT  SHRT_MAX

#ifndef WINDOWS
#define MAXCHAR 0x7f
#define MINCHAR 0x80
#endif

#define arrlen(array) (sizeof(array) / sizeof(*array))

#ifdef __GNUC__
#if defined(_WIN32) && !defined(__clang__)
#define PACKEDATTR __attribute__((packed,gcc_struct))
#else
#define PACKEDATTR __attribute__((packed))
#endif

#define PRINTF_ATTR(fmt, first) __attribute__((format(printf, fmt, first)))
#define PRINTF_ARG_ATTR(x) __attribute__((format_arg(x)))
#define NORETURN __attribute__((noreturn))
#else
#if defined(_MSC_VER)
#define PACKEDATTR __pragma(pack(pop))
#else
#define PACKEDATTR
#endif

#define PRINTF_ATTR(fmt, first)
#define PRINTF_ARG_ATTR(x)
#define NORETURN
#endif

#endif
