# Conan support
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.12/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.0.0 REQUIRED)
conan_add_remote(NAME fw4spl-conan INDEX 1
                 URL http://5.39.78.163:8081/artifactory/api/conan/conan-local)

macro(findConanDeps PROJECT_LIST CONAN_DEPS_LIST)
    unset(RESULT_LIST)

    foreach(PROJECT ${PROJECT_LIST})
        if(${PROJECT}_CONAN_DEPS)
            list(APPEND RESULT_LIST ${${PROJECT}_CONAN_DEPS})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES RESULT_LIST)
    set(CONAN_DEPS_LIST ${RESULT_LIST})
endmacro()

macro(installConanDeps CONAN_DEPS_LIST)
    set (CONAN_IMPORTS
      "bin, *.dll -> ./bin"
      "lib, *.dylib -> ./lib"
      "lib, *.so* -> ./lib"
   )
    conan_cmake_run(
        REQUIRES ${CONAN_DEPS_LIST}
        BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
        IMPORTS ${CONAN_IMPORTS}
    )
endmacro()
