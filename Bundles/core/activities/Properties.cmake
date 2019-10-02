
set( NAME activities )
set( VERSION 0.1 )
set( TYPE BUNDLE )

set( DEPENDENCIES
    fwCore
    fwActivities
    fwCom
    fwData
    fwDataCamp
    fwGui
    fwMedData
    fwRuntime
    fwServices
    fwTools
    fwMedDataTools
    )
set( REQUIREMENTS
    dataReg
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
set( CONAN_DEPS
    ${CONAN_QT}
    )
