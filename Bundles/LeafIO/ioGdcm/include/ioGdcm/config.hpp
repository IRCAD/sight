#ifndef _IOGDCM_CONFIG_HPP_
#define _IOGDCM_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IOGDCM_EXPORTS
    #define IOGDCM_API __declspec(dllexport)
    #else
    #define IOGDCM_API __declspec(dllimport)
    #endif

    #define IOGDCM_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOGDCM_EXPORTS
    #define IOGDCM_API __attribute__ ((visibility("default")))
    #define IOGDCM_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOGDCM_API __attribute__ ((visibility("hidden")))
    #define IOGDCM_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOGDCM_API
    #define IOGDCM_CLASS_API

#endif

#endif // #ifndef _IOGDCM_CONFIG_HPP_
