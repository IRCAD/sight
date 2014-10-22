/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __POC07TIMELINE_CONFIG_HPP__
#define __POC07TIMELINE_CONFIG_HPP__


#ifdef _WIN32

    #ifdef POC07TIMELINE_EXPORTS
    #define POC07TIMELINE_API __declspec(dllexport)
    #else
    #define POC07TIMELINE_API __declspec(dllimport)
    #endif

    #define POC07TIMELINE_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef POC07TIMELINE_EXPORTS
    #define POC07TIMELINE_API __attribute__ ((visibility("default")))
    #define POC07TIMELINE_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define POC07TIMELINE_API __attribute__ ((visibility("hidden")))
    #define POC07TIMELINE_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define POC07TIMELINE_API
    #define POC07TIMELINE_CLASS_API

#endif

#endif //__POC07TIMELINE_CONFIG_HPP__
