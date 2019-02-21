
set( NAME ExRealSense )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )

set( REQUIREMENTS
    appXml
    fwlauncher

    arDataReg
    dataReg
    servicesReg

    gui
    guiQt

    uiTools

    visuOgre
    visuOgreQt
    visuOgreAdaptor

    videoRealSense
    videoTools

    arMedia
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExRealSense)
