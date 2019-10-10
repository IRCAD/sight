fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Qml Quick Widgets QuickWidgets REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::QuickWidgets)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

    if(FW_BUILD_EXTERNAL)
        set(FW_QT5_LOCATION "${Sight_LIBRARY_DIR}/../..")
    else()
        set(FW_QT5_LOCATION "${CONAN_QT_ROOT}")
    endif()

target_compile_definitions(${FWPROJECT_NAME} PRIVATE QML_IMPORT_PATH="${FW_QT5_LOCATION}/qml")
