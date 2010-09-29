/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_CONFIG_HPP_
#define _GUI_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef GUI_EXPORTS
    #define GUI_API __declspec(dllexport)
    #else
    #define GUI_API __declspec(dllimport)
    #endif

    #define GUI_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef GUI_EXPORTS
    #define GUI_API __attribute__ ((visibility("default")))
    #define GUI_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define GUI_API __attribute__ ((visibility("hidden")))
    #define GUI_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define GUI_API
    #define GUI_CLASS_API

#endif

#endif // #ifndef _GUI_CONFIG_HPP_
