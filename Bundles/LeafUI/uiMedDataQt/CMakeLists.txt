fwLoadProperties()

fwUseForwardInclude(
    fwData
    fwGui
    fwMedData
    fwRuntime
    fwServices
    fwTools

    gui
)

fwReq(
    visuVTKAdaptor
    uiImageQt
    ctrlSelection
    )

find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )

fwLink(
    ${QT_LIBRARIES}
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


