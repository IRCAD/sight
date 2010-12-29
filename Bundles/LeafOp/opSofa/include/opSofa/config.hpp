/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPSOFA_CONFIG_HPP_
#define _OPSOFA_CONFIG_HPP_


#ifdef _WIN32

    #ifdef OPSOFA_EXPORTS
    #define OPSOFA_API __declspec(dllexport)
    #else
    #define OPSOFA_API __declspec(dllimport)
    #endif

    #define OPSOFA_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef OPSOFA_EXPORTS
    #define OPSOFA_API __attribute__ ((visibility("default")))
    #define OPSOFA_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define OPSOFA_API __attribute__ ((visibility("hidden")))
    #define OPSOFA_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define OPSOFA_API
    #define OPSOFA_CLASS_API

#endif

#endif //_OPSOFA_CONFIG_HPP_
