fwLoadProperties()
fwUseForwardInclude(
    fwData
    fwRuntime
    fwTools
    gui
)

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)


fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )
fwLink(
    ${Boost_LIBRARIES}
    ${QT_LIBRARIES}
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


