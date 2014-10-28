fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwGui
    fwMemory
    fwRuntime
    fwTools

    gui
)


find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets Concurrent REQUIRED)
find_package (LibXml2)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
    )
fwLink(
    ${QT_LIBRARIES}
    ${LIBXML2_LIBRARIES} 
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


