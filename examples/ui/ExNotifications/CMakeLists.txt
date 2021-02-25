sight_add_target( ExNotifications TYPE APP )

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_service
                 module_ui_base
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC ui_base)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight_module_ui_qt/flatdark.rcc
        sight_module_ui_qt/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExNotifications_AppCfg
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
