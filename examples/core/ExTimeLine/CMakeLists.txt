sight_add_target( ExTimeLine TYPE APP )

add_dependencies(ExTimeLine
                 module_appXml
                 module_service
                 sightrun
                 module_ui_console
)

target_include_directories(ExTimeLine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )

target_link_libraries(ExTimeLine PUBLIC data service core)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExTimeLine_AppCfg
) # Main application's configuration to launch

sight_generate_profile(ExTimeLine)
