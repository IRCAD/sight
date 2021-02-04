set( NAME ExVideoFiltering )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_ui_qt                   # Start the module, allow dark theme
    module_viz_ogreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticon

    # Reader
    videoOpenCV
    videoQt

    # Services
    videoTools
    videoFilter

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
        ExVideoFiltering_AppCfg
) # Main application's configuration to launch
