sight_add_target( Tuto04SignalSlot TYPE APP )

add_dependencies(Tuto04SignalSlot 
                 sightrun
                 module_appXml
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_viz_sample
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
        Tuto04SignalSlot_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto04SignalSlot)
