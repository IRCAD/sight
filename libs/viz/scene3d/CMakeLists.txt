sight_add_target(viz_scene3d TYPE LIBRARY FAST_DEBUG ON)

target_link_libraries(viz_scene3d PUBLIC OgreMain)

find_package(OpenGL QUIET REQUIRED)
target_link_libraries(viz_scene3d PRIVATE ${OPENGL_LIBRARIES})

set(FWPROJECT_FULLNAME viz_scene3d)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources.cfg.in"
    "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/viz_scene3d/resources.cfg" @ONLY
)

target_link_libraries(viz_scene3d PUBLIC core data geometry_data ui viz service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
