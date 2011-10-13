/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUMQT_CONFIG_HPP_
#define _DEVFORUMQT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef DEVFORUMQT_EXPORTS
    #define DEVFORUMQT_API __declspec(dllexport)
    #else
    #define DEVFORUMQT_API __declspec(dllimport)
    #endif

    #define DEVFORUMQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef DEVFORUMQT_EXPORTS
    #define DEVFORUMQT_API __attribute__ ((visibility("default")))
    #define DEVFORUMQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define DEVFORUMQT_API __attribute__ ((visibility("hidden")))
    #define DEVFORUMQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define DEVFORUMQT_API
    #define DEVFORUMQT_CLASS_API

#endif

#endif // _DEVFORUMQT_CONFIG_HPP_
