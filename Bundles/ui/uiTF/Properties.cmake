
set( NAME uiTF )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES 
    fwTools
    fwCore
    fwData
    fwDataTools
    fwGui
    fwGuiQt
    fwRuntime
    fwServices
    fwIO
)
set( REQUIREMENTS 
    ioAtoms
)

set( CONAN_DEPS
    qt/5.11.1@fw4spl/stable
)