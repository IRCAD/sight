/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */


#ifndef _FWACTIVITIES_CONFIG_HPP_
#define _FWACTIVITIES_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWACTIVITIES_EXPORTS
            #define FWACTIVITIES_API __declspec(dllexport)
        #else
            #define FWACTIVITIES_API __declspec(dllimport)
        #endif

        #define FWACTIVITIES_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWACTIVITIES_EXPORTS
            #define FWACTIVITIES_API __attribute__ ((visibility("default")))
            #define FWACTIVITIES_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWACTIVITIES_API __attribute__ ((visibility("hidden")))
            #define FWACTIVITIES_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWACTIVITIES_API
        #define FWACTIVITIES_CLASS_API

    #endif

#endif //FWACTIVITIES_API

