
set( NAME Ex07WheelWidget )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg

    gui
    guiQt

    appXml
    fwlauncher

    uiIO
    ioVTK

    maths
    media

    visuVTK
    visuVTKQt
    visuVTKAdaptor
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex07WheelWidget)
