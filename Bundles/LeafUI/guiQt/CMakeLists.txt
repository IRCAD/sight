fwLoadProperties()
fwUseForwardInclude(
    fwActivities
    fwGuiQt
    fwRuntime
    fwServices
    fwTools

    gui
)

find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)


fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


