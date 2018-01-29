
set( NAME Ex03Registration)
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    basicRegistration
    ctrlSelection
    dataReg
    fwlauncher
    gui
    guiQt
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

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex03RegistrationConfig)
