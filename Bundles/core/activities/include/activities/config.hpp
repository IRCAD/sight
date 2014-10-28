#ifndef ACTIVITIES_EXPORT_HPP_
#define ACTIVITIES_EXPORT_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )

    #ifdef ACTIVITIES_EXPORTS
    #define ACTIVITIES_API  __declspec(dllexport)
    #else
    #define ACTIVITIES_API __declspec(dllimport)
    #endif

    #define ACTIVITIES_CLASS_API

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef ACTIVITIES_EXPORTS
    #define ACTIVITIES_API __attribute__ ((visibility("default")))
    #define ACTIVITIES_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define ACTIVITIES_API __attribute__ ((visibility("hidden")))
    #define ACTIVITIES_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define ACTIVITIES_API
    #define ACTIVITIES_CLASS_API

#endif

#endif //ACTIVITIES_EXPORT_HPP_
