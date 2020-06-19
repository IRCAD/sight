
set( NAME Tuto04SignalSlot )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    ioVTK
    uiIO
    visuVTKQt
    vtkSimpleMesh # contains a visualization service of mesh.
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES tutoSignalSlotConfig)
