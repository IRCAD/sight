set( NAME Tuto02DataServiceBasic )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml           # XML configurations
    module_ui_qt            # Start the module, load module_ui_qt implementation of module_ui_base

    # Objects declaration
    data
    module_service

    # UI declaration/Actions
    module_ui_base

    # Reader
    module_io_vtk           # Contains the reader and writer for VTK files (image and mesh).

    # Services
    module_viz_sample       # Loads basic rendering service for images and meshes.
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        TutoDataServiceBasic_AppCfg
) # Main application's configuration to launch
