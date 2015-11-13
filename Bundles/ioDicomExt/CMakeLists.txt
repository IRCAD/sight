fwLoadProperties()
fwUseForwardInclude(
    gui
    io
    fwCore
    fwCom
    fwTools
    fwData
    fwServices
    fwMedData
    fwGui
    fwGuiQt
    fwDicomIOExt
    fwDicomIOFilter
    fwDicomIOFilterQt
    fwDicomData
    fwComEd
    fwDataIO
    fwZip
    fwRuntime
    fwThread
)
    
find_package (Boost COMPONENTS thread date_time REQUIRED)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

