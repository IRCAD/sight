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
    fwRenderOgre
)

find_package(CppUnit)

useOgre()

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(
    ${CPPUNIT_LIBRARY}
)

