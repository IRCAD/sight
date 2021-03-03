sight_add_target( Tuto01DataServiceBasicCpp TYPE APP START ON WARNINGS_AS_ERRORS ON )


add_dependencies(${FWPROJECT_NAME} 
                 sightrun
                 module_ui_qt
                 module_service
                 module_ui_base
                 module_io_vtk
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
)

sight_generate_profile(${FWPROJECT_NAME})
