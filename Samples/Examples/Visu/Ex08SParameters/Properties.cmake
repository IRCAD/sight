
set( NAME Ex08SParameters )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    appXml
    fwlauncher
    media
    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex07SParametersConfig)
