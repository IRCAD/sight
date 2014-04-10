/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_CONFIG_HPP__
#define __FWDICOMIOFILTER_CONFIG_HPP__


#ifdef _WIN32

    #ifdef FWDICOMIOFILTER_EXPORTS
    #define FWDICOMIOFILTER_API __declspec(dllexport)
    #define FWDICOMIOFILTER_TEMPLATE_API
    #else
    #define FWDICOMIOFILTER_API __declspec(dllimport)
    #define FWDICOMIOFILTER_TEMPLATE_API extern
    #endif

    #define FWDICOMIOFILTER_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWDICOMIOFILTER_EXPORTS
        #define FWDICOMIOFILTER_API __attribute__ ((visibility("default")))
        #define FWDICOMIOFILTER_CLASS_API __attribute__ ((visibility("default")))
        #define FWDICOMIOFILTER_TEMPLATE_API
        #else
        //#define FWDICOMIOFILTER_API __attribute__ ((visibility("hidden")))
        //#define FWDICOMIOFILTER_CLASS_API __attribute__ ((visibility("hidden")))
        #define FWDICOMIOFILTER_API
        #define FWDICOMIOFILTER_CLASS_API
        #define FWDICOMIOFILTER_TEMPLATE_API extern
        #endif

#else

        #define FWDICOMIOFILTER_API
        #define FWDICOMIOFILTER_CLASS_API
        #define FWDICOMIOFILTER_TEMPLATE_API

#endif


#endif //__FWDICOMIOFILTER_CONFIG_HPP__

