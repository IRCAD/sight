
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

set(CPPUNITTEST_OPTIONS BUNDLE uiVisuOgre WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set( CONAN_DEPS
    qt/5.11.2@sight/stable
)