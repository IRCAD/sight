
set( NAME module_activities )
set( VERSION 0.1 )
set( TYPE MODULE )

set( DEPENDENCIES
    core
    data
    ui_base
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
        module_ui_qt
        )
    add_definitions ( -DKEEP_OLD_SERVICE )
endif()
