/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCORE_CONFIG_HPP_
#define _FWCORE_CONFIG_HPP_


#ifdef _WIN32

    #ifdef FWCORE_EXPORTS
    #define FWCORE_API __declspec(dllexport)
    #else
    #define FWCORE_API __declspec(dllimport)
    #endif

    #define FWCORE_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWCORE_EXPORTS
    #define FWCORE_API __attribute__ ((visibility("default")))
    #define FWCORE_CLASS_API __attribute__ ((visibility("default")))
    #else
    //#define FWCORE_API __attribute__ ((visibility("hidden")))
    //#define FWCORE_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWCORE_API
    #define FWCORE_CLASS_API
    #endif

#else

    #define FWCORE_API
    #define FWCORE_CLASS_API

#endif


#endif //_FWCORE_CONFIG_HPP_
