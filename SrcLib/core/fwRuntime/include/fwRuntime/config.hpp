/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_CONFIG_HPP_
#define _FWRUNTIME_CONFIG_HPP_


#ifdef _WIN32

	#ifdef FWRUNTIME_EXPORTS
	#define FWRUNTIME_API __declspec(dllexport)
	#else
	#define FWRUNTIME_API __declspec(dllimport)
	#endif
	
	#define FWRUNTIME_CLASS_API 
	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWRUNTIME_EXPORTS
        #define FWRUNTIME_API __attribute__ ((visibility("default")))
        #define FWRUNTIME_CLASS_API __attribute__ ((visibility("default")))
        #else
        //#define FWRUNTIME_API __attribute__ ((visibility("hidden")))
        //#define FWRUNTIME_CLASS_API __attribute__ ((visibility("hidden")))
        #define FWRUNTIME_API
        #define FWRUNTIME_CLASS_API
        #endif

#else

	#define FWRUNTIME_API 
	#define FWRUNTIME_CLASS_API 
	
#endif


#endif //_FWRUNTIME_CONFIG_HPP_
