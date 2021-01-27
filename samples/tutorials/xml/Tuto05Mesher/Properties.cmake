set( NAME Tuto05Mesher )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_guiQt                   # Start the module, load qt implementation of module_gui

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style

    # Reader
    ioVTK

    # Services
    uiIO
    visuBasic   # contains a visualization service of mesh.
    opVTKMesh       # provides services to generate a mesh from an image.
)

moduleParam(module_guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_guiQt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto05Mesher_AppCfg
) # Main application's configuration to launch
