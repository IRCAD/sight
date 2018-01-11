
set( NAME calibrationActivityTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwCore
    fwTest
    fwData
    fwMedData
    fwActivities
)
set( REQUIREMENTS
    activities
    calibrationActivity
)

set(CPPUNITTEST_OPTIONS BUNDLE calibrationActivity WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
