#ifndef _OPKINECT_CONFIG_HPP_
#define _OPKINECT_CONFIG_HPP_


#ifdef _WIN32

    #ifdef OPKINECT_EXPORTS
    #define OPKINECT_API __declspec(dllexport)
    #else
    #define OPKINECT_API __declspec(dllimport)
    #endif

    #define OPKINECT_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef OPKINECT_EXPORTS
    #define OPKINECT_API __attribute__ ((visibility("default")))
    #define OPKINECT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define OPKINECT_API __attribute__ ((visibility("hidden")))
    #define OPKINECT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define OPKINECT_API
    #define OPKINECT_CLASS_API

#endif

#endif //_OPKINECT_CONFIG_HPP_
