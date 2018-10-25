
set( NAME gui )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwDataTools
    fwGui
    fwRuntime
    fwServices
    fwTools
    fwThread
    fwJobs
    fwMedData
    fwDataCamp
    fwActivities
)
set( REQUIREMENTS
    dataReg
    servicesReg
)

set( CONAN_DEPS
    boost/1.67.0@sight/stable
)