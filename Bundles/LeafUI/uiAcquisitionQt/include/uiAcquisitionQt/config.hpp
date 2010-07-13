/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIACQUISITIONQT_CONFIG_HPP_
#define _UIACQUISITIONQT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef UIACQUISITIONQT_EXPORTS
    #define UIACQUISITIONQT_API __declspec(dllexport)
    #else
    #define UIACQUISITIONQT_API __declspec(dllimport)
    #endif

    #define UIACQUISITIONQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIACQUISITIONQT_EXPORTS
    #define UIACQUISITIONQT_API __attribute__ ((visibility("default")))
    #define UIACQUISITIONQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIACQUISITIONQT_API __attribute__ ((visibility("hidden")))
    #define UIACQUISITIONQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIACQUISITIONQT_API
    #define UIACQUISITIONQT_CLASS_API

#endif

#endif //_UIACQUISITIONQT_CONFIG_HPP_
