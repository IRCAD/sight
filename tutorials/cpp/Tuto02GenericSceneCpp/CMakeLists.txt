sight_add_target( Tuto02GenericSceneCpp TYPE APP START ON WARNINGS_AS_ERRORS ON )


add_dependencies(${FWPROJECT_NAME} 
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_matrix
                 module_io_vtk
                 module_io_atoms
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
)
