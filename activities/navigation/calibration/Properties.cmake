
set( NAME activity_navigation_calibration )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    activity
    core
    data
)
set( REQUIREMENTS
    module_ui_base             # Start the module, load file location or window module_ui_base
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_viz_ogreQt              # Enable Ogre to render things in Qt window.
    module_activity
    module_activity

    # Objects declaration
    module_service             # fwService
    data

    # UI declaration/Actions
    module_ui_base
    module_ui_media
    module_ui_qt

    # Reader
    module_io_matrix
    module_io_vision
    module_io_video

    # Services
    module_geometry_vision
    module_sync
    module_geometry_vision
    module_data

    # Generic Scene
)
