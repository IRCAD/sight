/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIO_CONFIG_HPP_
#define _UIIO_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIIO_EXPORTS
    #define UIIO_API __declspec(dllexport)
    #else
    #define UIIO_API __declspec(dllimport)
    #endif

    #define UIIO_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIIO_EXPORTS
    #define UIIO_API __attribute__ ((visibility("default")))
    #define UIIO_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIIO_API __attribute__ ((visibility("hidden")))
    #define UIIO_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIIO_API
    #define UIIO_CLASS_API

#endif

#endif //_UIIO_CONFIG_HPP_
