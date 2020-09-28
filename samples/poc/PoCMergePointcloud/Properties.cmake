
set( NAME PoCMergePointcloud )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arData
    ctrlCamp
    fwData
    fwlauncher
    gui
    guiQt
    ioAtoms
    ioCalibration
    ioData
    ioData
    ioVTK
    maths
    media
    opDepthMap
    preferences
    servicesReg
    uiIO
    videoOrbbec
    videoQt
    videoTools
    visuOgre
    visuOgreAdaptor
    visuOgreAdaptor
    visuOgreQt
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES PoCMergePointcloud)

