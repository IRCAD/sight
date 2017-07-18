fwLoadProperties()


find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(ZLIB REQUIRED )

find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    ${ZLIB_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${cgogn_LIBRARY}
    ${ZLIB_LIBRARIES}
)


