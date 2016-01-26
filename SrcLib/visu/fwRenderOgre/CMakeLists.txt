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

fwForwardInclude(
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_Overlay_INCLUDE_DIRS}
)

fwLink(
    ${Boost_REGEX_LIBRARY}
    )

