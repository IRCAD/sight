sight_add_target( io_vtk TYPE LIBRARY PCH pchData )

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    target_include_directories(io_vtk SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(io_vtk PUBLIC  
        vtkFiltersGeometry
        vtkFiltersModeling
        vtkIOExport
        vtkIOGeometry
        vtkIOImage
        vtkIOLegacy
        vtkIOPLY
        vtkIOXML
        vtkRenderingFreeType
        vtkRenderingOpenGL2
    )

    #add vtk definitions instead of include(${VTK_USE_FILE})
    target_compile_definitions(io_vtk PRIVATE ${VTK_DEFINITIONS})
else()
    target_link_libraries(io_vtk PUBLIC
        VTK::FiltersGeometry
        VTK::FiltersModeling
        VTK::IOExport
        VTK::IOGeometry
        VTK::IOImage
        VTK::IOLegacy
        VTK::IOPLY
        VTK::IOXML
        VTK::RenderingFreeType
        VTK::RenderingOpenGL2 
    )
endif()

target_link_libraries(io_vtk PUBLIC core data io_base geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
