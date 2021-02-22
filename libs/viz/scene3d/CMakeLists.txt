# include first to be taken into account in the PCH compile flags
find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

fwLoadProperties()

target_link_libraries(${FWPROJECT_NAME} PUBLIC OgreOverlay)

find_package(OpenGL QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

# This is a hack to copy plugins inside the build directory
# Most developers executes apps inside the build directory so this is done as a convenience

# Fixup VCPKG paths
if(WIN32)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        if(EXISTS "${OGRE_PLUGIN_DIR}/../debug/bin")
            get_filename_component(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR}/../debug/bin" ABSOLUTE)
        endif()

        if(EXISTS "${OGRE_CONFIG_DIR}/../debug/bin")
            get_filename_component(OGRE_CONFIG_DIR "${OGRE_CONFIG_DIR}/../debug/bin" ABSOLUTE)
        endif()
    endif()

    file(GLOB OGRE_PLUGINS
        "${OGRE_PLUGIN_DIR}/*RenderSystem*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
        "${OGRE_PLUGIN_DIR}/*Plugin_*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
        "${OGRE_PLUGIN_DIR}/*Codec_*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
    )

    set(FW_OGRE_PLUGINS_DIR "${PROJECT_BINARY_DIR}/ogreplugins/")

    # This copies the plugins into the build directory
    file(INSTALL ${OGRE_PLUGINS} DESTINATION "${FW_OGRE_PLUGINS_DIR}")

    # This copies the plugins into the install directory
    install(DIRECTORY "${FW_OGRE_PLUGINS_DIR}" DESTINATION "ogrePlugins/")
endif()


set(FWPROJECT_FULLNAME ${FWPROJECT_NAME})

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources.cfg.in"
                "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}/resources.cfg" @ONLY)
