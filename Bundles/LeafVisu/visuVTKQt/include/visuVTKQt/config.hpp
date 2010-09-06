/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VISUVTKQT_CONFIG_HPP_
#define VISUVTKQT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef VISUVTKQT_EXPORTS
    #define VISUVTKQT_API __declspec(dllexport)
    #else
    #define VISUVTKQT_API __declspec(dllimport)
    #endif

    #define VISUVTKQT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef VISUVTKQT_EXPORTS
    #define VISUVTKQT_API __attribute__ ((visibility("default")))
    #define VISUVTKQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define VISUVTKQT_API __attribute__ ((visibility("hidden")))
    #define VISUVTKQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define VISUVTKQT_API
    #define VISUVTKQT_CLASS_API

#endif

#endif //VISUVTKQT_CONFIG_HPP_
