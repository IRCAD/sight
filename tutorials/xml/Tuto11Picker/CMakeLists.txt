sight_add_target( Tuto11Picker TYPE APP UNIQUE ON )



add_dependencies(Tuto11Picker 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_matrix
                 module_io_vtk
                 module_io_atoms
                 module_data
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(module_appXml PARAM_LIST config PARAM_VALUES Tuto11Picker_AppCfg)

sight_generate_profile(Tuto11Picker)
