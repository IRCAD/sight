
set( NAME TutoTrianConverterCtrl )
set( VERSION 0.1 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    fwRuntime
    fwServices
    fwData
    fwCore
)
set( REQUIREMENTS
    gui
    dataReg
    servicesReg
    ioVTK
    ioData
)
set( CONAN_DEPS
    boost/1.67.0@sight/stable
)