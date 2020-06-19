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
set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${FW_QT5_LOCATION}/qml" CACHE STRING "QtCreator extra import paths for QML modules" FORCE)

# Disable C4275 warning: non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'.
# On windows, Qml classes are entirely exported but IService class is not exported only its methods.
target_compile_options(${FWPROJECT_NAME} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/wd4275>)
