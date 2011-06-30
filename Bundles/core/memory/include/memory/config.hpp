/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MEMORY_CONFIG_HPP_
#define _MEMORY_CONFIG_HPP_


#ifdef _WIN32

    #ifdef MEMORY_EXPORTS
    #define MEMORY_API __declspec(dllexport)
    #else
    #define MEMORY_API __declspec(dllimport)
    #endif

    #define MEMORY_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef MEMORY_EXPORTS
    #define MEMORY_API __attribute__ ((visibility("default")))
    #define MEMORY_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define MEMORY_API __attribute__ ((visibility("hidden")))
    #define MEMORY_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define MEMORY_API
    #define MEMORY_CLASS_API

#endif

#endif //_MEMORY_CONFIG_HPP_
