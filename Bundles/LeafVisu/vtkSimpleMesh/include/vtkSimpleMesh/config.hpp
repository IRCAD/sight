/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKSIMPLEMESH_CONFIG_HPP_
#define VTKSIMPLEMESH_CONFIG_HPP_


#ifdef _WIN32

        #ifdef VTKSIMPLEMESH_EXPORTS
        #define VTKSIMPLEMESH_API __declspec(dllexport)
        #else
        #define VTKSIMPLEMESH_API __declspec(dllimport)
        #endif

        #define VTKSIMPLEMESH_CLASS_API

        #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef VTKSIMPLEMESH_EXPORTS
        #define VTKSIMPLEMESH_API __attribute__ ((visibility("default")))
        #define VTKSIMPLEMESH_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define VTKSIMPLEMESH_API __attribute__ ((visibility("hidden")))
        #define VTKSIMPLEMESH_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

#else

        #define VTKSIMPLEMESH_API
        #define VTKSIMPLEMESH_CLASS_API

#endif

#endif //VTKSIMPLEMESH_CONFIG_HPP_
