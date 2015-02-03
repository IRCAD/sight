fwLoadProperties()

find_package (Boost COMPONENTS thread date_time REQUIRED)
fwInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

find_package(Qt5 COMPONENTS Core Gui REQUIRED)
QT5_USE_MODULES(${FWPROJECT_NAME} Core Gui Widgets)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

