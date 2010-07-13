/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _FWZIP_CONFIG_HPP_
#define _FWZIP_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWZIP_EXPORTS
    #define FWZIP_API __declspec(dllexport)
    #else
    #define FWZIP_API __declspec(dllimport)
    #endif

    #define FWZIP_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWZIP_EXPORTS
    #define FWZIP_API __attribute__ ((visibility("default")))
    #define FWZIP_CLASS_API __attribute__ ((visibility("default")))
    #else
    //#define FWZIP_API __attribute__ ((visibility("hidden")))
    //#define FWZIP_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWZIP_API
    #define FWZIP_CLASS_API
    #endif

#else

    #define FWZIP_API
    #define FWZIP_CLASS_API

#endif

#endif //_FWZIP_CONFIG_HPP_
