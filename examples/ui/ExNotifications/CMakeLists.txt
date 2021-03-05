sight_add_target( ExNotifications TYPE APP )

add_dependencies(ExNotifications 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_service
                 module_ui_base
)

target_link_libraries(ExNotifications PUBLIC ui_base)

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
        ExNotifications_AppCfg
) # Main application's configuration to launch

sight_generate_profile(ExNotifications)
