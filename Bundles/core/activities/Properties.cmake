
set( NAME activities )
set( VERSION 0.1 )
set( TYPE BUNDLE )

if(NOT ${QML_APPLICATION})
    set( DEPENDENCIES
        fwCore
        fwActivities
        fwCom
        fwData
        fwDataCamp
        fwGui
        fwGuiQt
        fwMedData
        fwRuntime
        fwServices
        fwTools
        fwMedDataTools
        )

    set( REQUIREMENTS
        guiQt
        dataReg
        servicesReg
        )
    add_definitions ( -DQML_APPLICATION=0 )
else()
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
    add_definitions ( -DQML_APPLICATION=1 )
endif(NOT ${QML_APPLICATION})
set( CONAN_DEPS
    ${CONAN_QT}
    )
