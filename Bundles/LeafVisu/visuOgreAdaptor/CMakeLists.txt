fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwComEd
    fwData
    fwRuntime
    fwServices
    fwRenderOgre
    arData
)

useOgre()

find_package (Boost COMPONENTS regex REQUIRED)

fwLink(
    ${Boost_REGEX_LIBRARY}
    )
