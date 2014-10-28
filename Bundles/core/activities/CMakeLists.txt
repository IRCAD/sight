fwLoadProperties()
fwUseForwardInclude(
    fwActivities
    fwGui
    fwRuntime
    fwTools
)

Find_package (Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
QT5_USE_MODULES(activities Core Gui Widgets)


fwForwardInclude(${Boost_INCLUDE_DIRS} ${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

#set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


