
set( NAME DroidRender )
set( VERSION 0.1 )
set( TYPE APP )
set( PLATFORM DROID)
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    servicesReg
    dataReg
    launcher
    visu
    visuVTK
    visuVTKAndroid
    visuVTKAdaptor
    guiDroid
    io
    ioVTK
    ioAtoms
    ctrlCamp
    gui
)

bundle_param(appXml PARAM_LIST config PARAM_VALUES DroidRenderConfig)

set( JAVA_PACKAGES 
    ndkGui
)
