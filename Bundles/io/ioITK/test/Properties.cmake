
set( NAME ioITKTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwCore
    fwTest
    fwTools
    fwRuntime
    fwData
    fwMedData
    fwServices
    fwGui
    fwThread
    fwIO
)
set( REQUIREMENTS ioITK )

set(CPPUNITTEST_OPTIONS BUNDLE ioITK WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
