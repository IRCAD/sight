sight_add_target(io_bitmap TYPE LIBRARY)
target_link_libraries(io_bitmap PUBLIC core data io)

option(SIGHT_ENABLE_NVJPEG "Enable nvJPEG support." OFF)
option(SIGHT_ENABLE_NVJPEG2K "Enable nvJPEG2000 support." OFF)

if(SIGHT_ENABLE_NVJPEG2K)
    set(SIGHT_ENABLE_NVJPEG ON FORCE)
endif()

if(SIGHT_ENABLE_NVJPEG)
    # Search for CUDA as nvJPEG is bundled with it
    if(NOT CUDAToolkit_FOUND)
        find_package(CUDAToolkit)
    endif()

    if(CUDAToolkit_FOUND)

        # We found CUDA, so we have nvJPEG
        target_compile_definitions(io_bitmap PUBLIC SIGHT_ENABLE_NVJPEG)
        target_link_libraries(io_bitmap PRIVATE CUDA::cudart CUDA::nvjpeg CUDA::cuda_driver CUDA::nppidei)

        if(SIGHT_ENABLE_NVJPEG2K)
            # Search for nvJPEG2000
            if(UNIX)
                find_library(
                    NVJPEG2K_LIB NAMES nvjpeg2k_static PATH_SUFFIXES "libnvjpeg2k/${CUDAToolkit_VERSION_MAJOR}"
                )
            else()
                find_library(NVJPEG2K_LIB NAMES nvjpeg2k)
            endif()

            if("${NVJPEG2K_LIB}" STREQUAL "" OR "${NVJPEG2K_LIB}" STREQUAL "NVJPEG2K_LIB-NOTFOUND")
                message(FATAL_ERROR "nvjpeg2k library was not found.")
            else()

                target_compile_definitions(io_bitmap PUBLIC SIGHT_ENABLE_NVJPEG2K)
                message("-- Found nvJPEG2000: ${NVJPEG2K_LIB}")
                target_link_libraries(io_bitmap PRIVATE "${NVJPEG2K_LIB}")

                if(UNIX)
                    find_file(NVJPEG2K_HEADER NAMES nvjpeg2k.h PATH_SUFFIXES "libnvjpeg2k/${CUDAToolkit_VERSION_MAJOR}"
                                                                             REQUIRED
                    )
                    cmake_path(GET NVJPEG2K_HEADER PARENT_PATH NVJPEG2K_INCLUDE_DIR)
                    target_include_directories(io_bitmap PRIVATE "${NVJPEG2K_INCLUDE_DIR}")
                endif()
            endif()
        endif()
    else()
        message(FATAL_ERROR "CUDA library was not found.")
    endif()
endif()

# Use libjpeg-turbo as a fallback
find_package(JPEG QUIET REQUIRED)
target_link_libraries(io_bitmap PRIVATE JPEG::JPEG)

# Use libtiff as fast lossless cpu alternative
find_package(TIFF QUIET REQUIRED)
target_link_libraries(io_bitmap PRIVATE TIFF::TIFF)

# Use libPNG for good mixed capability and maximum compatibility
find_package(PNG QUIET REQUIRED)
target_link_libraries(io_bitmap PRIVATE PNG::PNG)

# Use openJPEG as a fallback for JPEG2K
find_package(OpenJPEG QUIET REQUIRED)
target_link_libraries(io_bitmap PRIVATE openjp2)

# This is needed on Linux because the OpenJPEGConfig.cmake seems to be faulty...
target_include_directories(io_bitmap PRIVATE ${OPENJPEG_INCLUDE_DIRS})

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
