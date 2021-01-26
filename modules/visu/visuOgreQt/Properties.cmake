
set( NAME visuOgreQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    core
    data
    fwGui
    fwGuiQt
    fwRenderOgre
)
set( REQUIREMENTS
    visuOgre
    module_services
)
set( WARNINGS_AS_ERRORS ON )

if(UNIX)
    set(PLUGINS xcbglintegrations)
endif()
