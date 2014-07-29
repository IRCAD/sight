/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef __UIVIEWPOINTS_CONFIG_HPP__
#define __UIVIEWPOINTS_CONFIG_HPP__

    #ifdef _WIN32

        #ifdef UIVIEWPOINTS_EXPORTS
            #define UIVIEWPOINTS_API __declspec(dllexport)
        #else
            #define UIVIEWPOINTS_API __declspec(dllimport)
        #endif

        #define UIVIEWPOINTS_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef UIVIEWPOINTS_EXPORTS
            #define UIVIEWPOINTS_API __attribute__ ((visibility("default")))
            #define UIVIEWPOINTS_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define UIVIEWPOINTS_API __attribute__ ((visibility("hidden")))
            #define UIVIEWPOINTS_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define UIVIEWPOINTS_API
        #define UIVIEWPOINTS_CLASS_API

    #endif

#endif // __UIVIEWPOINTS_CONFIG_HPP__

