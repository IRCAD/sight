/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IOXML_CONFIG_HPP_
#define IOXML_CONFIG_HPP_


#ifdef _WIN32

    #ifdef IOXML_EXPORTS
    #define IOXML_API __declspec(dllexport)
    #else
    #define IOXML_API __declspec(dllimport)
    #endif

    #define IOXML_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOXML_EXPORTS
    #define IOXML_API __attribute__ ((visibility("default")))
    #define IOXML_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOXML_API __attribute__ ((visibility("hidden")))
    #define IOXML_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOXML_API
    #define IOXML_CLASS_API

#endif

#endif //IOXML_CONFIG_HPP_
