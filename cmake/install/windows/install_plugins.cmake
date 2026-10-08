cmake_policy(SET CMP0057 NEW)

include(GNUInstallDirs)

# DEPENDS comes as a space-separated string
separate_arguments(DEPENDS UNIX_COMMAND "${DEPENDS}")

# Qt plugin categories that no application uses: QML development tools and SQL drivers
set(_unused_qt_plugins qmllint qmlls qmltooling sqldrivers)

get_filename_component(_install_root "${CMAKE_INSTALL_PREFIX}/${PLUGINS_DESTINATION}" ABSOLUTE)
set(_plugin_install_directory "${_install_root}/plugins")
list(TRANSFORM _unused_qt_plugins PREPEND "${_plugin_install_directory}/" OUTPUT_VARIABLE _stale_plugin_directories)
file(REMOVE_RECURSE ${_stale_plugin_directories})
file(GLOB_RECURSE _stale_plugin_files LIST_DIRECTORIES FALSE "${_plugin_install_directory}/*.pdb"
                                                             "${_plugin_install_directory}/*.lib"
)
if(_stale_plugin_files)
    file(REMOVE ${_stale_plugin_files})
endif()

set(_plugin_file_filters
    FILES_MATCHING
    PATTERN
    "*"
    PATTERN
    "*.pdb"
    EXCLUDE
    PATTERN
    "*.lib"
    EXCLUDE
)

set(_qt_plugin_filters)
foreach(_plugin ${_unused_qt_plugins})
    list(APPEND _qt_plugin_filters PATTERN "${_plugin}" EXCLUDE)
endforeach()

# install the Qt6 plugins
if(DEPENDS MATCHES "_qt")
    message(STATUS "Install Qt plugins from '${QT_PLUGINS_SOURCE_DIR}'")
    file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX}/${PLUGINS_DESTINATION}
         TYPE DIRECTORY FILES ${QT_PLUGINS_SOURCE_DIR} ${_plugin_file_filters} ${_qt_plugin_filters}
    )
endif()

if("viz_scene3d" IN_LIST DEPENDS)
    message(STATUS "Install Ogre plugins from '${OGRE_PLUGINS_SOURCE_DIR}'")
    file(INSTALL DESTINATION ${CMAKE_INSTALL_PREFIX}/${PLUGINS_DESTINATION}
         TYPE DIRECTORY FILES ${OGRE_PLUGINS_SOURCE_DIR} ${_plugin_file_filters}
    )
endif()
