set( NAME Tuto02GenericSceneCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    data
    service
)
set( REQUIREMENTS
    module_ui_base             # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, allow dark theme
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_viz_ogreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    module_service             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticon

    # Reader
    module_io_matrix
    module_io_vtk
    module_io_atoms

    # Services

    # Generic Scene
)
set( WARNINGS_AS_ERRORS ON )

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        module_ui_qt-0.1/flatdark.rcc
        module_ui_qt-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt
