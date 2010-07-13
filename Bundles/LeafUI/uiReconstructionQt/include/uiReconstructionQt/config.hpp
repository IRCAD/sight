/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIRECONSTRUCTIONQT_CONFIG_HPP_
#define _UIRECONSTRUCTIONQT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIRECONSTRUCTIONQT_EXPORTS
    #define UIRECONSTRUCTIONQT_API __declspec(dllexport)
    #else
    #define UIRECONSTRUCTIONQT_API __declspec(dllimport)
    #endif

    #define UIRECONSTRUCTIONQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIRECONSTRUCTIONQT_EXPORTS
    #define UIRECONSTRUCTIONQT_API __attribute__ ((visibility("default")))
    #define UIRECONSTRUCTIONQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIRECONSTRUCTIONQT_API __attribute__ ((visibility("hidden")))
    #define UIRECONSTRUCTIONQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIRECONSTRUCTIONQT_API
    #define UIRECONSTRUCTIONQT_CLASS_API

#endif

#endif //_UIRECONSTRUCTIONQT_CONFIG_HPP_
