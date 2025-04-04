sight_add_target(io_session TYPE LIBRARY OBJECT_LIBRARY ON)

if(MSVC)
    find_package(NIFTI COMPONENTS nifti2 niftiio REQUIRED)
    target_link_libraries(${SIGHT_TARGET}_obj PRIVATE NIFTI::niftiio NIFTI::nifti2)
else()
    # NIFTI official cmake configuration files from Debian/Ubuntu are broken: they reference the wrong path
    # "/usr/lib/XXX" instead of "/usr/lib/x86_64-linux-gnu/XXX", making cmake to fail at the configuration stage.
    # To workaround that, we link the library manually on that platform.
    # This should be removed once the cmake files are fixed.
    target_link_libraries(${SIGHT_TARGET}_obj PRIVATE -lniftiio -lnifti2 -lznz)
    target_include_directories(${SIGHT_TARGET}_obj PRIVATE SYSTEM /usr/include/nifti)
endif()

target_link_libraries(${SIGHT_TARGET}_obj PUBLIC io io_zip io_vtk io_dicom)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
