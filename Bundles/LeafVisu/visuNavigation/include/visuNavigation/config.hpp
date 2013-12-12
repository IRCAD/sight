/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef __VISUNAVIGATION_CONFIG_HPP__
#define __VISUNAVIGATION_CONFIG_HPP__

    #ifdef _WIN32

        #ifdef VISUNAVIGATION_EXPORTS
            #define VISUNAVIGATION_API __declspec(dllexport)
        #else
            #define VISUNAVIGATION_API __declspec(dllimport)
        #endif

        #define VISUNAVIGATION_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef VISUNAVIGATION_EXPORTS
            #define VISUNAVIGATION_API __attribute__ ((visibility("default")))
            #define VISUNAVIGATION_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define VISUNAVIGATION_API __attribute__ ((visibility("hidden")))
            #define VISUNAVIGATION_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define VISUNAVIGATION_API
        #define VISUNAVIGATION_CLASS_API

    #endif

#endif // __VISUNAVIGATION_CONFIG_HPP__

