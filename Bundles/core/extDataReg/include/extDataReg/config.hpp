/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATAREG_CONFIG_HPP__
#define __EXTDATAREG_CONFIG_HPP__

#ifdef _WIN32

    #ifdef EXTDATAREG_EXPORTS
    #define EXTDATAREG_API __declspec(dllexport)
    #else
    #define EXTDATAREG_API __declspec(dllimport)
    #endif

    #define EXTDATAREG_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef EXTDATAREG_EXPORTS
    #define EXTDATAREG_API __attribute__ ((visibility("default")))
    #define EXTDATAREG_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define EXTDATAREG_API __attribute__ ((visibility("hidden")))
    #define EXTDATAREG_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define EXTDATAREG_API
    #define EXTDATAREG_CLASS_API

#endif

#endif // __EXTDATAREG_CONFIG_HPP__
