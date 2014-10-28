fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwRuntime
    fwTools
)


find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Help Widgets REQUIRED)
QT5_USE_MODULES(uiGenericQt Core Gui Help Widgets)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${QT_INCLUDE_DIR}
    )
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


