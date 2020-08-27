
set( NAME ExOgreRGBDStream )
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

    media
    preferences
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExOgreRGBDStream)
