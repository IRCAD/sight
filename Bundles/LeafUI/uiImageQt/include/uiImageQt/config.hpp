/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_CONFIG_HPP_
#define _UIIMAGEQT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIIMAGEQT_EXPORTS
    #define UIIMAGEQT_API __declspec(dllexport)
    #else
    #define UIIMAGEQT_API __declspec(dllimport)
    #endif

    #define UIIMAGEQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIIMAGEQT_EXPORTS
    #define UIIMAGEQT_API __attribute__ ((visibility("default")))
    #define UIIMAGEQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIIMAGEQT_API __attribute__ ((visibility("hidden")))
    #define UIIMAGEQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIIMAGEQT_API
    #define UIIMAGEQT_CLASS_API

#endif

#endif //_UIIMAGEQT_CONFIG_HPP_
