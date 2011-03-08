/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _TUTOTRIANCONVERTERCTRL_CONFIG_HPP_
#define _TUTOTRIANCONVERTERCTRL_CONFIG_HPP_

#ifdef _WIN32

    #ifdef TUTOTRIANCONVERTERCTRL_EXPORTS
    #define TUTOTRIANCONVERTERCTRL_API __declspec(dllexport)
    #else
    #define TUTOTRIANCONVERTERCTRL_API __declspec(dllimport)
    #endif

    #define TUTOTRIANCONVERTERCTRL_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef TUTOTRIANCONVERTERCTRL_EXPORTS
    #define TUTOTRIANCONVERTERCTRL_API __attribute__ ((visibility("default")))
    #define TUTOTRIANCONVERTERCTRL_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define TUTOTRIANCONVERTERCTRL_API __attribute__ ((visibility("hidden")))
    #define TUTOTRIANCONVERTERCTRL_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define TUTOTRIANCONVERTERCTRL_API
    #define TUTOTRIANCONVERTERCTRL_CLASS_API

#endif

#endif // _TUTOTRIANCONVERTERCTRL_CONFIG_HPP_
