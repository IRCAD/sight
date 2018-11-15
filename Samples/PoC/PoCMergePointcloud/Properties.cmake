
set( NAME PoCMergePointcloud )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arDataReg
    ctrlCamp
    dataReg
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

bundleParam(appXml PARAM_LIST config PARAM_VALUES PoCMergePointcloud)

