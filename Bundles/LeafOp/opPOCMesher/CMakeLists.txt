fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwDataTools
)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwForwardInclude(${EXTERNAL_LIBRARIES}/include/cgogn/include)
fwLink(${cgogn_LIBRARY})
