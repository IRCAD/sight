# Conan support
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.12/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.0.0 REQUIRED)
conan_add_remote(NAME sight-conan INDEX 1
                 URL http://5.39.78.163:8081/artifactory/api/conan/conan-local)

unset(CONAN_OPTIONS)
find_package(CUDA QUIET)
if(CUDA_FOUND)
    message(STATUS "CUDA ${CUDA_VERSION_STRING} found on your PC. Cuda Conan packages will be used")
    set(CONAN_OPTIONS "*:use_cuda=True")
endif()

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

    conan_cmake_run(
        REQUIRES ${CONAN_DEPS_LIST}
        BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
        OPTIONS ${CONAN_OPTIONS}
        #BUILD missing
    )

endmacro()

macro(installConanDepsForSDK CONAN_DEPS_LIST)

    foreach(CONAN_DEP ${CONAN_DEPENDENCIES})
        string(REGEX REPLACE "([^\/]*)/.*" "\\1" CONAN_REQUIREMENT ${CONAN_DEP})
        string(TOUPPER ${CONAN_REQUIREMENT} CONAN_REQUIREMENT )
        install(DIRECTORY "${CONAN_${CONAN_REQUIREMENT}_ROOT}/" DESTINATION ${CMAKE_INSTALL_PREFIX})
    endforeach()

endmacro()
