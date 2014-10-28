/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2012.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _FWATOMSHDF5IO_CONFIG_HPP_
#define _FWATOMSHDF5IO_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWATOMSHDF5IO_EXPORTS
            #define FWATOMSHDF5IO_API __declspec(dllexport)
        #else
            #define FWATOMSHDF5IO_API __declspec(dllimport)
        #endif

        #define FWATOMSHDF5IO_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWATOMSHDF5IO_EXPORTS
            #define FWATOMSHDF5IO_API __attribute__ ((visibility("default")))
            #define FWATOMSHDF5IO_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWATOMSHDF5IO_API __attribute__ ((visibility("hidden")))
            #define FWATOMSHDF5IO_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWATOMSHDF5IO_API
        #define FWATOMSHDF5IO_CLASS_API

    #endif

#endif //FWATOMSHDF5IO_API

