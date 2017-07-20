fwLoadProperties()

if(${FWPROJECT_NAME}_INSTALL)
    install(DIRECTORY "${QT_DIR}/plugins" DESTINATION qtplugins COMPONENT QtPlugins)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)


set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

