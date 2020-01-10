
set( NAME ExNotifications )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    gui
    guiQt
    appXml
    fwlauncher
    uiGenericQt
    media
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExNotificationsConfig)
