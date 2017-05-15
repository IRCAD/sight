fwLoadProperties()


find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(Qt5 COMPONENTS Core Gui REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
