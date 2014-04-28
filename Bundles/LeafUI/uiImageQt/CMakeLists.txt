fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwData
    fwGuiQt
    fwRuntime
    fwTools
    gui
)

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui QtHelp REQUIRED)
include(${QT_USE_FILE})


fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )
fwLink(
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


