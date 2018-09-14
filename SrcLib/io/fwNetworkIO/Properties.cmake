
set( NAME fwNetworkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwTools
    fwThread
    fwData
    fwMedData
    fwCom
    fwGui
    fwGuiQt
    )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    qt/5.11.1@fw4spl/stable
)