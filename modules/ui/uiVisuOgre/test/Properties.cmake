
set( NAME uiVisuOgreTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools fwRenderOgre)
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    uiIO
    uiVisuOgre
)

set( CONAN_DEPS
    ${CONAN_QT}
)
