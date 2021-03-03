sight_add_target( Tuto12Scene2D TYPE APP )



add_dependencies(${FWPROJECT_NAME} 
                 sightrun
                 module_appXml
                 module_ui_qt
                 module_service
                 module_ui_base
                 module_viz_scene2d
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
        Tuto12Scene2D_AppCfg
   ) # Allow dark theme via module_ui_qt

sight_generate_profile(${FWPROJECT_NAME})
