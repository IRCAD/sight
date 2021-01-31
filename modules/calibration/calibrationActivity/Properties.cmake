
set( NAME calibrationActivity )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    data
)
set( REQUIREMENTS
    module_io_base             # Start the module, load file location or window module_io_base
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_viz_ogreQt              # Enable Ogre to render things in Qt window.
    validators
    activities

    # Objects declaration
    module_services             # fwService
    data

    # UI declaration/Actions
    module_ui_base
    media
    uiTools
    uiIO
    module_ui_qt
    uiPreferences
    uiMedDataQt

    # Reader
    module_io_file
    ioCalibration
    videoQt

    # Services
    videoCalibration
    videoCharucoCalibration
    videoOpenCV
    videoOpenni
    videoTools
    registrationCV
    ctrlCamp

    # Generic Scene
)
