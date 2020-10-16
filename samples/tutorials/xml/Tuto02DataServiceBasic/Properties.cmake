
set( NAME Tuto02DataServiceBasic )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
    ioVTK           # contains the reader and writer for VTK files (image and mesh).
    visuOgreBasic   # loads basic rendering services for images and meshes.
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES tutoDataServiceBasicConfig)
