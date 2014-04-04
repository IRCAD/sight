/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2014.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATATOOLS_CONFIG_HPP__
#define __FWMEDDATATOOLS_CONFIG_HPP__

    #ifdef _WIN32

        #ifdef FWMEDDATATOOLS_EXPORTS
            #define FWMEDDATATOOLS_API __declspec(dllexport)
        #else
            #define FWMEDDATATOOLS_API __declspec(dllimport)
        #endif

        #define FWMEDDATATOOLS_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWMEDDATATOOLS_EXPORTS
            #define FWMEDDATATOOLS_API __attribute__ ((visibility("default")))
            #define FWMEDDATATOOLS_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWMEDDATATOOLS_API __attribute__ ((visibility("hidden")))
            #define FWMEDDATATOOLS_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWMEDDATATOOLS_API
        #define FWMEDDATATOOLS_CLASS_API

    #endif

#endif // __FWMEDDATATOOLS_CONFIG_HPP__

