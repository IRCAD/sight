/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATACAMP_CONFIG_HPP_
#define _FWDATACAMP_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWDATACAMP_EXPORTS
            #define FWDATACAMP_API __declspec(dllexport)
             #define FWDATACAMP_TEMPLATE_API extern "C++"
        #else
            #define FWDATACAMP_API __declspec(dllimport)
            #define FWDATACAMP_TEMPLATE_API extern "C++"
        #endif

        #define FWDATACAMP_CLASS_API
        
        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWDATACAMP_EXPORTS
            #define FWDATACAMP_API __attribute__ ((visibility("default")))
            #define FWDATACAMP_CLASS_API __attribute__ ((visibility("default")))
            #define FWDATACAMP_TEMPLATE_API extern "C++"
        #else
            #define FWDATACAMP_API __attribute__ ((visibility("hidden")))
            #define FWDATACAMP_CLASS_API __attribute__ ((visibility("hidden")))
            #define FWDATACAMP_TEMPLATE_API extern "C++"
        #endif

    #else

        #define FWDATACAMP_API
        #define FWDATACAMP_CLASS_API
        #define FWDATACAMP_TEMPLATE_API extern "C++"

    #endif

#endif //FWDATACAMP_API

