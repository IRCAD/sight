/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_CONFIG_HPP_
#define _FWMEMORY_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWMEMORY_EXPORTS
    #define FWMEMORY_API __declspec(dllexport)
    #else
    #define FWMEMORY_API __declspec(dllimport)
    #endif

    #define FWMEMORY_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWMEMORY_EXPORTS
    #define FWMEMORY_API __attribute__ ((visibility("default")))
    #define FWMEMORY_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define FWMEMORY_API __attribute__ ((visibility("hidden")))
    #define FWMEMORY_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define FWMEMORY_API
    #define FWMEMORY_CLASS_API

#endif

#endif // _FWMEMORY_CONFIG_HPP_
