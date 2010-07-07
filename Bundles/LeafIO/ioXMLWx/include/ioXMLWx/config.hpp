/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IOXMLWX_CONFIG_HPP_
#define IOXMLWX_CONFIG_HPP_


#ifdef _WIN32

    #ifdef IOXMLWX_EXPORTS
    #define IOXMLWX_API __declspec(dllexport)
    #else
    #define IOXMLWX_API __declspec(dllimport)
    #endif

    #define IOXMLWX_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOXMLWX_EXPORTS
    #define IOXMLWX_API __attribute__ ((visibility("default")))
    #define IOXMLWX_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOXMLWX_API __attribute__ ((visibility("hidden")))
    #define IOXMLWX_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOXMLWX_API
    #define IOXMLWX_CLASS_API

#endif

#endif //IOXMLWX_CONFIG_HPP_
