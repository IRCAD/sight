sight_add_target( Tuto01BasicQml TYPE APP START ON )


find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)
target_link_libraries(Tuto01BasicQml PUBLIC Qt5::Core Qt5::Quick Qt5::Qml)
set_target_properties(Tuto01BasicQml PROPERTIES AUTOMOC TRUE)

add_dependencies(Tuto01BasicQml 
                 data
                 module_service
                 module_ui_qml
                 sightrun
)

target_link_libraries(Tuto01BasicQml PUBLIC 
                      data
                      service
                      ui_qml
)

sight_generate_profile(Tuto01BasicQml)
