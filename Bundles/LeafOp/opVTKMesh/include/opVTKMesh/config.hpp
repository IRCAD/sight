/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPVTKMESH_CONFIG_HPP_
#define _OPVTKMESH_CONFIG_HPP_


#ifdef _WIN32

        #ifdef OPVTKMESH_EXPORTS
        #define OPVTKMESH_API __declspec(dllexport)
        #else
        #define OPVTKMESH_API __declspec(dllimport)
        #endif

        #define OPVTKMESH_CLASS_API

        #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef OPVTKMESH_EXPORTS
        #define OPVTKMESH_API __attribute__ ((visibility("default")))
        #define OPVTKMESH_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define OPVTKMESH_API __attribute__ ((visibility("hidden")))
        #define OPVTKMESH_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

#else

        #define OPVTKMESH_API
        #define OPVTKMESH_CLASS_API

#endif

#endif //_OPVTKMESH_CONFIG_HPP_
