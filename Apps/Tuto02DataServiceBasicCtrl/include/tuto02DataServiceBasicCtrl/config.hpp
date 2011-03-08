/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _TUTO02DATASERVICEBASICCTRL_CONFIG_HPP_
#define _TUTO02DATASERVICEBASICCTRL_CONFIG_HPP_

#ifdef _WIN32

    #ifdef TUTO02DATASERVICEBASICCTRL_EXPORTS
    #define TUTO02DATASERVICEBASICCTRL_API __declspec(dllexport)
    #else
    #define TUTO02DATASERVICEBASICCTRL_API __declspec(dllimport)
    #endif

    #define TUTO02DATASERVICEBASICCTRL_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef TUTO02DATASERVICEBASICCTRL_EXPORTS
    #define TUTO02DATASERVICEBASICCTRL_API __attribute__ ((visibility("default")))
    #define TUTO02DATASERVICEBASICCTRL_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define TUTO02DATASERVICEBASICCTRL_API __attribute__ ((visibility("hidden")))
    #define TUTO02DATASERVICEBASICCTRL_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define TUTO02DATASERVICEBASICCTRL_API
    #define TUTO02DATASERVICEBASICCTRL_CLASS_API

#endif

#endif // _TUTO02DATASERVICEBASICCTRL_CONFIG_HPP_
