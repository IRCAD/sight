/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMDSEMANTICPATCH_CONFIG_HPP_
#define _FWMDSEMANTICPATCH_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWMDSEMANTICPATCH_EXPORTS
            #define FWMDSEMANTICPATCH_API __declspec(dllexport)
        #else
            #define FWMDSEMANTICPATCH_API __declspec(dllimport)
            #define FWMDSEMANTICPATCH_TEMPLATE_API extern "C++"
        #endif

        #define FWMDSEMANTICPATCH_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWMDSEMANTICPATCH_EXPORTS
            #define FWMDSEMANTICPATCH_API __attribute__ ((visibility("default")))
            #define FWMDSEMANTICPATCH_CLASS_API __attribute__ ((visibility("default")))
            #define FWMDSEMANTICPATCH_TEMPLATE_API extern "C++"
        #else
            #define FWMDSEMANTICPATCH_API __attribute__ ((visibility("hidden")))
            #define FWMDSEMANTICPATCH_CLASS_API __attribute__ ((visibility("hidden")))
            #define FWMDSEMANTICPATCH_TEMPLATE_API extern "C++"
        #endif

    #else

        #define FWMDSEMANTICPATCH_API
        #define FWMDSEMANTICPATCH_CLASS_API
        #define FWMDSEMANTICPATCH_TEMPLATE_API extern "C++"

    #endif

#endif //_FWMDSEMANTICPATCH_CONFIG_HPP_

