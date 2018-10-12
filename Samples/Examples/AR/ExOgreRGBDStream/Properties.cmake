
set( NAME ExOgreRGBDStream )
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

    scene2D
    uiTF

    visuOgreAdaptor
    visuOgreQt

    videoTools
    videoOpenni
    videoOpenCV
    videoQt

    arMedia
    media
    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExOgreRGBDStream)
