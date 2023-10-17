# Replace export macros like the preprocessor would do to avoid conflicts with client repositories
# Indeed, if two targets have the same name (i.e. data), the macro (here DATA_API) would be defined twice with different
# values (__declspec(dllimport)/__declspec(dllexport))

# Such a pity, but it seems there is no other way to pass arguments and protect their content (spaces in paths for
# instance) than using double-quotes, BUT, for some reason, we still have these double quotes in the local variables
# and we have to strip them out manually...
string(REPLACE "\"" "" SOURCE_DIR ${SOURCE_DIR})
string(REPLACE "\"" "" CONFIG_SOURCE_DIR ${CONFIG_SOURCE_DIR})
string(REPLACE "\"" "" TARGET_DIR ${TARGET_DIR})
string(REPLACE "\"" "" HEADERS "${HEADERS}")

# Reverse the hack used to pass a list as argument...
string(REPLACE "," ";" HEADERS_REMAKE "${HEADERS}")

# Disable the config.hpp
set(CONFIG_HEADER "${CONFIG_SOURCE_DIR}/config.hpp")

file(READ "${CONFIG_HEADER}" HEADER_CONTENT)
string(REGEX REPLACE "#pragma once" "#pragma once\n\n#if 0" HEADER_CONTENT "${HEADER_CONTENT}")
set(HEADER_CONTENT "${HEADER_CONTENT}#endif\n")

cmake_path(RELATIVE_PATH CONFIG_HEADER BASE_DIRECTORY ${CONFIG_SOURCE_DIR})
file(WRITE "${TARGET_DIR}/${CONFIG_HEADER}" "${HEADER_CONTENT}")

# Substitute _API macros in all regular headers
foreach(HEADER ${HEADERS_REMAKE})
    file(READ "${HEADER}" HEADER_CONTENT)

    set(QT_API_SUFFIX "API_QT")
    string(REGEX REPLACE "[_A-Z0-9]*_CLASS_API_QT ?" "__declspec(dllimport) " HEADER_CONTENT "${HEADER_CONTENT}")
    string(REGEX REPLACE "[_A-Z0-9]*_DEPRECATED_CLASS_API ?" "[[deprecated]] " HEADER_CONTENT "${HEADER_CONTENT}")
    string(REGEX REPLACE "[_A-Z0-9]*_CLASS_API ?" "" HEADER_CONTENT "${HEADER_CONTENT}")
    string(REGEX REPLACE "[_A-Z0-9]*_${QT_API_SUFFIX} ?" "" HEADER_CONTENT "${HEADER_CONTENT}")
    string(REGEX REPLACE "[_A-Z0-9]*_API ?" "__declspec(dllimport) " HEADER_CONTENT "${HEADER_CONTENT}")

    cmake_path(RELATIVE_PATH HEADER BASE_DIRECTORY ${SOURCE_DIR})

    file(WRITE "${TARGET_DIR}/${HEADER}" "${HEADER_CONTENT}")
endforeach()
