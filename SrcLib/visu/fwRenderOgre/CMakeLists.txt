fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwDataTools
    fwGui
    fwGuiQt
    fwMath
    fwRender
    fwRuntime
    fwServices
    fwTools
)

useOgre()
    
find_package (Boost COMPONENTS regex REQUIRED)

include(FindOpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

fwForwardInclude(
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_Overlay_INCLUDE_DIRS}
)

fwLink(
    ${Boost_REGEX_LIBRARY}
)

