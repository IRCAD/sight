/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// fwTools - copyright (C) 2005 IRCAD.
//


#ifndef _FWTOOLS_CONFIG_HPP_
#define _FWTOOLS_CONFIG_HPP_


#ifdef _WIN32

    #ifdef FWTOOLS_EXPORTS
    #define FWTOOLS_API __declspec(dllexport)
    #define FWTOOLS_TEMPLATE_API
    #else
    #define FWTOOLS_API __declspec(dllimport)
    #define FWTOOLS_TEMPLATE_API extern
    #endif

    #define FWTOOLS_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWTOOLS_EXPORTS
        #define FWTOOLS_API __attribute__ ((visibility("default")))
        #define FWTOOLS_CLASS_API __attribute__ ((visibility("default")))
        #define FWTOOLS_TEMPLATE_API
        #else
        //#define FWTOOLS_API __attribute__ ((visibility("hidden")))
        //#define FWTOOLS_CLASS_API __attribute__ ((visibility("hidden")))
        #define FWTOOLS_API
        #define FWTOOLS_CLASS_API
        #define FWTOOLS_TEMPLATE_API extern
        #endif

#else

        #define FWTOOLS_API
    #define FWTOOLS_CLASS_API
        #define FWTOOLS_TEMPLATE_API 

#endif


#endif //_FWTOOLS_CONFIG_HPP_
