
set( NAME Tuto03DataService )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
    ioVTK
    uiIO # contains services to show dialogs for reader/writer selection
    visuOgreBasic
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES tutoDataServiceConfig)
