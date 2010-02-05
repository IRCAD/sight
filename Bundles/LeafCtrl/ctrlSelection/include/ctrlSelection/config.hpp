/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_CONFIG_HPP_
#define _CTRLSELECTION_CONFIG_HPP_


#ifdef _WIN32

	#ifdef CTRLSELECTION_EXPORTS
	#define CTRLSELECTION_API __declspec(dllexport)
	#else
	#define CTRLSELECTION_API __declspec(dllimport)
	#endif

	#define CTRLSELECTION_CLASS_API

	#pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef CTRLSELECTION_EXPORTS
	#define CTRLSELECTION_API __attribute__ ((visibility("default")))
	#define CTRLSELECTION_CLASS_API __attribute__ ((visibility("default")))
	#else
	#define CTRLSELECTION_API __attribute__ ((visibility("hidden")))
	#define CTRLSELECTION_CLASS_API __attribute__ ((visibility("hidden")))
	#endif

#else

	#define CTRLSELECTION_API
	#define CTRLSELECTION_CLASS_API

#endif

#endif // _CTRLSELECTION_CONFIG_HPP_
