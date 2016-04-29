
fwLoadProperties()
fwUseForwardInclude(
    gui
    fwData
    fwServices
    fwComEd
    fwDataTools
    fwGuiQt
    fwItkIO
)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(ITK REQUIRED)
find_package(VTK REQUIRED)

fwForwardInclude(
    ${ITK_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}

    ITKCommon

    vtkCommonDataModel
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
