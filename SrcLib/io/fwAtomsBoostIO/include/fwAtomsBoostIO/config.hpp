/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2012.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _FWATOMSBOOSTIO_CONFIG_HPP_
#define _FWATOMSBOOSTIO_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWATOMSBOOSTIO_EXPORTS
            #define FWATOMSBOOSTIO_API __declspec(dllexport)
        #else
            #define FWATOMSBOOSTIO_API __declspec(dllimport)
        #endif

        #define FWATOMSBOOSTIO_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWATOMSBOOSTIO_EXPORTS
            #define FWATOMSBOOSTIO_API __attribute__ ((visibility("default")))
            #define FWATOMSBOOSTIO_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWATOMSBOOSTIO_API __attribute__ ((visibility("hidden")))
            #define FWATOMSBOOSTIO_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWATOMSBOOSTIO_API
        #define FWATOMSBOOSTIO_CLASS_API

    #endif

#endif //FWATOMSBOOSTIO_API

