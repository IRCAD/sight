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
    module_viz_scene3d                # Start the module, allow to use viz_scene3d
    module_viz_scene3dQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    module_service             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticons

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
        sight_module_ui_qt/flatdark.rcc
        sight_module_ui_qt/flatdark.qss
) # Allow dark theme via module_ui_qt
