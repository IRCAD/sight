
set( NAME ExSParameters )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    fwData
    gui
    guiQt
    appXml
    fwlauncher
    media
    preferences
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES Ex07SParametersConfig)
