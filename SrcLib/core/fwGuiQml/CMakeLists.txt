fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Qml Quick Concurrent REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent Qt5::Quick Qt5::Qml)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
