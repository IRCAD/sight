/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOMIDAS_CONFIG_HPP_
#define _IOMIDAS_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IOMIDAS_EXPORTS
    #define IOMIDAS_API __declspec(dllexport)
    #else
    #define IOMIDAS_API __declspec(dllimport)
    #endif

    #define IOMIDAS_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOMIDAS_EXPORTS
    #define IOMIDAS_API __attribute__ ((visibility("default")))
    #define IOMIDAS_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOMIDAS_API __attribute__ ((visibility("hidden")))
    #define IOMIDAS_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOMIDAS_API
    #define IOMIDAS_CLASS_API

#endif

#endif // #ifndef _IOMIDAS_CONFIG_HPP_
