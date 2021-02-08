fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Help Multimedia MultimediaWidgets  Qml Quick Widgets QuickWidgets REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Help 
                                                Qt5::Multimedia Qt5::MultimediaWidgets
                                                Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::QuickWidgets )

find_package(glm QUIET REQUIRED)

fwForwardInclude(

    ${GLM_INCLUDE_DIRS}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

if(FW_BUILD_EXTERNAL)
    set(FW_QT5_LOCATION "${Sight_LIBRARY_DIR}/../..")
else()
    set(FW_QT5_LOCATION "${CONAN_QT_ROOT}")
endif()

target_compile_definitions(${FWPROJECT_NAME} PRIVATE QML_IMPORT_PATH="${FW_QT5_LOCATION}/qml")

# Copy resources for styles
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")

qt5_add_binary_resources(${FWPROJECT_NAME}_DARKSTYLE_RCC
    "rc/darkstyle.qrc"
    DESTINATION "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/darkstyle.rcc"
)

qt5_add_binary_resources(${FWPROJECT_NAME}_FLATDARK_RCC
    "rc/flatdark.qrc"
    DESTINATION "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/flatdark.rcc"
)

add_dependencies(${FWPROJECT_NAME} ${FWPROJECT_NAME}_DARKSTYLE_RCC)
add_dependencies(${FWPROJECT_NAME} ${FWPROJECT_NAME}_FLATDARK_RCC)
