/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCAMP_CONFIG_HPP_
#define _FWCAMP_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWCAMP_EXPORTS
            #define FWCAMP_API __declspec(dllexport)
        #else
            #define FWCAMP_API __declspec(dllimport)
        #endif

        #define FWCAMP_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWCAMP_EXPORTS
            #define FWCAMP_API __attribute__ ((visibility("default")))
            #define FWCAMP_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWCAMP_API __attribute__ ((visibility("hidden")))
            #define FWCAMP_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWCAMP_API
        #define FWCAMP_CLASS_API

    #endif

#endif //FWCAMP_API

