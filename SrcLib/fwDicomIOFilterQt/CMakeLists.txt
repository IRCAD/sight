fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwTools
    fwMedData
    fwDicomData
    fwDicomIOFilter
    fwGui
    fwGuiQt
    fwDicomIOExt
)


find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

find_package(Qt5 COMPONENTS Core Gui Widgets Concurrent REQUIRED)
QT5_USE_MODULES(${FWPROJECT_NAME} Core Gui Widgets Concurrent)
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

