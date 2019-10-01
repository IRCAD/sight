# Conan support
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.14/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.0.0 REQUIRED)
conan_add_remote(NAME sight INDEX 1
                 URL https://conan.ircad.fr/artifactory/api/conan/sight)

unset(CONAN_OPTIONS)
find_package(CUDA QUIET)
if(CUDA_FOUND)
    set(CONAN_OPTIONS "*:cuda=${CUDA_VERSION_STRING}")
    message(STATUS "CUDA ${CUDA_VERSION_STRING} found on your PC. Cuda Conan packages will be used.\n"
                   "Conan options used: ${CONAN_OPTIONS}")
else()
    set(CONAN_OPTIONS "*:cuda=None")
    message(STATUS "CUDA not found on your PC. Cuda Conan packages will be NOT used.\n"
                   "Conan options used: ${CONAN_OPTIONS}")
endif()

macro(findConanDeps PROJECT_LIST CONAN_DEPS_LIST)
    unset(RESULT_LIST)

    foreach(PROJECT ${PROJECT_LIST})
        if(${PROJECT}_CONAN_DEPS)
            list(APPEND RESULT_LIST ${${PROJECT}_CONAN_DEPS})
        endif()
    endforeach()

    # Also add the common conan package which contains common.py used for sharing C flags across conan packages
    list(APPEND RESULT_LIST ${CONAN_COMMON})

    list(REMOVE_DUPLICATES RESULT_LIST)
    set(CONAN_DEPS_LIST ${RESULT_LIST})
endmacro()

macro(installConanDeps CONAN_DEPS_LIST)

    if(CONAN_BUILD_MISSING)
        set(CONAN_BUILD_OPTION "missing")
    else()
        set(CONAN_BUILD_OPTION "never")
    endif()

    if(UNIX AND NOT APPLE AND NOT CONAN_SETTINGS)
        # Install conan custom settings
        execute_process(COMMAND ${CONAN_CMD} config install https://git.ircad.fr/conan/conan-config.git --type git --verify-ssl=False
                    RESULT_VARIABLE RETURN_CODE
                    OUTPUT_VARIABLE CONAN_VERSION_OUTPUT
                    ERROR_VARIABLE CONAN_VERSION_OUTPUT)
        if(NOT "${RETURN_CODE}" STREQUAL "0")
            message(FATAL_ERROR "Error in conan settings configuration: ${CONAN_VERSION_OUTPUT}")
        else()
            message(STATUS "conan settings installed: ${CONAN_VERSION_OUTPUT}")
        endif()

        # Find information about current Linux distribution to configure conan
        find_program(LSB_RELEASE_EXEC lsb_release)
        if(NOT LSB_RELEASE_EXEC)
            message(FATAL_ERROR "lsb_release executable not found!")
        endif()
        execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
            OUTPUT_VARIABLE LSB_RELEASE_ID
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(COMMAND ${LSB_RELEASE_EXEC} -rs
            COMMAND cut -d. -f1
            OUTPUT_VARIABLE LSB_MAJOR_RELEASE_NUMBER
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        string(TOLOWER ${LSB_RELEASE_ID} LSB_RELEASE_ID_LOWER)
        set(CONAN_DISTRO "${LSB_RELEASE_ID_LOWER}${LSB_MAJOR_RELEASE_NUMBER}" CACHE INTERNAL "custom conan distro" FORCE)
        set(CONAN_SETTINGS "os.distro=${CONAN_DISTRO}" CACHE INTERNAL "custom conan settings" FORCE)
    endif()
    
    # Force use of c++17 as standard
    if(MSVC)
        list(APPEND CONAN_SETTINGS "compiler.cppstd=17")
    else()
        list(APPEND CONAN_SETTINGS "compiler.cppstd=gnu17")
    endif()

    # Save compiler id in case we trick conan
    set(SAVE_CMAKE_C_COMPILER_ID ${CMAKE_C_COMPILER_ID})
    set(SAVE_CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
    set(SAVE_CMAKE_C_COMPILER_VERSION ${CMAKE_C_COMPILER_VERSION})
    set(SAVE_CMAKE_CXX_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})

    if(CONAN_DISTRO STREQUAL "linuxmint19" AND NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
        include(CheckCXXSourceCompiles)

        # Check stdlib version
        set(CODE "
            #include <bits/c++config.h>
            #if _GLIBCXX_RELEASE < 8
            #error _GLIBCXX_RELEASE < 8
            #endif

            #if _GLIBCXX_USE_CXX11_ABI < 1
            #error _GLIBCXX_USE_CXX11_ABI < 1
            #endif

            int main()
            {
                return 0;
            }
            "
        )

        check_cxx_source_compiles("${CODE}" STDLIB_VERSION_8_SUPPORTED)

        # If we have a supported stdlib, but a different compiler than gcc 8,
        # allow to download and use conan package built with gcc 8
        if(STDLIB_VERSION_8_SUPPORTED)
            # Force use gcc 8 for conan package
            set(CMAKE_C_COMPILER_ID GNU)
            set(CMAKE_CXX_COMPILER_ID GNU)
            set(CMAKE_C_COMPILER_VERSION 8.0)
            set(CMAKE_CXX_COMPILER_VERSION 8.0)
        endif()
    endif()

    conan_cmake_run(
        REQUIRES ${CONAN_DEPS_LIST}
        BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
        OPTIONS ${CONAN_OPTIONS}
        BUILD ${CONAN_BUILD_OPTION}
        SETTINGS ${CONAN_SETTINGS}
    )

    # Restore backup
    set(CMAKE_C_COMPILER_ID ${SAVE_CMAKE_C_COMPILER_ID})
    set(CMAKE_CXX_COMPILER_ID ${SAVE_CMAKE_CXX_COMPILER_ID})
    set(CMAKE_C_COMPILER_VERSION ${SAVE_CMAKE_C_COMPILER_VERSION})
    set(CMAKE_CXX_COMPILER_VERSION ${SAVE_CMAKE_CXX_COMPILER_VERSION})
endmacro()

macro(installConanDepsForSDK)

    foreach(CONAN_DEP ${CONAN_DEPENDENCIES})
        string(REGEX REPLACE "([^\/]*)/.*" "\\1" CONAN_REQUIREMENT ${CONAN_DEP})
        string(TOUPPER ${CONAN_REQUIREMENT} CONAN_REQUIREMENT )
        install(DIRECTORY "${CONAN_${CONAN_REQUIREMENT}_ROOT}/" DESTINATION ".")
    endforeach()

endmacro()
