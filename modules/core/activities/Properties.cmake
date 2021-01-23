
set( NAME activities )
set( VERSION 0.1 )
set( TYPE MODULE )

set( DEPENDENCIES
    core
    fwActivities
    fwData
    fwGui
    fwMedData
    fwServices
    fwMedDataTools
)
set( REQUIREMENTS
    fwData
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
