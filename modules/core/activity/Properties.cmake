
set( NAME module_activity )
set( VERSION 0.1 )
set( TYPE MODULE )

set( DEPENDENCIES
    core
    data
    ui_base
    service
)
set( REQUIREMENTS
    data
    module_service
)

if(NOT ${ENABLE_QML_APPLICATION})

    list(APPEND DEPENDENCIES
        )
    list(APPEND REQUIREMENTS
        module_ui_qt
        )
endif()
