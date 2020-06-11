
set( NAME ExOpenvslam )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    media
    appXml
    arDataReg
    ctrlCamp
    dataReg
    fwlauncher
    gui
    guiQt
    ioCalibration
    ioData
    ioVTK
    uiTools
    uiIO
    material
    servicesReg
    openvslamTracker
    videoOpenCV
    videoQt
    videoTools
    visuOgre
    visuOgreAdaptor
    visuOgreQt
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExOpenvslamConfig)
