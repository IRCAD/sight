/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IO_CONFIG_HPP_
#define _IO_CONFIG_HPP_


#ifdef _WIN32

	#ifdef IO_EXPORTS
	#define IO_API __declspec(dllexport)
	#else
	#define IO_API __declspec(dllimport)
	#endif

	#define IO_CLASS_API

	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef IO_EXPORTS
	#define IO_API __attribute__ ((visibility("default")))
	#define IO_CLASS_API __attribute__ ((visibility("default")))
	#else
	#define IO_API __attribute__ ((visibility("hidden")))
	#define IO_CLASS_API __attribute__ ((visibility("hidden")))
	#endif

#else

	#define IO_API
	#define IO_CLASS_API

#endif

#endif //_IO_CONFIG_HPP_
