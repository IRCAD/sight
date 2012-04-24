/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2012.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _POINTLISTREGISTRATION_CONFIG_HPP_
#define _POINTLISTREGISTRATION_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef POINTLISTREGISTRATION_EXPORTS
            #define POINTLISTREGISTRATION_API __declspec(dllexport)
        #else
            #define POINTLISTREGISTRATION_API __declspec(dllimport)
        #endif

        #define POINTLISTREGISTRATION_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef POINTLISTREGISTRATION_EXPORTS
            #define POINTLISTREGISTRATION_API __attribute__ ((visibility("default")))
            #define POINTLISTREGISTRATION_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define POINTLISTREGISTRATION_API __attribute__ ((visibility("hidden")))
            #define POINTLISTREGISTRATION_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define POINTLISTREGISTRATION_API
        #define POINTLISTREGISTRATION_CLASS_API

    #endif

#endif //POINTLISTREGISTRATION_API

