/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IOTUTO_CONFIG_HPP_
#define IOTUTO_CONFIG_HPP_


#ifdef _WIN32

    #ifdef IOTUTO_EXPORTS
    #define IOTUTO_API __declspec(dllexport)
    #else
    #define IOTUTO_API __declspec(dllimport)
    #endif

    #define IOTUTO_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOTUTO_EXPORTS
    #define IOTUTO_API __attribute__ ((visibility("default")))
    #define IOTUTO_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOTUTO_API __attribute__ ((visibility("hidden")))
    #define IOTUTO_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOTUTO_API
    #define IOTUTO_CLASS_API

#endif

#endif //IOTUTO_CONFIG_HPP_
