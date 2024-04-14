sight_add_target(ui_qml TYPE LIBRARY)

target_compile_definitions(ui_qml PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Core Concurrent Gui Qml Quick REQUIRED)

target_link_libraries(ui_qml PUBLIC Qt5::Core Qt5::Concurrent Qt5::Qml Qt5::Quick)
set_target_properties(ui_qml PROPERTIES AUTOMOC TRUE)

# Disable C4275 warning: non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'.
# On windows, Qml classes are entirely exported but IService class is not exported only its methods.
target_compile_options(ui_qml PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/wd4275>)

target_link_libraries(ui_qml PUBLIC core data ui service)
