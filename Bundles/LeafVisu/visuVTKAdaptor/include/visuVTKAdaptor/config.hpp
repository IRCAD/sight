/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_CONFIG_HPP_
#define _VISUVTKADAPTOR_CONFIG_HPP_


#ifdef _WIN32

    #ifdef VISUVTKADAPTOR_EXPORTS
    #define VISUVTKADAPTOR_API __declspec(dllexport)
    #else
    #define VISUVTKADAPTOR_API __declspec(dllimport)
    #endif

    #define VISUVTKADAPTOR_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef VISUVTKADAPTOR_EXPORTS
    #define VISUVTKADAPTOR_API __attribute__ ((visibility("default")))
    #define VISUVTKADAPTOR_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define VISUVTKADAPTOR_API __attribute__ ((visibility("hidden")))
    #define VISUVTKADAPTOR_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define VISUVTKADAPTOR_API
    #define VISUVTKADAPTOR_CLASS_API

#endif

#endif // _VISUVTKADAPTOR_CONFIG_HPP_
