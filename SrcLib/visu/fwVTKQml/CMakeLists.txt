fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Qml Qt5::Quick)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
