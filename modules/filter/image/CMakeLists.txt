sight_add_target(module_filter_image TYPE MODULE)

# Necessary only because of SPlaneSlicer, can be removed if rewritten as a visualisation adaptor
find_package(VTK QUIET REQUIRED COMPONENTS vtkImagingCore)

target_link_libraries(module_filter_image PUBLIC VTK::ImagingCore)

target_link_libraries(module_filter_image PUBLIC data service filter_image io_vtk ui ui_history)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
