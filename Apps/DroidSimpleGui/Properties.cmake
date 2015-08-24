
set( NAME DroidSimpleGui )
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
    vtkSimpleMesh
    guiDroid
    io
    ioVTK
)
set( PLATFORM DROID)
set( JAVA_PACKAGES 
    ndkgui
)

bundle_param(appXml PARAM_LIST config PARAM_VALUES DroidSimpleGuiConfig)
