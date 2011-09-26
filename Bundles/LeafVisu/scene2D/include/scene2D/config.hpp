/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_CONFIG_HPP_
#define _SCENE2D_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef SCENE2D_EXPORTS
            #define SCENE2D_API __declspec(dllexport)
        #else
            #define SCENE2D_API __declspec(dllimport)
        #endif

        #define SCENE2D_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef SCENE2D_EXPORTS
            #define SCENE2D_API __attribute__ ((visibility("default")))
            #define SCENE2D_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define SCENE2D_API __attribute__ ((visibility("hidden")))
            #define SCENE2D_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define SCENE2D_API
        #define SCENE2D_CLASS_API

    #endif

#endif //SCENE2D_API

