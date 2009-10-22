/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPIMAGEFILTER_CONFIG_HPP_
#define _OPIMAGEFILTER_CONFIG_HPP_


#ifdef _WIN32

	#ifdef OPIMAGEFILTER_EXPORTS
	#define OPIMAGEFILTER_API __declspec(dllexport)
	#else
	#define OPIMAGEFILTER_API __declspec(dllimport)
	#endif

	#define OPIMAGEFILTER_CLASS_API

	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef OPIMAGEFILTER_EXPORTS
	#define OPIMAGEFILTER_API __attribute__ ((visibility("default")))
	#define OPIMAGEFILTER_CLASS_API __attribute__ ((visibility("default")))
	#else
	#define OPIMAGEFILTER_API __attribute__ ((visibility("hidden")))
	#define OPIMAGEFILTER_CLASS_API __attribute__ ((visibility("hidden")))
	#endif

#else

	#define OPIMAGEFILTER_API
	#define OPIMAGEFILTER_CLASS_API

#endif

#endif //_OPIMAGEFILTER_CONFIG_HPP_
