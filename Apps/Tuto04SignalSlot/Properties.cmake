
set( NAME Tuto04SignalSlot )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioVTK
    uiIO
    visuVTK
    visuVTKQt
    vtkSimpleMesh # contains a visualization service of mesh.
    launcher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoSignalSlotConfig)
