/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_CONFIG_HPP_
#define _IODATA_CONFIG_HPP_


#ifdef _WIN32
	#pragma warning( disable : 4251 )


	#ifdef IODATA_EXPORTS
	#define IODATA_API __declspec(dllexport)
	#else
	#define IODATA_API __declspec(dllimport)
	#endif

	#define IODATA_CLASS_API
	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef IODATA_EXPORTS
	#define IODATA_API __attribute__ ((visibility("default")))
	#define IODATA_CLASS_API __attribute__ ((visibility("default")))
	#else
	#define IODATA_API __attribute__ ((visibility("hidden")))
	#define IODATA_CLASS_API __attribute__ ((visibility("hidden")))
	#endif

#else

	#define IODATA_API
	#define IODATA_CLASS_API

#endif

#endif // #ifndef _IODATA_CONFIG_HPP_
