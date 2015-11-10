
set( NAME fwTools )
set( VERSION 0.1 )
set( TYPE LIBRARY )
if(ANDROID)
    set( DEPENDENCIES
            fwCore
            fwRuntime
    )
else()
    set( DEPENDENCIES fwCore )
endif()
set( REQUIREMENTS  )

