/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _BINDING_CONFIG_HPP_
#define _BINDING_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef BINDING_EXPORTS
            #define BINDING_API __declspec(dllexport)
        #else
            #define BINDING_API __declspec(dllimport)
        #endif

        #define BINDING_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef BINDING_EXPORTS
            #define BINDING_API __attribute__ ((visibility("default")))
            #define BINDING_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define BINDING_API __attribute__ ((visibility("hidden")))
            #define BINDING_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define BINDING_API
        #define BINDING_CLASS_API

    #endif

#endif //BINDING_API

