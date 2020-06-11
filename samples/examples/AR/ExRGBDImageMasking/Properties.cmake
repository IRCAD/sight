
set( NAME ExRGBDImageMasking )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    fwlauncher
    arDataReg
    arPatchMedicalData
    patchMedicalData
    dataReg
    dataManagerConfig
    servicesReg
    ctrlCamp
    gui
    guiQt
    media
    ioActivity
    ioAtoms
    ioData
    material
    maths
    uiIO
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    videoRealSense
    videoTools
    videoQt
    depthSegmentation
    )

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExRGBDImageMasking)
