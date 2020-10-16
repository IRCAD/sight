
set( NAME Tuto04SignalSlot )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
    ioVTK
    uiIO
    visuOgreBasic # contains a visualization service of mesh.
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES tutoSignalSlotConfig)
