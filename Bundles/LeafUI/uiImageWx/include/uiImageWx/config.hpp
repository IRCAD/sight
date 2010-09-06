/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEWX_CONFIG_HPP_
#define _UIIMAGEWX_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIIMAGEWX_EXPORTS
    #define UIIMAGEWX_API __declspec(dllexport)
    #else
    #define UIIMAGEWX_API __declspec(dllimport)
    #endif

    #define UIIMAGEWX_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIIMAGEWX_EXPORTS
    #define UIIMAGEWX_API __attribute__ ((visibility("default")))
    #define UIIMAGEWX_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIIMAGEWX_API __attribute__ ((visibility("hidden")))
    #define UIIMAGEWX_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIIMAGEWX_API
    #define UIIMAGEWX_CLASS_API

#endif

#endif //_UIIMAGEWX_CONFIG_HPP_
