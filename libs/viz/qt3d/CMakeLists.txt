sight_add_target( viz_qt3d TYPE LIBRARY WARNINGS_AS_ERRORS ON )


target_compile_definitions(viz_qt3d PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS
    Core
    Gui
    Quick
    3DCore
    3DRender
    3DExtras
    3DInput
REQUIRED)

target_link_libraries(viz_qt3d PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::3DCore
    Qt5::3DRender
    Qt5::3DExtras
    Qt5::3DInput
)

set(FWPROJECT_FULLNAME viz_qt3d)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/resources.cfg.in"
                "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/viz_qt3d/resources.cfg" @ONLY)

set_target_properties(viz_qt3d PROPERTIES AUTOMOC TRUE)

# Disable C4251 warning: class 'class_1>' needs to have dll-interface to be used by clients of class 'class_2'
# On windows, Qml classes are entirely exported but IService class is not exported, only its methods.
target_compile_options(viz_qt3d PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/wd4251>)

target_link_libraries(viz_qt3d PUBLIC 
                      core
                      data
                      geometry_data
                      ui_base
                      viz_base
                      service
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
