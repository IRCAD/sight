/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_CONFIG_HPP_
#define _FWITKIO_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWITKIO_EXPORTS
    #define FWITKIO_API __declspec(dllexport)
    #else
    #define FWITKIO_API __declspec(dllimport)
    #endif

    #define FWITKIO_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWITKIO_EXPORTS
    #define FWITKIO_API __attribute__ ((visibility("default")))
    #define FWITKIO_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define FWITKIO_API __attribute__ ((visibility("hidden")))
    #define FWITKIO_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define FWITKIO_API
    #define FWITKIO_CLASS_API

#endif

#endif //FWITKIO_API
