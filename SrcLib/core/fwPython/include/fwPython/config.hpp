/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWPYTHON_CONFIG_HPP_
#define _FWPYTHON_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWPYTHON_EXPORTS
    #define FWPYTHON_API __declspec(dllexport)
    #else
    #define FWPYTHON_API __declspec(dllimport)
    #endif

    #define FWPYTHON_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWPYTHON_EXPORTS
    #define FWPYTHON_API __attribute__ ((visibility("default")))
    #define FWPYTHON_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define FWPYTHON_API __attribute__ ((visibility("hidden")))
    #define FWPYTHON_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define FWPYTHON_API
    #define FWPYTHON_CLASS_API

#endif

#endif //FWPYTHON_CONFIG_API
