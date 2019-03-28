
set( NAME guiQmlTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwTest
    fwCore
    fwData
    fwRuntime
    fwServices
)
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQml
)

set( CONAN_DEPS
    qt/5.11.2@sight/stable
)
