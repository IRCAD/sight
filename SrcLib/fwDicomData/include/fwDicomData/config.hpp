/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _FWDICOMDATA_CONFIG_HPP_
#define _FWDICOMDATA_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWDICOMDATA_EXPORTS
            #define FWDICOMDATA_API __declspec(dllexport)
        #else
            #define FWDICOMDATA_API __declspec(dllimport)
        #endif

        #define FWDICOMDATA_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWDICOMDATA_EXPORTS
            #define FWDICOMDATA_API __attribute__ ((visibility("default")))
            #define FWDICOMDATA_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWDICOMDATA_API __attribute__ ((visibility("hidden")))
            #define FWDICOMDATA_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWDICOMDATA_API
        #define FWDICOMDATA_CLASS_API

    #endif

#endif //FWDICOMDATA_API

