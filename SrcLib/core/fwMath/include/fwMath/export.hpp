/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMATH_CONFIG_HPP_
#define _FWMATH_CONFIG_HPP_


#ifdef _WIN32

	#ifdef FWMATH_EXPORTS
	#define FWMATH_API __declspec(dllexport)
	#else
	#define FWMATH_API __declspec(dllimport)
	#endif
	
	#define FWMATH_CLASS_API

	#pragma warning(disable: 4290)
	
#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWMATH_EXPORTS
        #define FWMATH_API __attribute__ ((visibility("default")))
        #define FWMATH_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define FWMATH_API __attribute__ ((visibility("hidden")))
        #define FWMATH_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

#else

    #define FWMATH_API
	#define FWMATH_CLASS_API

#endif


#endif //_FWMATH_CONFIG_HPP_
