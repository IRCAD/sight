
set( NAME DroidRender )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    servicesReg
    dataReg
    launcher
    visu
    visuVTKAndroid
    visuVTKDroidAdaptor
    visuVTKAdaptor
    vtkSimpleMesh
    guiDroid
    arGuiDroid
    io
    ioAtoms
    ioVTK
    fwMedData
    ctrlCamp
)
set( PLATFORM DROID)

bundle_param(appXml PARAM_LIST config PARAM_VALUES DroidRenderConfig)
