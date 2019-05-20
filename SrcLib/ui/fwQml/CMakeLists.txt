fwLoadProperties()

target_compile_definitions(${FWPROJECT_NAME} PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Core Gui Qml Quick REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Qml Qt5::Quick)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

if(FW_BUILD_EXTERNAL)
    set(FW_QT5_LOCATION "${Sight_LIBRARY_DIR}/../..")
else()
    set(FW_QT5_LOCATION "${CONAN_QT_ROOT}")
endif()
target_compile_definitions(${FWPROJECT_NAME} PRIVATE QML_IMPORT_PATH="${FW_QT5_LOCATION}/qml")
set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${FW_QT5_LOCATION}/qml" CACHE string "QtCreator extra import paths for QML modules" FORCE)
