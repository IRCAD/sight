
set( NAME Ex03Registration)
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
    visuVTKVRAdaptor
    ioVTK
    ioITK
    ioData
    uiGenericQt
    uiImageQt
    uiVisu
    uiVisuQt
    ctrlSelection
    uiMeasurement
    uiMeasurementQt
    appXml
    basicRegistration
    ioAtoms
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex03RegistrationConfig)
