cmake_policy(SET CMP0057 NEW)

include(GNUInstallDirs)

# install all Qt6 plugins
if(${DEPENDS} MATCHES "_qt")
    message(STATUS "Install Qt plugins from '${QT_PLUGINS_SOURCE_DIR}'")
    file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX}/${PLUGINS_DESTINATION} TYPE DIRECTORY FILES
                                                                            ${QT_PLUGINS_SOURCE_DIR}
    )
endif()

if(${DEPENDS} MATCHES "viz_scene3d ")
    message(STATUS "Install Ogre plugins from '${OGRE_PLUGINS_SOURCE_DIR}'")
    file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX}/${PLUGINS_DESTINATION} TYPE DIRECTORY FILES
                                                                            ${OGRE_PLUGINS_SOURCE_DIR}
    )
endif()
