/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _FWTEST_CONFIG_HPP_
#define _FWTEST_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWTEST_EXPORTS
            #define FWTEST_API __declspec(dllexport)
        #else
            #define FWTEST_API __declspec(dllimport)
        #endif

        #define FWTEST_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWTEST_EXPORTS
            #define FWTEST_API __attribute__ ((visibility("default")))
            #define FWTEST_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWTEST_API __attribute__ ((visibility("hidden")))
            #define FWTEST_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWTEST_API
        #define FWTEST_CLASS_API

    #endif

#endif //FWTEST_API

