/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cassert>

#ifndef _FWXML_CONFIG_HPP_
#define _FWXML_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWXML_EXPORTS
    #define FWXML_API __declspec(dllexport)
    #define FWXML_TEMPLATE_API
    #else
    #define FWXML_API __declspec(dllimport)
    #define FWXML_TEMPLATE_API extern "C++"
    #endif

    #define FWXML_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4267) // disable warning C4267: conversion from 'size_t' to 'type', possible loss of data
    #pragma warning(disable: 4800) // disable warning C4800: 'const int' : forcing value to bool 'true' or 'false' (performance warning)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWXML_EXPORTS
    #define FWXML_API __attribute__ ((visibility("default")))
    #define FWXML_CLASS_API __attribute__ ((visibility("default")))
    #define FWXML_TEMPLATE_API extern "C++"
    #else
    #define FWXML_API __attribute__ ((visibility("hidden")))
    #define FWXML_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWXML_TEMPLATE_API extern "C++"
    #endif

#else

    #define FWXML_API
    #define FWXML_CLASS_API
    #define FWXML_TEMPLATE_API extern "C++"

#endif

#endif //_FWXML_CONFIG_HPP_
