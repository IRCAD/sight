fwLoadProperties()

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

find_package(VTK REQUIRED)
fwForwardInclude(${VTK_INCLUDE_DIRS})
fwLink(${VTK_LIBRARIES})

find_package(Qt5 COMPONENTS Core Gui REQUIRED)
QT5_USE_MODULES(${FWPROJECT_NAME} Core Gui Widgets Concurrent)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

