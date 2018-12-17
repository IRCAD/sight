
set( NAME uiVisuOgreTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools fwRenderOgre)
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    uiIO
    uiVisuOgre
)

set( CONAN_DEPS
    qt/5.11.2@sight/stable
)
