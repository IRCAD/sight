fwLoadProperties()

add_definitions("-DQT_NO_KEYWORDS")

if(UNIX AND NOT APPLE)
    find_package(Qt5 COMPONENTS Core Gui Widgets X11Extras REQUIRED)
else()
    find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
endif()

#find_package(OIS REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
#    ${OIS_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
#    ${OIS_LIBRARIES}
)

if(UNIX AND NOT APPLE)
    fwForwardInclude(${Qt5X11Extras_INCLUDE_DIRS})
    fwLink(${Qt5X11Extras_LIBRARIES})
endif()

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
