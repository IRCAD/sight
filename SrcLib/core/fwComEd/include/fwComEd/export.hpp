/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_CONFIG_HPP_
#define _FWCOMED_CONFIG_HPP_


#ifdef _WIN32

    #ifdef FWCOMED_EXPORTS
    #define FWCOMED_API __declspec(dllexport)
    #else
    #define FWCOMED_API __declspec(dllimport)
    #endif
    
    #define FWCOMED_CLASS_API 

    #pragma warning(disable: 4290)
    
#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWCOMED_EXPORTS
        #define FWCOMED_API __attribute__ ((visibility("default")))
        #define FWCOMED_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define FWCOMED_API __attribute__ ((visibility("hidden")))
        #define FWCOMED_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

#else

    #define FWCOMED_API
    #define FWCOMED_CLASS_API

#endif


#endif //_FWCOMED_CONFIG_HPP_
