
set( NAME module_viz_scene3dQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    core
    data
    ui_base
    ui_qt
    viz_scene3d
)
set( REQUIREMENTS
    module_viz_scene3d
    module_service
)
set( WARNINGS_AS_ERRORS ON )

if(UNIX)
    set(PLUGINS xcbglintegrations)
endif()
