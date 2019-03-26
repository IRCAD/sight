
set( NAME ExVLCVideoStreaming )
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
    uiPreferences
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor
    videoQt
    videoVLC
    videoTools
    media
    arMedia
    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExVLCVideoStreamingConfig)
