/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIRECONSTRUCTION_CONFIG_HPP_
#define _UIRECONSTRUCTION_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIRECONSTRUCTION_EXPORTS
    #define UIRECONSTRUCTION_API __declspec(dllexport)
    #else
    #define UIRECONSTRUCTION_API __declspec(dllimport)
    #endif

    #define UIRECONSTRUCTION_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIRECONSTRUCTION_EXPORTS
    #define UIRECONSTRUCTION_API __attribute__ ((visibility("default")))
    #define UIRECONSTRUCTION_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIRECONSTRUCTION_API __attribute__ ((visibility("hidden")))
    #define UIRECONSTRUCTION_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIRECONSTRUCTION_API
    #define UIRECONSTRUCTION_CLASS_API

#endif

#endif //_UIRECONSTRUCTION_CONFIG_HPP_
