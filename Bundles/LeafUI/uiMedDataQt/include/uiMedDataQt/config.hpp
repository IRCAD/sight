/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEDDATAQT_CONFIG_HPP_
#define _UIMEDDATAQT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIMEDDATAQT_EXPORTS
    #define UIMEDDATAQT_API __declspec(dllexport)
    #else
    #define UIMEDDATAQT_API __declspec(dllimport)
    #endif

    #define UIMEDDATAQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIMEDDATAQT_EXPORTS
    #define UIMEDDATAQT_API __attribute__ ((visibility("default")))
    #define UIMEDDATAQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIMEDDATAQT_API __attribute__ ((visibility("hidden")))
    #define UIMEDDATAQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIMEDDATAQT_API
    #define UIMEDDATAQT_CLASS_API

#endif

#endif //_UIMEDDATAQT_CONFIG_HPP_
