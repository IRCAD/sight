/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSTRUCTURALPATCH_CONFIG_HPP_
#define _FWSTRUCTURALPATCH_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWSTRUCTURALPATCH_EXPORTS
            #define FWSTRUCTURALPATCH_API __declspec(dllexport)
        #else
            #define FWSTRUCTURALPATCH_API __declspec(dllimport)
            #define FWSTRUCTURALPATCH_TEMPLATE_API extern "C++"
        #endif

        #define FWSTRUCTURALPATCH_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWSTRUCTURALPATCH_EXPORTS
            #define FWSTRUCTURALPATCH_API __attribute__ ((visibility("default")))
            #define FWSTRUCTURALPATCH_CLASS_API __attribute__ ((visibility("default")))
            #define FWSTRUCTURALPATCH_TEMPLATE_API extern "C++"
        #else
            #define FWSTRUCTURALPATCH_API __attribute__ ((visibility("hidden")))
            #define FWSTRUCTURALPATCH_CLASS_API __attribute__ ((visibility("hidden")))
            #define FWSTRUCTURALPATCH_TEMPLATE_API extern "C++"
        #endif

    #else

        #define FWSTRUCTURALPATCH_API
        #define FWSTRUCTURALPATCH_CLASS_API
        #define FWSTRUCTURALPATCH_TEMPLATE_API extern "C++"

    #endif

#endif //_FWSTRUCTURALPATCH_CONFIG_HPP_

