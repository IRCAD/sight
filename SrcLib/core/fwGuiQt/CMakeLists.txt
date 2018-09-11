fwLoadProperties()

if(WIN32)
    set(FW_QT5_PLUGINS_PATH "bin/qt5/plugins")
else()
    set(FW_QT5_PLUGINS_PATH "lib/qt5/plugins")
endif()

configure_file("${CMAKE_CURRENT_LIST_DIR}/bin/qt.conf" "${CMAKE_BINARY_DIR}/bin/qt.conf")
if(BUILD_SDK)
    install(
      FILES
        "${CMAKE_BINARY_DIR}/bin/qt.conf"
      DESTINATION
        ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
    )
endif()

target_compile_definitions(${FWPROJECT_NAME} PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Concurrent REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
