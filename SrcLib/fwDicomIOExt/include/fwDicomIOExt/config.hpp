/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_CONFIG_HPP__
#define __FWDICOMIOEXT_CONFIG_HPP__


#ifdef _WIN32

    #ifdef FWDICOMIOEXT_EXPORTS
    #define FWDICOMIOEXT_API __declspec(dllexport)
    #define FWDICOMIOEXT_TEMPLATE_API
    #else
    #define FWDICOMIOEXT_API __declspec(dllimport)
    #define FWDICOMIOEXT_TEMPLATE_API extern
    #endif

    #define FWDICOMIOEXT_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWDICOMIOEXT_EXPORTS
        #define FWDICOMIOEXT_API __attribute__ ((visibility("default")))
        #define FWDICOMIOEXT_CLASS_API __attribute__ ((visibility("default")))
        #define FWDICOMIOEXT_TEMPLATE_API
        #else
        //#define FWDICOMIOEXT_API __attribute__ ((visibility("hidden")))
        //#define FWDICOMIOEXT_CLASS_API __attribute__ ((visibility("hidden")))
        #define FWDICOMIOEXT_API
        #define FWDICOMIOEXT_CLASS_API
        #define FWDICOMIOEXT_TEMPLATE_API extern
        #endif

#else

        #define FWDICOMIOEXT_API
        #define FWDICOMIOEXT_CLASS_API
        #define FWDICOMIOEXT_TEMPLATE_API

#endif


#endif //__FWDICOMIOEXT_CONFIG_HPP__

