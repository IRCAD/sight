
set( NAME ExRealSense )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )

set( REQUIREMENTS
    appXml
    fwlauncher

    arData
    fwData
    servicesReg

    ctrlCamp

    gui
    guiQt
    media

    uiTools

    visuOgre
    visuOgreQt
    visuOgreAdaptor
    material

    videoRealSense
    videoTools
    videoQt
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExRealSense)
