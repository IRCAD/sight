fwLoadProperties()

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

find_package(VTK REQUIRED)
fwForwardInclude(${VTK_INCLUDE_DIRS})
fwLink(${VTK_LIBRARIES})

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

