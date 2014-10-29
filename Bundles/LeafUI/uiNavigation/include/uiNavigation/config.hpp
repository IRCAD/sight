/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef __UINAVIGATION_CONFIG_HPP__
#define __UINAVIGATION_CONFIG_HPP__

    #ifdef _WIN32

        #ifdef UINAVIGATION_EXPORTS
            #define UINAVIGATION_API __declspec(dllexport)
        #else
            #define UINAVIGATION_API __declspec(dllimport)
        #endif

        #define UINAVIGATION_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef UINAVIGATION_EXPORTS
            #define UINAVIGATION_API __attribute__ ((visibility("default")))
            #define UINAVIGATION_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define UINAVIGATION_API __attribute__ ((visibility("hidden")))
            #define UINAVIGATION_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define UINAVIGATION_API
        #define UINAVIGATION_CLASS_API

    #endif

#endif // __UINAVIGATION_CONFIG_HPP__

