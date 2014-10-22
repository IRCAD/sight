/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_CONFIG_HPP__
#define __EXTDATA_CONFIG_HPP__

#ifdef _WIN32

    #ifdef EXTDATA_EXPORTS
    #define EXTDATA_API __declspec(dllexport)
    #else
    #define EXTDATA_API __declspec(dllimport)
    #endif

    #define EXTDATA_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef EXTDATA_EXPORTS
    #define EXTDATA_API __attribute__ ((visibility("default")))
    #define EXTDATA_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define EXTDATA_API __attribute__ ((visibility("hidden")))
    #define EXTDATA_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define EXTDATA_API
    #define EXTDATA_CLASS_API

#endif

#endif // __EXTDATA_CONFIG_HPP__
