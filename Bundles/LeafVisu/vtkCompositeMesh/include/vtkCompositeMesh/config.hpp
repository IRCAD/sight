/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef VTKCOMPOSITEMESH_CONFIG_HPP_
#define VTKCOMPOSITEMESH_CONFIG_HPP_


#ifdef _WIN32

        #ifdef VTKCOMPOSITEMESH_EXPORTS
        #define VTKCOMPOSITEMESH_API __declspec(dllexport)
        #else
        #define VTKCOMPOSITEMESH_API __declspec(dllimport)
        #endif

        #define VTKCOMPOSITEMESH_CLASS_API

        #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef VTKCOMPOSITEMESH_EXPORTS
        #define VTKCOMPOSITEMESH_API __attribute__ ((visibility("default")))
        #define VTKCOMPOSITEMESH_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define VTKCOMPOSITEMESH_API __attribute__ ((visibility("hidden")))
        #define VTKCOMPOSITEMESH_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

#else

        #define VTKCOMPOSITEMESH_API
        #define VTKCOMPOSITEMESH_CLASS_API

#endif

#endif //VTKCOMPOSITEMESH_CONFIG_HPP_
