fwLoadProperties()

find_package(Boost QUIET COMPONENTS Regex REQUIRED)
find_package(Qt5 QUIET COMPONENTS Core Gui Quick Qml QuickControls2 REQUIRED)


fwForwardInclude(
    ${Boost_REGEX_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5QuickControls2_INCLUDE_DIRS}
)

fwLink(
       ${Boost_REGEX_LIBRARY}
       ${Qt5Core_LIBRARIES}
       ${Qt5Gui_LIBRARIES}
       ${Qt5Quick_LIBRARIES}
       ${Qt5Qml_LIBRARIES}
       ${Qt5QuickControls2_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
