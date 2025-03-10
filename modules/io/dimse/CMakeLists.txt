sight_add_target(module_io_dimse TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(
    ${SIGHT_TARGET}
    PRIVATE core
            ui
            ui_qt
            io_dimse
            data
            service
            io
)

add_dependencies(${SIGHT_TARGET} module_ui_qt)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
