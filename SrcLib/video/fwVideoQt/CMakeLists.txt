fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Multimedia REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Multimedia)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${FWPROJECT_NAME} PUBLIC "-DQT_NO_KEYWORDS")
