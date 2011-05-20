/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTKGDCM_CONFIG_HPP_
#define _IOVTKGDCM_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IOVTKGDCM_EXPORTS
    #define IOVTKGDCM_API __declspec(dllexport)
    #else
    #define IOVTKGDCM_API __declspec(dllimport)
    #endif

    #define IOVTKGDCM_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOVTKGDCM_EXPORTS
    #define IOVTKGDCM_API __attribute__ ((visibility("default")))
    #define IOVTKGDCM_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOVTKGDCM_API __attribute__ ((visibility("hidden")))
    #define IOVTKGDCM_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOVTKGDCM_API
    #define IOVTKGDCM_CLASS_API

#endif

#endif // #ifndef _IOVTKGDCM_CONFIG_HPP_
