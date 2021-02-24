sight_add_target( ExOpenvslam TYPE APP )

# For now openvslam is only available for unix platforms.
if(UNIX)
endif()

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_viz_scene3d
                 module_ui_qt
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_vtk
                 module_io_video
                 module_sync
                 module_data
                 module_io_vision
                 module_navigation_openvslam
)
