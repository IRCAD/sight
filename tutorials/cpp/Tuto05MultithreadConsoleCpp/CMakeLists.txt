sight_add_target( Tuto05MultithreadConsoleCpp TYPE APP )

find_package(Boost QUIET COMPONENTS log REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${Boost_LOG_LIBRARY})

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 data
                 module_service
                 module_ui_console
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC data service)
