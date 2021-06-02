sight_add_target( module_filter_image TYPE MODULE )

# Necessary only because of SPlaneSlicer, can be removed if rewritten as a visualisation adaptor
find_package(VTK QUIET REQUIRED COMPONENTS vtkImagingCore)

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    target_include_directories(module_filter_image SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(module_filter_image PUBLIC vtkImagingCore)
else()
    target_link_libraries(module_filter_image PUBLIC VTK::ImagingCore)
endif()

target_link_libraries(module_filter_image PUBLIC 
                      data
                      service
                      filter_image
                      io_vtk
                      ui_base
                      ui_history
)
