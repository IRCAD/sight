#include <cassert>

#ifndef _FWXML_CONFIG_HPP_
#define _FWXML_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWXML_EXPORTS
    #define FWXML_API __declspec(dllexport)
    #define FWXML_TEMPLATE_API
    #else
    #define FWXML_API __declspec(dllimport)
    #define FWXML_TEMPLATE_API extern "C++"
    #endif

    #define FWXML_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4267) // disable warning 'argument'�: conversion de 'size_t' en 'unsigned int', perte possible de donn�es
    #pragma warning(disable: 4800) // disable warning C4800: 'const int'�: valeur forc�e � la valeur bool�enne 'true' ou 'false' (avertissement sur les performances)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWXML_EXPORTS
    #define FWXML_API __attribute__ ((visibility("default")))
    #define FWXML_CLASS_API __attribute__ ((visibility("default")))
    #define FWXML_TEMPLATE_API extern "C++"
    #else
    #define FWXML_API __attribute__ ((visibility("hidden")))
    #define FWXML_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWXML_TEMPLATE_API extern "C++"
    #endif

#else

    #define FWXML_API
    #define FWXML_CLASS_API
    #define FWXML_TEMPLATE_API extern "C++"

#endif

#endif //_FWXML_CONFIG_HPP_
