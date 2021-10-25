sight_add_target( module_ui_qt TYPE MODULE START ON WARNINGS_AS_ERRORS OFF)

add_dependencies(module_ui_qt module_ui_base module_data)

find_package(Qt5 QUIET COMPONENTS Core Gui Help Multimedia MultimediaWidgets  Qml Quick Widgets QuickWidgets REQUIRED)

target_link_libraries(module_ui_qt PRIVATE  Qt5::Core Qt5::Gui Qt5::Help 
                                            Qt5::Multimedia Qt5::MultimediaWidgets
                                            Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::QuickWidgets )

find_package(glm QUIET REQUIRED)
target_include_directories(module_ui_qt SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

set_target_properties(module_ui_qt PROPERTIES AUTOMOC TRUE)

if(MSVC)
    set(FW_QT5_LOCATION "${Qt5_DIR}/../../..$<$<CONFIG:Debug>:/debug>")
    target_compile_definitions(module_ui_qt PRIVATE QML_IMPORT_PATH="${FW_QT5_LOCATION}/qml")
endif()

# Copy resources for styles
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/module_ui_qt")

qt5_add_binary_resources(module_ui_qt_DARKSTYLE_RCC
    "rc/darkstyle.qrc"
    DESTINATION "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/module_ui_qt/darkstyle.rcc"
)

qt5_add_binary_resources(module_ui_qt_FLATDARK_RCC
    "rc/flatdark.qrc"
    DESTINATION "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/module_ui_qt/flatdark.rcc"
)

add_dependencies(module_ui_qt module_ui_qt_DARKSTYLE_RCC)
add_dependencies(module_ui_qt module_ui_qt_FLATDARK_RCC)

target_link_libraries(module_ui_qt PUBLIC 
                      activity
                      core
                      data
                      geometry_data
                      io_base
                      service
                      ui_qt
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
