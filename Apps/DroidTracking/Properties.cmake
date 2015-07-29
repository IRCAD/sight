
set( NAME DroidTracking )
set( VERSION 0.1 )
set( TYPE APP )
set( PLATFORM DROID )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    appXml 
    servicesReg 
    dataReg
    extDataReg
    launcher 
    ctrlCamera
    arDataReg
    visuVTKAndroid
    visuVTKDroidAdaptor
    visuVTKARAdaptor
    visuVideoAndroid
    visu
    visuVTK
    videoTools
    vtkSimpleMesh
    io
    ioVTK
    tracker
    trackerAruco
)

bundle_param(appXml PARAM_LIST config PARAM_VALUES DroidTrackingConfig )