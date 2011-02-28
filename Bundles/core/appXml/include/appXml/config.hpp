/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _APPXML_CONFIG_HPP_
#define _APPXML_CONFIG_HPP_

#ifdef _WIN32

    #ifdef APPXML_EXPORTS
    #define APPXML_API __declspec(dllexport)
    #else
    #define APPXML_API __declspec(dllimport)
    #endif

    #define APPXML_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef APPXML_EXPORTS
    #define APPXML_API __attribute__ ((visibility("default")))
    #define APPXML_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define APPXML_API __attribute__ ((visibility("hidden")))
    #define APPXML_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define APPXML_API
    #define APPXML_CLASS_API

#endif

#endif // _APPXML_CONFIG_HPP_
