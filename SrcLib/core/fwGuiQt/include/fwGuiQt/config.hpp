/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _FWGUIQT_CONFIG_HPP_
#define _FWGUIQT_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWGUIQT_EXPORTS
    #define FWGUIQT_API __declspec(dllexport)
    #else
    #define FWGUIQT_API __declspec(dllimport)
    #endif

    #define FWGUIQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWGUIQT_EXPORTS
    #define FWGUIQT_API __attribute__ ((visibility("default")))
    #define FWGUIQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    //#define FWGUIQT_API __attribute__ ((visibility("hidden")))
    //#define FWGUIQT_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWGUIQT_API
    #define FWGUIQT_CLASS_API
    #endif

#else

    #define FWGUIQT_API
    #define FWGUIQT_CLASS_API

#endif

#endif //_FWGUIQT_CONFIG_HPP_
