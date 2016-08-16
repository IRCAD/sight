fwLoadProperties()


find_package(Boost COMPONENTS filesystem REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(DCMTK REQUIRED)
fwInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
