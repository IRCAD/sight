fwLoadProperties()

find_package(VTK QUIET REQUIRED)
fwForwardInclude(${VTK_INCLUDE_DIRS})
fwLink(${VTK_LIBRARIES})

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

