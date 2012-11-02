/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
 

#ifndef _FWATOMS_CONFIG_HPP_
#define _FWATOMS_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWATOMS_EXPORTS
            #define FWATOMS_API __declspec(dllexport)
        #else
            #define FWATOMS_API __declspec(dllimport)
        #endif

        #define FWATOMS_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWATOMS_EXPORTS
            #define FWATOMS_API __attribute__ ((visibility("default")))
            #define FWATOMS_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWATOMS_API __attribute__ ((visibility("hidden")))
            #define FWATOMS_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWATOMS_API
        #define FWATOMS_CLASS_API

    #endif

#endif //FWATOMS_API

