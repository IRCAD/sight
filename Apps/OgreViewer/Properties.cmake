
set( NAME OgreViewer )
set( VERSION 0.3 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    preferences             # Start the bundle, load file location or window preferences
    visuOgre                # Start the bundle, redirect Ogre's log to Sight log
    material                # Start the bundle, load Ogre's materials
    visuVTKQt               # Start the bundle, allow vtk to be displayed in QT.

    visuOgreQt              # Allow Ogre to be displayed in QT.

    # Objects declaration
    fwData
    fwMedData

    # UI declaration/Actions
    gui
    media
    arMedia
    guiQt

    # Configuration launchers
    fwServices
    ogreConfig

    # Services
    ctrlSelection
    uiVisuOgre
    uiIO
    ioAtoms
    videoTools
    uiImageQt
    videoQt
    ctrlCamp

    # Generic Scene
    fwRenderOgre
    fwRenderVTK
    visuOgreAdaptor
    visuVTKAdaptor
    visuVTKVRAdaptor

    arDataReg
    dataReg
    servicesReg
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES OgreViewer_Extension)
