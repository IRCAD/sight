/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
 

#ifndef _FWCOMMAND_CONFIG_HPP_
#define _FWCOMMAND_CONFIG_HPP_

    #ifdef _WIN32

        #ifdef FWCOMMAND_EXPORTS
            #define FWCOMMAND_API __declspec(dllexport)
        #else
            #define FWCOMMAND_API __declspec(dllimport)
        #endif

        #define FWCOMMAND_CLASS_API

        #pragma warning(disable: 4290)

    #elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef FWCOMMAND_EXPORTS
            #define FWCOMMAND_API __attribute__ ((visibility("default")))
            #define FWCOMMAND_CLASS_API __attribute__ ((visibility("default")))
        #else
            #define FWCOMMAND_API __attribute__ ((visibility("hidden")))
            #define FWCOMMAND_CLASS_API __attribute__ ((visibility("hidden")))
        #endif

    #else

        #define FWCOMMAND_API
        #define FWCOMMAND_CLASS_API

    #endif

#endif //FWCOMMAND_API

