
fwLoadProperties()
fwUseForwardInclude(
    gui
    fwData
    fwServices
    fwComEd
    ARLcore
    fwDataTools
    fwGuiQt
    fwItkIO
)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

fwForwardInclude(
    ${QT_INCLUDE_DIR}
    )
fwLink(
    ${QT_LIBRARIES}
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
