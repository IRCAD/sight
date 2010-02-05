/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRENDER_CONFIG_HPP_
#define _FWRENDER_CONFIG_HPP_

#ifdef _WIN32

	#ifdef FWRENDER_EXPORTS
	#define FWRENDER_API __declspec(dllexport)
	#else
	#define FWRENDER_API __declspec(dllimport)
	#endif

	#define FWRENDER_CLASS_API

	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef FWRENDER_EXPORTS
	#define FWRENDER_API __attribute__ ((visibility("default")))
	#define FWRENDER_CLASS_API __attribute__ ((visibility("default")))
	#else
	#define FWRENDER_API __attribute__ ((visibility("hidden")))
	#define FWRENDER_CLASS_API __attribute__ ((visibility("hidden")))
	#endif

#else

	#define FWRENDER_API
	#define FWRENDER_CLASS_API

#endif

#endif //FWRENDER_API
