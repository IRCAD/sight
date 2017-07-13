
set( NAME Ex03Registration)
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml2
    basicRegistration
    ctrlSelection
    dataReg
    fwlauncher
    gui
    guiQt
    io
    ioAtoms
    ioData
    ioITK
    ioVTK
    servicesReg
    uiGenericQt
    uiIO
    uiImageQt
    uiMeasurement
    uiVisu
    uiVisuQt
    visuVTK
    visuVTKAdaptor
    visuVTKQt

)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex03RegistrationConfig)
