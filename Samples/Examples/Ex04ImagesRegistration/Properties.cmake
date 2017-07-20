
set( NAME Ex04ImagesRegistration)
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    io
    fwlauncher
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    ioVTK
    ioITK
    ioData
    uiImageQt
    uiVisu
    uiVisuQt
    ctrlSelection
    appXml2
    basicRegistration
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex04ImagesRegistrationConfig)
