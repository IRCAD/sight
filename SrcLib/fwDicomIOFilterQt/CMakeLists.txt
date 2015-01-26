fwLoadProperties()

add_definitions("-DQT_NO_KEYWORDS")

find_package(Boost REQUIRED)
find_package(DCMTK REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${DCMTK_config_INCLUDE_DIRS}
    ${QT_INCLUDE_DIR}
    )
fwLink(
    ${Boost_LIBRARIES}
    ${DCMTK_LIBRARIES}
    ${QT_LIBRARIES}
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

