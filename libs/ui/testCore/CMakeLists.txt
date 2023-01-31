sight_add_target(guiTest TYPE LIBRARY FAST_DEBUG ON)

set(CMAKE_AUTOMOC ON)
set_target_properties(guiTest PROPERTIES AUTOMOC TRUE)

target_link_libraries(guiTest PUBLIC Qt5::Widgets Qt5::Test)
target_link_libraries(guiTest PUBLIC core utest utestData)
