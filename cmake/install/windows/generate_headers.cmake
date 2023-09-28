# Replace export macros like the preprocessor would do to avoid conflicts with client repositories
# Indeed, if two targets have the same name (i.e. data), the macro (here DATA_API) would be defined twice with different
# values (__declspec(dllimport)/__declspec(dllexport))

# Reverse the hack used to pass a list as argument...
string(REPLACE "," ";" HEADERS_REMAKE "${HEADERS}")

foreach(HEADER ${HEADERS_REMAKE})
    file(READ "${HEADER}" HEADER_CONTENT)
    if(${HEADER} MATCHES "/config.hpp")
        string(REGEX REPLACE "#pragma once" "#pragma once\n\n#if 0" HEADER_CONTENT "${HEADER_CONTENT}")
        set(HEADER_CONTENT "${HEADER_CONTENT}#endif\n")
    else()
        set(QT_API_SUFFIX "API_QT")
        string(REGEX REPLACE "[_A-Z0-9]*_CLASS_API_QT ?" "__declspec(dllimport) " HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_DEPRECATED_CLASS_API ?" "[[deprecated]] " HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_CLASS_API ?" "" HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_${QT_API_SUFFIX} ?" "" HEADER_CONTENT "${HEADER_CONTENT}")
        string(REGEX REPLACE "[_A-Z0-9]*_API ?" "__declspec(dllimport) " HEADER_CONTENT "${HEADER_CONTENT}")
    endif()

    get_filename_component(HEADER_FILENAME ${HEADER} NAME)
    if(${HEADER_FILENAME} STREQUAL "config.hpp")
        cmake_path(RELATIVE_PATH HEADER BASE_DIRECTORY ${CONFIG_SOURCE_DIR})
    else()
        cmake_path(RELATIVE_PATH HEADER BASE_DIRECTORY ${SOURCE_DIR})
    endif()
    file(WRITE "${TARGET_DIR}/${HEADER}" "${HEADER_CONTENT}")
endforeach()
