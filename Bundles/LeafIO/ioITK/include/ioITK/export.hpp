/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_CONFIG_HPP_
#define _IOITK_CONFIG_HPP_

#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IOITK_EXPORTS
    #define IOITK_API __declspec(dllexport)
    #else
    #define IOITK_API __declspec(dllimport)
    #endif

    #define IOITK_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOITK_EXPORTS
    #define IOITK_API __attribute__ ((visibility("default")))
    #define IOITK_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOITK_API __attribute__ ((visibility("hidden")))
    #define IOITK_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOITK_API
    #define IOITK_CLASS_API

#endif

#endif // #ifndef _IOITK_CONFIG_HPP_
