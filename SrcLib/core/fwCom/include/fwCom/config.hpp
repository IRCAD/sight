/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2012.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _FWCOM_CONFIG_HPP_
#define _FWCOM_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWCOM_EXPORTS
            #define FWCOM_API __declspec(dllexport)
        #else
            #define FWCOM_API __declspec(dllimport)
        #endif

        #define FWCOM_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWCOM_EXPORTS
            #define FWCOM_API __attribute__ ((visibility("default")))
            #define FWCOM_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWCOM_API __attribute__ ((visibility("hidden")))
            #define FWCOM_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWCOM_API
        #define FWCOM_CLASS_API

    #endif

#endif //FWCOM_API

