/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_CONFIG_HPP_
#define _FWDATAIO_CONFIG_HPP_

#include <assert.h>

#ifdef _WIN32

	#ifdef FWDATAIO_EXPORTS
	#define FWDATAIO_API __declspec(dllexport)
	#define FWDATAIO_TEMPLATE_API 
	#else
	#define FWDATAIO_API __declspec(dllimport)
	#define FWDATAIO_TEMPLATE_API extern "C++"
	#endif

	#define FWDATAIO_CLASS_API

	#pragma warning(disable: 4290)
	#pragma warning(disable: 4267) // disable warning 'argument'�: conversion de 'size_t' en 'unsigned int', perte possible de donn�es
	#pragma warning(disable: 4800) // disable warning C4800: 'const int'�: valeur forc�e � la valeur bool�enne 'true' ou 'false' (avertissement sur les performances)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

	#ifdef FWDATAIO_EXPORTS
	#define FWDATAIO_API __attribute__ ((visibility("default")))
	#define FWDATAIO_CLASS_API __attribute__ ((visibility("default")))
	#define FWDATAIO_TEMPLATE_API extern "C++"
	#else
	#define FWDATAIO_API __attribute__ ((visibility("hidden")))
	#define FWDATAIO_CLASS_API __attribute__ ((visibility("hidden")))
	#define FWDATAIO_TEMPLATE_API extern "C++"
	#endif

#else

	#define FWDATAIO_API
	#define FWDATAIO_CLASS_API
	#define FWDATAIO_TEMPLATE_API extern "C++"

#endif

#endif //_FWDATAIO_CONFIG_HPP_
