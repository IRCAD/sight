
set( NAME activities )
set( VERSION 0.1 )
set( TYPE MODULE )

set( DEPENDENCIES
    core
    fwActivities
    data
    fwGui
    fwMedData
    fwServices
    fwMedDataTools
)
set( REQUIREMENTS
    data
    servicesReg
)

if(NOT ${ENABLE_QML_APPLICATION})

    list(APPEND DEPENDENCIES
        fwGuiQt
        )
    list(APPEND REQUIREMENTS
        guiQt
        )
    add_definitions ( -DKEEP_OLD_SERVICE )
endif()
