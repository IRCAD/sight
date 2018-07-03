
set( NAME ExVideoFiltering )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    arDataReg
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
    arMedia
    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExVideoFilteringConfig)
