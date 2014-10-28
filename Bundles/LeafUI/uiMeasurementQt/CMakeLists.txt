fwLoadProperties()
fwUseForwardInclude(
    fwRuntime
    fwTools

    gui
)


find_package(Boost REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})


fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


