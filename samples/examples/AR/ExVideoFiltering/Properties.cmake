
set( NAME ExVideoFiltering )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    arData
    gui
    guiQt
    servicesReg
    fwlauncher
    appXml
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor
    videoFilter
    videoVLC
    videoQt
    videoTools
    preferences
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExVideoFilteringConfig)
