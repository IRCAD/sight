/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _FWGUIWX_CONFIG_HPP_
#define _FWGUIWX_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWGUIWX_EXPORTS
    #define FWGUIWX_API __declspec(dllexport)
    #else
    #define FWGUIWX_API __declspec(dllimport)
    #endif

    #define FWGUIWX_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWGUIWX_EXPORTS
    #define FWGUIWX_API __attribute__ ((visibility("default")))
    #define FWGUIWX_CLASS_API __attribute__ ((visibility("default")))
    #else
    //#define FWGUIWX_API __attribute__ ((visibility("hidden")))
    //#define FWGUIWX_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWGUIWX_API
    #define FWGUIWX_CLASS_API
    #endif

#else

    #define FWGUIWX_API
    #define FWGUIWX_CLASS_API

#endif

#endif //_FWGUIWX_CONFIG_HPP_
