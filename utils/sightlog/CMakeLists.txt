sight_add_target(sightlog TYPE EXECUTABLE)

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
target_link_libraries(sightlog PRIVATE Boost::program_options)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(sightlog PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(sightlog PROPERTIES AUTOMOC TRUE)

target_link_libraries(sightlog PUBLIC core io_zip)
