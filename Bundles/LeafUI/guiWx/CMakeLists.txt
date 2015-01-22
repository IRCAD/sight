find_package(wxWidgets COMPONENTS base core)
if(wxWidgets_FOUND)
    fwLoadProperties()
    
    fwUseForwardInclude(
          fwActivities
          fwGuiWx
          fwRuntime
          fwServices
          fwTools
          gui
    )
    
    foreach(wxDef ${wxWidgets_DEFINITIONS})
        add_definitions(-D${wxDef})
    endforeach()

    fwForwardInclude(${wxWidgets_INCLUDE_DIRS})
    fwLink(${wxWidgets_LIBRARIES})
else(wxWidgets_FOUND)
    message(WARNING "Required package wxWidgets not found, bundle 'guiWx' disabled.")
endif(wxWidgets_FOUND)
