sight_add_target( ExTimeLine TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 module_appXml
                 module_service
                 fwlauncher
                 module_ui_console
                 exTimeLineData
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
                      core
                      exTimeLineData
)
