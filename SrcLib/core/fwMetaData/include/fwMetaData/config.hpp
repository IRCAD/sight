/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */
 

#ifndef _FWMETADATA_CONFIG_HPP_
#define _FWMETADATA_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWMETADATA_EXPORTS
            #define FWMETADATA_API __declspec(dllexport)
        #else
            #define FWMETADATA_API __declspec(dllimport)
        #endif

        #define FWMETADATA_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWMETADATA_EXPORTS
            #define FWMETADATA_API __attribute__ ((visibility("default")))
            #define FWMETADATA_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWMETADATA_API __attribute__ ((visibility("hidden")))
            #define FWMETADATA_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWMETADATA_API
        #define FWMETADATA_CLASS_API

    #endif

#endif //FWMETADATA_API

