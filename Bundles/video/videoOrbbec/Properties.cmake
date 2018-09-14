set( NAME videoOrbbec )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        arData
        arServices
        fwData
        fwServices
        fwVideoQt
    )
set( REQUIREMENTS )
set( CONAN_DEPS
    qt/5.11.1@fw4spl/stable
    openni/2.2.0-rev-958951f@fw4spl/stable
)