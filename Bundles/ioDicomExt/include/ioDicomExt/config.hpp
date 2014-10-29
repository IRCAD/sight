/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_CONFIG_HPP__
#define __IODICOMEXT_CONFIG_HPP__


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IODICOMEXT_EXPORTS
    #define IODICOMEXT_API __declspec(dllexport)
    #else
    #define IODICOMEXT_API __declspec(dllimport)
    #endif

    #define IODICOMEXT_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IODICOMEXT_EXPORTS
    #define IODICOMEXT_API __attribute__ ((visibility("default")))
    #define IODICOMEXT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IODICOMEXT_API __attribute__ ((visibility("hidden")))
    #define IODICOMEXT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IODICOMEXT_API
    #define IODICOMEXT_CLASS_API

#endif

#endif // #ifndef __IODICOMEXT_CONFIG_HPP__
