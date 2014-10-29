fwLoadProperties()
fwUseForwardInclude(
    fwRuntime
    fwData
    fwGuiQt

    gui
    io
)


find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)


fwForwardInclude(
    ${QT_INCLUDE_DIR}
    )
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


