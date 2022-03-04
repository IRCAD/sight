sight_add_target(Tuto05MultithreadConsoleCpp TYPE APP)

find_package(Boost QUIET COMPONENTS log REQUIRED)
target_link_libraries(Tuto05MultithreadConsoleCpp PRIVATE ${Boost_LOG_LIBRARY})

add_dependencies(Tuto05MultithreadConsoleCpp sightrun module_appXml data module_service module_ui_console)

target_link_libraries(Tuto05MultithreadConsoleCpp PUBLIC data service)

# Allow dark theme via module_ui_qt
module_param(module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES /flatdark.rcc /flatdark.qss)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES Tuto05MultithreadConsoleCpp_AppCfg)

sight_generate_profile(Tuto05MultithreadConsoleCpp)
