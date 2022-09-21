# Replace export macros like the preprocessor would do to avoid conflicts with client repositories
# Indeed, if two targets have the same name (i.e. data), the macro (here DATA_API) would be defined twice with different
# values (__declspec(dllimport)/__declspec(dllexport))
file(GLOB_RECURSE INSTALLED_HEADERS FOLLOW_SYMLINKS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/*.hpp"
     "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/*.h"
     "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/*.hxx"
)

message(STATUS "Post-processing header files for export macros...")

foreach(HEADER ${INSTALLED_HEADERS})
    file(READ "${HEADER}" HEADER_CONTENT)
    if(${HEADER} MATCHES "/config.hpp")
        string(REGEX REPLACE "#pragma once" "#pragma once\n\n#if 0" HEADER_CONTENT "${HEADER_CONTENT}")
        set(HEADER_CONTENT "${HEADER_CONTENT}#endif\n")
    else()
        if("${HEADER_CONTENT}" MATCHES "QT_CLASS")
            message("${HEADER}: QT_QT   ")
            set(QT_API_SUFFIX "QT_QT_API")
        else()
            set(QT_API_SUFFIX "QT_API")
        endif()
        string(REGEX REPLACE "[_A-Z0-9]*_CLASS_QT_API ?" "__declspec(dllimport) " HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_DEPRECATED_CLASS_API ?" "[[deprecated]] " HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_CLASS_API ?" "" HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_${QT_API_SUFFIX} ?" "" HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_API ?" "__declspec(dllimport) " HEADER_CONTENT "${HEADER_CONTENT}")
    endif()
    file(WRITE "${HEADER}" "${HEADER_CONTENT}")
endforeach()

message(STATUS "Done...")
