/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_CONFIG_HPP__
#define __MIDASIO_CONFIG_HPP__


#ifdef _WIN32

    #ifdef MIDASIO_EXPORTS
    #define MIDASIO_API __declspec(dllexport)
    #define MIDASIO_TEMPLATE_API
    #else
    #define MIDASIO_API __declspec(dllimport)
    #define MIDASIO_TEMPLATE_API extern
    #endif

    #define MIDASIO_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef MIDASIO_EXPORTS
        #define MIDASIO_API __attribute__ ((visibility("default")))
        #define MIDASIO_CLASS_API __attribute__ ((visibility("default")))
        #define MIDASIO_TEMPLATE_API
        #else
        //#define MIDASIO_API __attribute__ ((visibility("hidden")))
        //#define MIDASIO_CLASS_API __attribute__ ((visibility("hidden")))
        #define MIDASIO_API
        #define MIDASIO_CLASS_API
        #define MIDASIO_TEMPLATE_API extern
        #endif

#else

        #define MIDASIO_API
        #define MIDASIO_CLASS_API
        #define MIDASIO_TEMPLATE_API

#endif


#endif //__MIDASIO_CONFIG_HPP__

