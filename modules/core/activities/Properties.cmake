
set( NAME module_activities )
set( VERSION 0.1 )
set( TYPE MODULE )

set( DEPENDENCIES
    core
    data
    base
    services
)
set( REQUIREMENTS
    data
    module_services
)

if(NOT ${ENABLE_QML_APPLICATION})

    list(APPEND DEPENDENCIES
        )
    list(APPEND REQUIREMENTS
        qt
        )
    add_definitions ( -DKEEP_OLD_SERVICE )
endif()
