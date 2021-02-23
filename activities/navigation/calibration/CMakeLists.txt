fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 module_ui_base
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_activity
                 module_activity
                 module_service
                 data
                 module_ui_base
                 module_ui_icons
                 module_ui_qt
                 module_io_matrix
                 module_io_vision
                 module_io_video
                 module_geometry_vision
                 module_sync
                 module_geometry_vision
                 module_data
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      activity
                      core
                      data
)
