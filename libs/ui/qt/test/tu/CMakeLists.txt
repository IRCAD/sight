fwLoadProperties()

target_compile_definitions(${FWPROJECT_NAME} PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Widgets)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
