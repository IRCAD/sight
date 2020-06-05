
set( NAME TutoSceneQt3D )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    fwlauncher
    appXml
    visuQt3D
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES TutoSceneQt3D)
