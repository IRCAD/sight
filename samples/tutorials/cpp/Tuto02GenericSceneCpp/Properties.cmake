set( NAME Tuto02GenericSceneCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    data
    services
)
set( REQUIREMENTS
    module_io_base             # Start the module, load file location or window module_io_base
    module_ui_qt                   # Start the module, allow dark theme
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_viz_ogreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    style
    flatIcon

    # Reader
    module_io_file
    module_io_vtk
    module_io_atoms

    # Services
    uiIO
    uiImageQt

    # Generic Scene
)
set( WARNINGS_AS_ERRORS ON )

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt
