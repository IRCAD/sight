/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_CONFIG_HPP__
#define __FWDATA_CONFIG_HPP__

#ifdef _WIN32

    #ifdef FWDATA_EXPORTS
    #define FWDATA_API __declspec(dllexport)
    #define FWDATA_TEMPLATE_API
    #else
    #define FWDATA_API __declspec(dllimport)
    #define FWDATA_TEMPLATE_API extern
    #endif

    #define FWDATA_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWDATA_EXPORTS
    #define FWDATA_API __attribute__ ((visibility("default")))
    #define FWDATA_CLASS_API __attribute__ ((visibility("default")))
    #define FWDATA_TEMPLATE_API extern
    #else
    #define FWDATA_API __attribute__ ((visibility("hidden")))
    #define FWDATA_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWDATA_TEMPLATE_API extern
    #endif

#else

    #define FWDATA_API
    #define FWDATA_CLASS_API
    #define FWDATA_TEMPLATE_API extern

#endif

#endif // __FWDATA_CONFIG_HPP__

