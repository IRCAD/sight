sight_add_target( ExTimeLine TYPE APP )

add_dependencies(${FWPROJECT_NAME} 
                 module_appXml
                 module_service
                 sightrun
                 module_ui_console
)

target_include_directories(${FWPROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )

target_link_libraries(${FWPROJECT_NAME} PUBLIC data service core)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExTimeLine_AppCfg
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
