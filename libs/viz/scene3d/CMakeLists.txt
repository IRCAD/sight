sight_add_target(viz_scene3d TYPE LIBRARY PCH pchServiceOmp)

# include first to be taken into account in the PCH compile flags
find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    target_link_libraries(viz_scene3d PRIVATE OpenMP::OpenMP_CXX)
endif()

target_link_libraries(viz_scene3d PUBLIC OgreOverlay)

find_package(OpenGL QUIET REQUIRED)
target_link_libraries(viz_scene3d PRIVATE ${OPENGL_LIBRARIES})

# This is a hack to copy plugins inside the build directory
# Most developers executes apps inside the build directory so this is done as a convenience

copy_ogre_plugins()

set(FWPROJECT_FULLNAME viz_scene3d)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources.cfg.in"
                "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/viz_scene3d/resources.cfg" @ONLY)

target_link_libraries(viz_scene3d PUBLIC 
                      core
                      data
                      geometry_data
                      ui_base
                      viz_base
                      service
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
