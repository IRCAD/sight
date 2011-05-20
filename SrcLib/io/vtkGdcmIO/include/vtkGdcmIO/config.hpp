/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKGDCMIO_CONFIG_HPP_
#define _VTKGDCMIO_CONFIG_HPP_


#ifdef _WIN32

    #ifdef VTKGDCMIO_EXPORTS
    #define VTKGDCMIO_API __declspec(dllexport)
    #define VTKGDCMIO_TEMPLATE_API
    #else
    #define VTKGDCMIO_API __declspec(dllimport)
    #define VTKGDCMIO_TEMPLATE_API extern
    #endif

    #define VTKGDCMIO_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef VTKGDCMIO_EXPORTS
        #define VTKGDCMIO_API __attribute__ ((visibility("default")))
        #define VTKGDCMIO_CLASS_API __attribute__ ((visibility("default")))
        #define VTKGDCMIO_TEMPLATE_API
        #else
        //#define VTKGDCMIO_API __attribute__ ((visibility("hidden")))
        //#define VTKGDCMIO_CLASS_API __attribute__ ((visibility("hidden")))
        #define VTKGDCMIO_API
        #define VTKGDCMIO_CLASS_API
        #define VTKGDCMIO_TEMPLATE_API extern
        #endif

#else

        #define VTKGDCMIO_API
        #define VTKGDCMIO_CLASS_API
        #define VTKGDCMIO_TEMPLATE_API

#endif


#endif //_VTKGDCMIO_CONFIG_HPP_

