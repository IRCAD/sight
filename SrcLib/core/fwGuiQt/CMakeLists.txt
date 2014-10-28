fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwGui
    fwRuntime
    fwTools
)

configure_file("${CMAKE_CURRENT_LIST_DIR}/bin/qt.conf" "${CMAKE_BINARY_DIR}/bin/qt.conf")

if(${FWPROJECT_NAME}_INSTALL)
    install(DIRECTORY "${QT_DIR}/plugins/" DESTINATION qtplugins)
endif()

add_definitions("-DQT_NO_KEYWORDS")


find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui REQUIRED)

QT5_USE_MODULES(fwGuiQt Core Gui Widgets Concurrent)


fwForwardInclude(${Boost_INCLUDE_DIRS} ${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
