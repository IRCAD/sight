sight_add_target( ExOpenvslam TYPE APP )

# For now openvslam is only available for unix platforms.
if(UNIX)
endif()

add_dependencies(${FWPROJECT_NAME} 
                 sightrun
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

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExOpenvslam_AppCfg
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
