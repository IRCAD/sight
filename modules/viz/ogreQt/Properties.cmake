
set( NAME module_viz_ogreQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    core
    data
    ui_base
    ui_qt
    viz_ogre
)
set( REQUIREMENTS
    module_viz_ogre
    module_service
)
set( WARNINGS_AS_ERRORS ON )

if(UNIX)
    set(PLUGINS xcbglintegrations)
endif()
