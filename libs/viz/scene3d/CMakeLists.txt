sight_add_target(viz_scene3d TYPE LIBRARY FAST_DEBUG ON)

target_link_libraries(viz_scene3d PUBLIC OgreMain)

find_package(OpenGL QUIET REQUIRED)
target_link_libraries(viz_scene3d PRIVATE ${OPENGL_LIBRARIES})

if(OGRE_VERSION VERSION_GREATER_EQUAL 13.6.2)
    set(VES_POSITION xfb_position)
    set(VES_NORMAL xfb_normal)
    set(VES_COLOR xfb_colour)
    set(VES_TEXTURE_COORDINATES_0 xfb_uv0)
else()
    set(VES_POSITION oPos)
    set(VES_NORMAL oNormal)
    set(VES_COLOR oColour)
    set(VES_TEXTURE_COORDINATES_0 oUv0)
endif()

set(SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${SIGHT_TARGET}/Media/glsl")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RenderScene_GP.glsl.in" "${SHADERS_OUTPUT_DIR}/common/RenderScene_GP.glsl" @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VolumeBricks_GP.glsl.in" "${SHADERS_OUTPUT_DIR}/vr/VolumeBricks_GP.glsl" @ONLY
)

target_link_libraries(viz_scene3d PUBLIC core data geometry_data ui viz service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
