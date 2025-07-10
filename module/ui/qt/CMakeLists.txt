sight_add_target(module_ui_qt TYPE MODULE)

add_dependencies(${SIGHT_TARGET} module_ui module_ui_icons module_data module_io_session)

find_package(Qt6 QUIET COMPONENTS Multimedia MultimediaWidgets Widgets SvgWidgets Help REQUIRED)
target_link_libraries(
    ${SIGHT_TARGET} PUBLIC Qt6::Multimedia Qt6::MultimediaWidgets Qt6::Widgets Qt6::SvgWidgets Qt6::Help
)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

# Copy resources for styles
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}")

qt_add_binary_resources(
    module_ui_qt_FLATDARK_RCC "rc/flatdark.qrc" DESTINATION
    "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/module_ui_qt/flatdark.rcc"
)

qt_add_binary_resources(
    module_ui_qt_IRCAD_RCC "rc/noctura.qrc" DESTINATION
    "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/module_ui_qt/noctura.rcc"
)

add_dependencies(${SIGHT_TARGET} module_ui_qt_FLATDARK_RCC)
add_dependencies(${SIGHT_TARGET} module_ui_qt_IRCAD_RCC)

target_link_libraries(${SIGHT_TARGET} PUBLIC activity app geometry_data io io_joystick ui_qt)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
