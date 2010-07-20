#ifndef _MINIZIP_CONFIG_HPP_
#define _MINIZIP_CONFIG_HPP_

//TODO: minizip/microzip as external dependency
#define MINIZIP_CLASS_API
#define MINIZIP_API

//#ifdef _WIN32

    //#ifdef MINIZIP_EXPORTS
    //#define MINIZIP_API __declspec(dllexport)
    //#else
    //#define MINIZIP_API __declspec(dllimport)
    //#endif

    //#define MINIZIP_CLASS_API

    //#pragma warning(disable: 4290)

//#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    //#ifdef MINIZIP_EXPORTS
    //#define MINIZIP_API __attribute__ ((visibility("default")))
    //#define MINIZIP_CLASS_API __attribute__ ((visibility("default")))
    //#else
    //#define MINIZIP_API
    //#define MINIZIP_CLASS_API
    //#endif

//#else

    //#define MINIZIP_API
    //#define MINIZIP_CLASS_API

//#endif

#endif //_MINIZIP_CONFIG_HPP_
