sight_add_target( module_io_dimse TYPE MODULE PRIORITY 1 )

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_io_dimse PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(module_io_dimse PROPERTIES AUTOMOC TRUE)

target_link_libraries(module_io_dimse PUBLIC 
                      core
                      ui_base
                      ui_qt
                      io_dimse
                      data
                      service
                      io_base
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
