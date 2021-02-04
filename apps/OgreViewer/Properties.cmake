set( NAME OgreViewer )
set( VERSION 0.5 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_ui_qt                   # Start the module, allow dark theme
    patchMedicalData        # Patch old atoms data

    module_viz_ogreQt              # Enable Ogre to render things in Qt window.

    # Objects declaration
    module_services
    data
    ioPacs

    # UI declaration/Actions
    module_ui_base
    uiVisuOgre
    module_ui_flaticon

    # Reader
    module_io_vtk
    module_io_atoms
    ioGdcm

    # Services
    scene2D
    module_data
    ctrlTF
    module_filter_image
    ioPacs
    uiMedDataQt

    # Configuration launchers
    ogreConfig

    # Generic Scene
    )

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        module_ui_qt-0.1/flatdark.rcc
        module_ui_qt-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        OgreViewer_AppCfg
) # Main application's configuration to launch
