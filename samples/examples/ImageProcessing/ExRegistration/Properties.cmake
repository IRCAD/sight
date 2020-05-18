
set( NAME ExRegistration)
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
    uiIO
    uiImageQt
    uiVisuQt
    visuVTK
    visuVTKAdaptor
    visuVTKQt
    ctrlPicking
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExRegistrationConfig)
