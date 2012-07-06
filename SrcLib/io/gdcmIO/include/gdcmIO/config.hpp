#ifndef _GDCMIO_CONFIG_HPP_
#define _GDCMIO_CONFIG_HPP_


#ifdef _WIN32

    #ifdef GDCMIO_EXPORTS
    #define GDCMIO_API __declspec(dllexport)
    #define GDCMIO_TEMPLATE_API
    #else
    #define GDCMIO_API __declspec(dllimport)
    #define GDCMIO_TEMPLATE_API extern
    #endif

    #define GDCMIO_CLASS_API

    #pragma warning(disable: 4290)
    #pragma warning(disable: 4800) // avoid waring on return hard coded value

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

        #ifdef GDCMIO_EXPORTS
        #define GDCMIO_API __attribute__ ((visibility("default")))
        #define GDCMIO_CLASS_API __attribute__ ((visibility("default")))
        #define GDCMIO_TEMPLATE_API
        #else
        //#define GDCMIO_API __attribute__ ((visibility("hidden")))
        //#define GDCMIO_CLASS_API __attribute__ ((visibility("hidden")))
        #define GDCMIO_API
        #define GDCMIO_CLASS_API
        #define GDCMIO_TEMPLATE_API extern
        #endif

#else

        #define GDCMIO_API
        #define GDCMIO_CLASS_API
        #define GDCMIO_TEMPLATE_API

#endif


#endif //_GDCMIO_CONFIG_HPP_

