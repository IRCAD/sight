fwLoadProperties()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS})

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

target_include_directories(
    ${FWPROJECT_NAME} 
    PRIVATE ${uiVisuOgre_INCLUDE_DIR}
    PRIVATE ${CMAKE_BINARY_DIR}/uiVisuOgre/include
    )
