
set( NAME guiQtTest )
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
    guiQt
    ioVTK
    visuVTKQt
    vtkSimpleNegato
    Tuto01Basic
    Tuto02DataServiceBasic
    appXml
)

set(CPPUNITTEST_OPTIONS BUNDLE guiQt WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set( CONAN_DEPS
    qt/5.11.1@fw4spl/stable
)