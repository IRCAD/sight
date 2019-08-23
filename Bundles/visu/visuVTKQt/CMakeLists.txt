fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
find_package(VTK QUIET COMPONENTS vtkGUISupportQt REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    vtkGUISupportQt
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
