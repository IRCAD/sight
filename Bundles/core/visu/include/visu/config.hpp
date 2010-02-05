/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISU_CONFIG_HPP_
#define _VISU_CONFIG_HPP_


#ifdef _WIN32

    #ifdef VISU_EXPORTS
    #define VISU_API __declspec(dllexport)
    #else
    #define VISU_API __declspec(dllimport)
    #endif

    #define VISU_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef VISU_EXPORTS
    #define VISU_API __attribute__ ((visibility("default")))
    #define VISU_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define VISU_API __attribute__ ((visibility("hidden")))
    #define VISU_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define VISU_API
    #define VISU_CLASS_API

#endif

#endif //_VISU_CONFIG_HPP_
