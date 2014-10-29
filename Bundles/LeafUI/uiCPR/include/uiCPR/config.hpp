/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef __UICPR_CONFIG_HPP__
#define __UICPR_CONFIG_HPP__

    #ifdef _WIN32

        #ifdef UICPR_EXPORTS
            #define UICPR_API __declspec(dllexport)
        #else
            #define UICPR_API __declspec(dllimport)
        #endif

        #define UICPR_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef UICPR_EXPORTS
            #define UICPR_API __attribute__ ((visibility("default")))
            #define UICPR_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define UICPR_API __attribute__ ((visibility("hidden")))
            #define UICPR_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define UICPR_API
        #define UICPR_CLASS_API

    #endif

#endif // __UICPR_CONFIG_HPP__

