/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_CONFIG_HPP_
#define _VTKIO_CONFIG_HPP_


#ifdef _WIN32

	#ifdef VTKIO_EXPORTS
	#define VTKIO_API __declspec(dllexport)
    #define VTKIO_TEMPLATE_API
	#else
	#define VTKIO_API __declspec(dllimport)
    #define VTKIO_TEMPLATE_API extern
	#endif

	#define VTKIO_CLASS_API

	#pragma warning(disable: 4290)
	#pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef VTKIO_EXPORTS
        #define VTKIO_API __attribute__ ((visibility("default")))
        #define VTKIO_CLASS_API __attribute__ ((visibility("default")))
		#define VTKIO_TEMPLATE_API
        #else
        //#define VTKIO_API __attribute__ ((visibility("hidden")))
        //#define VTKIO_CLASS_API __attribute__ ((visibility("hidden")))
        #define VTKIO_API
        #define VTKIO_CLASS_API
		#define VTKIO_TEMPLATE_API extern
        #endif

#else

        #define VTKIO_API
		#define VTKIO_CLASS_API
		#define VTKIO_TEMPLATE_API

#endif


#endif //_VTKIO_CONFIG_HPP_

