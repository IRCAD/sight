/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_CONFIG_HPP_
#define _FWSERVICES_CONFIG_HPP_


#ifdef _WIN32

	#ifdef FWSERVICES_EXPORTS
	#define FWSERVICES_API __declspec(dllexport)
	#else
	#define FWSERVICES_API __declspec(dllimport)
	#endif

	#define FWSERVICES_CLASS_API

	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWSERVICES_EXPORTS
        #define FWSERVICES_API __attribute__ ((visibility("default")))
        #define FWSERVICES_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define FWSERVICES_API __attribute__ ((visibility("hidden")))
        #define FWSERVICES_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

#else

    #define FWSERVICES_API
	#define FWSERVICES_CLASS_API

#endif


#endif //_FWSERVICES_CONFIG_HPP_
