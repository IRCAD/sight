sight_add_target(module_filter_image TYPE MODULE)

# On windows for a strange reason the VXL "core" directory (needed by itkArray.h) is not exported by ITK
# Mark it as a private system directory to avoid putting hardcoded paths in .cmake files
if(MSVC)
    find_package(VXL QUIET REQUIRED)
    target_include_directories(module_filter_image SYSTEM PRIVATE ${VXL_CORE_INCLUDE_DIRS})
endif()

# Necessary only because of SPlaneSlicer, can be removed if rewritten as a visualisation adaptor
find_package(VTK QUIET REQUIRED COMPONENTS vtkImagingCore)

target_link_libraries(module_filter_image PUBLIC VTK::ImagingCore)

target_link_libraries(module_filter_image PUBLIC data service filter_image io_vtk ui ui_history)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
