cmake_policy(SET CMP0057 NEW)

# install all Qt5 plugins
message("${DEPENDS}")
if(${DEPENDS} MATCHES "_qt")
    message(STATUS "Install Qt plugins from '${QT_PLUGINS_SOURCE_DIR}'")
    file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX}/${QT_DESTINATION} TYPE DIRECTORY FILES ${QT_PLUGINS_SOURCE_DIR})
endif()

if(${DEPENDS} MATCHES "viz_scene3d ")
    if("${BUILD_TYPE}" STREQUAL "Debug")
        if(EXISTS "${OGRE_PLUGIN_DIR}/../debug/bin")
            get_filename_component(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR}/../debug/bin" ABSOLUTE)
        endif()
    endif()
    
    message(STATUS "Install Ogre plugins from '${OGRE_PLUGIN_DIR}'")

    file(GLOB OGRE_PLUGINS
        "${OGRE_PLUGIN_DIR}/*RenderSystem*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
        "${OGRE_PLUGIN_DIR}/*Plugin_*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
        "${OGRE_PLUGIN_DIR}/*Codec_*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
    )
    file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX}/ogrePlugins FILES ${OGRE_PLUGINS})
endif()
