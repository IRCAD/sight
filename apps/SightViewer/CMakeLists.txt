sight_add_target( SightViewer TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_viz_scene3d
                 module_ui_qt
                 module_viz_scene3dQt
                 module_service
                 data
                 viz_scene2d
                 io_dimse
                 module_ui_base
                 module_ui_viz
                 module_ui_flaticons
                 module_io_vtk
                 module_io_atoms
                 module_io_dicom
                 module_viz_scene2d
                 module_data
                 module_filter_image
                 module_io_dimse
                 config_viz_scene3d
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
        SightViewer_AppCfg
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
