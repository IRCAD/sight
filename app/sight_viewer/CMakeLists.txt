if(WIN32)

    # Preserve NSIS backslashes, quotes and variables when CPackConfig.cmake is generated.
    set(CPACK_VERBATIM_VARIABLES TRUE)

    set(SIGHT_VIEWER_PROG_ID "SightViewer.File")

    set(SIGHT_VIEWER_FILE_EXTENSIONS
        ".dcm"
        ".nii"
        ".inr"
        # Windows only uses the final suffix for file associations. Registering
        # .gz makes Sight Viewer available in "Open with" for .nii.gz/.inr.gz.
        ".gz"
        ".vtk"
        ".vtp"
        ".vti"
        ".vtu"
        ".mhd"
        ".ply"
        ".obj"
        ".stl"
    )

    set(SIGHT_VIEWER_INSTALL_COMMANDS "")
    set(SIGHT_VIEWER_UNINSTALL_COMMANDS "")

    file(TO_NATIVE_PATH "${SIGHT_MODULE_RC_PREFIX}/sight_viewer/app.ico" SIGHT_VIEWER_ICON_PATH)
    file(TO_NATIVE_PATH "bin/sight_viewer.bat" SIGHT_VIEWER_LAUNCHER_PATH)

    # ProgID
    string(APPEND SIGHT_VIEWER_INSTALL_COMMANDS
           "WriteRegStr HKCU \"Software\\Classes\\${SIGHT_VIEWER_PROG_ID}\" \"\" \"Sight Viewer file\"\n"
    )

    # Icon
    string(APPEND SIGHT_VIEWER_INSTALL_COMMANDS
           "WriteRegStr HKCU \"Software\\Classes\\${SIGHT_VIEWER_PROG_ID}\\DefaultIcon\" \"\" "
           "\"$INSTDIR\\${SIGHT_VIEWER_ICON_PATH}\"\n"
    )

    # Command executed when a file is opened with Sight Viewer.
    string(APPEND SIGHT_VIEWER_INSTALL_COMMANDS
           "WriteRegStr HKCU \"Software\\Classes\\${SIGHT_VIEWER_PROG_ID}\\shell\\open\\command\" \"\" "
           "'\"$INSTDIR\\${SIGHT_VIEWER_LAUNCHER_PATH}\" --input \"%1\"'\n"
    )

    # Make Sight Viewer available for folders from the Windows Explorer context menu.
    string(
        APPEND
        SIGHT_VIEWER_INSTALL_COMMANDS
        "WriteRegStr HKCU \"Software\\Classes\\Directory\\shell\\SightViewer\" \"\" "
        "\"Open with Sight Viewer\"\n"
        "WriteRegStr HKCU \"Software\\Classes\\Directory\\shell\\SightViewer\" \"Icon\" "
        "\"$INSTDIR\\${SIGHT_VIEWER_ICON_PATH}\"\n"
        "WriteRegStr HKCU \"Software\\Classes\\Directory\\shell\\SightViewer\\command\" \"\" "
        "'\"$INSTDIR\\${SIGHT_VIEWER_LAUNCHER_PATH}\" --input \"%1\"'\n"
    )

    string(APPEND SIGHT_VIEWER_UNINSTALL_COMMANDS
           "DeleteRegKey HKCU \"Software\\Classes\\Directory\\shell\\SightViewer\"\n"
    )

    # Keep an explicit Sight Viewer command for gzip-compressed medical images.
    string(
        APPEND
        SIGHT_VIEWER_INSTALL_COMMANDS
        "WriteRegStr HKCU \"Software\\Classes\\SystemFileAssociations\\.gz\\shell\\SightViewer\" \"\" "
        "\"Open with Sight Viewer\"\n"
        "WriteRegStr HKCU \"Software\\Classes\\SystemFileAssociations\\.gz\\shell\\SightViewer\" \"Icon\" "
        "\"$INSTDIR\\${SIGHT_VIEWER_ICON_PATH}\"\n"
        "WriteRegStr HKCU \"Software\\Classes\\SystemFileAssociations\\.gz\\shell\\SightViewer\\command\" \"\" "
        "'\"$INSTDIR\\${SIGHT_VIEWER_LAUNCHER_PATH}\" --input \"%1\"'\n"
    )

    string(APPEND SIGHT_VIEWER_UNINSTALL_COMMANDS
           "DeleteRegKey HKCU \"Software\\Classes\\SystemFileAssociations\\.gz\\shell\\SightViewer\"\n"
    )

    # Associate supported extensions with the ProgID
    foreach(SIGHT_VIEWER_EXTENSION IN LISTS SIGHT_VIEWER_FILE_EXTENSIONS)

        string(APPEND SIGHT_VIEWER_INSTALL_COMMANDS "WriteRegStr HKCU "
               "\"Software\\Classes\\${SIGHT_VIEWER_EXTENSION}\\OpenWithProgids\" "
               "\"${SIGHT_VIEWER_PROG_ID}\" \"\"\n"
        )

        string(APPEND SIGHT_VIEWER_UNINSTALL_COMMANDS "DeleteRegValue HKCU "
               "\"Software\\Classes\\${SIGHT_VIEWER_EXTENSION}\\OpenWithProgids\" " "\"${SIGHT_VIEWER_PROG_ID}\"\n"
        )

    endforeach()

    # Delete registry entries during uninstallation.
    string(APPEND SIGHT_VIEWER_UNINSTALL_COMMANDS "DeleteRegKey HKCU \"Software\\Classes\\${SIGHT_VIEWER_PROG_ID}\"\n")

    # Notify Windows that file associations have changed.
    string(APPEND SIGHT_VIEWER_INSTALL_COMMANDS "System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, p 0, p 0)'\n")
    string(APPEND SIGHT_VIEWER_UNINSTALL_COMMANDS
           "System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, p 0, p 0)'\n"
    )

    # These variables must be set before sight_add_target(), which generates CPackConfig.cmake.
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${SIGHT_VIEWER_INSTALL_COMMANDS}")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${SIGHT_VIEWER_UNINSTALL_COMMANDS}")

endif()

sight_add_target(sight_viewer TYPE APP)

add_dependencies(
    sight_viewer
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_viz_scene3d_qt
    module_service
    data
    io
    viz_scene2d
    module_ui
    module_ui_viz
    module_ui_icons
    module_io_itk
    module_io_vtk
    module_io_dicom
    module_io_session
    config_viz_scene2d
    config_viz_scene3d
    config_io_dicom
)

module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/noctura.rcc
                                                                            sight::module::ui::qt/noctura.qss true
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config parameters PARAM_VALUES SightViewer_AppCfg SightViewer_AppCfgParam)

if(LINUX)
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/rc/sight_viewer_desktop" DESTINATION "${CMAKE_INSTALL_BINDIR}")

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/rc/sight_viewer.desktop"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    )

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/rc/sight_viewer-mime.xml"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/mime/packages"
    )
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif(SIGHT_BUILD_TESTS)
