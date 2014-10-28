/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOATOMS_CONFIG_HPP_
#define _IOATOMS_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IOATOMS_EXPORTS
    #define IOATOMS_API __declspec(dllexport)
    #else
    #define IOATOMS_API __declspec(dllimport)
    #endif

    #define IOATOMS_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOATOMS_EXPORTS
    #define IOATOMS_API __attribute__ ((visibility("default")))
    #define IOATOMS_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOATOMS_API __attribute__ ((visibility("hidden")))
    #define IOATOMS_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOATOMS_API
    #define IOATOMS_CLASS_API

#endif

#endif // #ifndef _IOATOMS_CONFIG_HPP_
