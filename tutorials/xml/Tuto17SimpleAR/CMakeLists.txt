fwLoadProperties()

set(MODULE_PATH "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${FWPROJECT_NAME}")

file(DOWNLOAD https://owncloud.ircad.fr/index.php/s/ZGo6FTVOsQlWycG/download "${MODULE_PATH}/calibration.xml" SHOW_PROGRESS
     EXPECTED_HASH SHA256=2f983ec650e9df2329354c68d9aff3818741dda921c1857692886cf1bbd947b2)

file(DOWNLOAD https://owncloud.ircad.fr/index.php/s/oyo3mDMObZcRpQM/download "${MODULE_PATH}/aruco_tag.m4v" SHOW_PROGRESS
      EXPECTED_HASH SHA256=92ffe4f0f89380985980a5c1c2fb7536543dee237cddd21922ead2c4e83ecbe2)

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_matrix
                 module_io_vtk
                 module_io_atoms
                 module_io_video
                 module_io_vision
                 module_data
                 module_sync
                 module_navigation_optics
                 module_geometry_vision
                 module_geometry_vision
                 module_geometry_base
)
