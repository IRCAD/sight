sight_add_target(module_io_dicomweb TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

add_dependencies(${SIGHT_TARGET} data module_service)

target_link_libraries(
    ${SIGHT_TARGET}
    PUBLIC core
           ui
           ui_qt
           io_http
           data
           service
           io
)
