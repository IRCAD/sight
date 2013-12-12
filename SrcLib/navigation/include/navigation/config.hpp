/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef __NAVIGATION_CONFIG_HPP__
#define __NAVIGATION_CONFIG_HPP__

    #ifdef _WIN32

        #ifdef NAVIGATION_EXPORTS
            #define NAVIGATION_API __declspec(dllexport)
        #else
            #define NAVIGATION_API __declspec(dllimport)
        #endif

        #define NAVIGATION_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef NAVIGATION_EXPORTS
            #define NAVIGATION_API __attribute__ ((visibility("default")))
            #define NAVIGATION_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define NAVIGATION_API __attribute__ ((visibility("hidden")))
            #define NAVIGATION_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define NAVIGATION_API
        #define NAVIGATION_CLASS_API

    #endif

#endif // __NAVIGATION_CONFIG_HPP__

