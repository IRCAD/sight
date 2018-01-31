
fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
find_package(ITK QUIET REQUIRED)
find_package(VTK QUIET COMPONENTS vtkCommonDataModel NO_MODULE REQUIRED)
include(${VTK_USE_FILE})

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

    ${VTK_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
