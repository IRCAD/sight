sight_add_target( module_activity TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui REQUIRED)
target_link_libraries(module_activity PUBLIC Qt5::Core Qt5::Gui)

if(NOT ${ENABLE_QML_APPLICATION})
    find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)
    target_link_libraries(module_activity PUBLIC Qt5::Widgets)
endif()

set_target_properties(module_activity PROPERTIES AUTOMOC TRUE)

add_dependencies(module_activity 
                 data
                 module_service
)

target_link_libraries(module_activity PUBLIC 
                      core
                      data
                      ui_base
                      service
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
