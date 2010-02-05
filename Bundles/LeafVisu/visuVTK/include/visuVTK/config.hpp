/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VISUVTK_CONFIG_HPP_
#define VISUVTK_CONFIG_HPP_


#ifdef _WIN32

    #ifdef VISUVTK_EXPORTS
    #define VISUVTK_API __declspec(dllexport)
    #else
    #define VISUVTK_API __declspec(dllimport)
    #endif

    #define VISUVTK_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef VISUVTK_EXPORTS
    #define VISUVTK_API __attribute__ ((visibility("default")))
    #define VISUVTK_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define VISUVTK_API __attribute__ ((visibility("hidden")))
    #define VISUVTK_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define VISUVTK_API
    #define VISUVTK_CLASS_API

#endif

#endif //VISUVTK_CONFIG_HPP_
