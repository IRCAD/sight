/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRENDERVTK_CONFIG_HPP_
#define _FWRENDERVTK_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWRENDERVTK_EXPORTS
    #define FWRENDERVTK_API __declspec(dllexport)
    #else
    #define FWRENDERVTK_API __declspec(dllimport)
    #endif

    #define FWRENDERVTK_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWRENDERVTK_EXPORTS
    #define FWRENDERVTK_API __attribute__ ((visibility("default")))
    #define FWRENDERVTK_CLASS_API __attribute__ ((visibility("default")))
    #else
    //#define FWRENDERVTK_API __attribute__ ((visibility("hidden")))
    //#define FWRENDERVTK_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWRENDERVTK_API
    #define FWRENDERVTK_CLASS_API
    #endif

#else

    #define FWRENDERVTK_API
    #define FWRENDERVTK_CLASS_API

#endif

#endif //_FWRENDERVTK_CONFIG_HPP_
