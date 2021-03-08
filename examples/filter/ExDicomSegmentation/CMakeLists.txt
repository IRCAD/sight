sight_add_target( ExDicomSegmentation TYPE APP UNIQUE ON )



add_dependencies(ExDicomSegmentation 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_viz_scene3d
                 module_ui_qt
                 module_viz_scene3dQt
                 module_activity
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_activity
                 module_io_matrix
                 module_io_dicom
                 module_io_vtk
                 module_io_atoms
                 3DVisualizationActivity
                 2DVisualizationActivity
                 module_filter_image
                 module_data
                 module_filter_mesh
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
        ExDicomSegmentation_AppCfg
) # Main application's configuration to launch

sight_generate_profile(ExDicomSegmentation)
