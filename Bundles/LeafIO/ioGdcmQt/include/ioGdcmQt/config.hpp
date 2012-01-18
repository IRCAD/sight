#ifndef _IOGDCMQT_CONFIG_HPP_
#define _IOGDCMQT_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef IOGDCMQT_EXPORTS
    #define IOGDCMQT_API __declspec(dllexport)
    #else
    #define IOGDCMQT_API __declspec(dllimport)
    #endif

    #define IOGDCMQT_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef IOGDCMQT_EXPORTS
    #define IOGDCMQT_API __attribute__ ((visibility("default")))
    #define IOGDCMQT_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define IOGDCMQT_API __attribute__ ((visibility("hidden")))
    #define IOGDCMQT_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define IOGDCMQT_API
    #define IOGDCMQT_CLASS_API

#endif

#endif // #ifndef _IOGDCMQT_CONFIG_HPP_
