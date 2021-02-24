sight_add_target( ExTimeLine TYPE APP )

add_dependencies(${FWPROJECT_NAME} 
                 module_appXml
                 module_service
                 fwlauncher
                 module_ui_console
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
                      core
                      exTimeLineData
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExTimeLine_AppCfg
) # Main application's configuration to launch

sight_add_shortcut(${FWPROJECT_NAME})
