sight_add_target(module_io_dimse TYPE MODULE)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_io_dimse PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(module_io_dimse PROPERTIES AUTOMOC TRUE)

target_link_libraries(
    module_io_dimse
    PUBLIC core
           ui
           ui_qt
           io_dimse
           data
           service
           io
)

add_dependencies(module_io_dimse module_ui_qt)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
