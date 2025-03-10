sight_add_target(module_ui_dicom TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Widgets Concurrent REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets Qt6::Concurrent)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

add_dependencies(${SIGHT_TARGET} data module_service)

target_link_libraries(
    ${SIGHT_TARGET}
    PRIVATE core
            filter_dicom
            ui_dicom
            ui
            ui_qt
            service
            io
            data
)
