fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwGui
    fwRenderVTK
    fwRuntime
)

fwReq(
    visu
    visuVTK
)

add_definitions("-DQT_NO_KEYWORDS")

find_package(Boost REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui QtSql QtNetWork QtOpenGL REQUIRED)
include(${QT_USE_FILE})
find_package(VTK REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${VTK_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )
fwLink(
    ${Boost_LIBRARIES} 
    ${QT_LIBRARIES}
    QVTK
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

