fwLoadProperties()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(VTK COMPONENTS vtkViewsQt REQUIRED NO_MODULE)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${VTK_LIBRARIES}

)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

