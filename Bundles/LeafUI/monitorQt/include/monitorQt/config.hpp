/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MONITORQT_CONFIG_HPP_
#define _MONITORQT_CONFIG_HPP_

#ifdef _WIN32

    #ifdef MONITORQT_EXPORTS
    #define MONITORQT_API __declspec(dllexport)
    #else
    #define MONITORQT_API __declspec(dllimport)
    #endif

    #define MONITORQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef MONITORQT_EXPORTS
    #define MONITORQT_API __attribute__ ((visibility("default")))
    #define MONITORQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define MONITORQT_API __attribute__ ((visibility("hidden")))
    #define MONITORQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define MONITORQT_API
    #define MONITORQT_CLASS_API

#endif

#endif // _MONITORQT_CONFIG_HPP_
