sight_add_target( Tuto05MultithreadConsoleCpp TYPE APP )

find_package(Boost QUIET COMPONENTS log REQUIRED)
target_link_libraries(Tuto05MultithreadConsoleCpp PRIVATE ${Boost_LOG_LIBRARY})

add_dependencies(Tuto05MultithreadConsoleCpp 
                 sightrun
                 module_appXml
                 data
                 module_service
                 module_ui_console
)

target_link_libraries(Tuto05MultithreadConsoleCpp PUBLIC data service)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
/flatdark.rcc
/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto05MultithreadConsoleCpp_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto05MultithreadConsoleCpp)
