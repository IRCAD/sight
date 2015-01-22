find_package(wxWidgets COMPONENTS base core adv aui)
if(wxWidgets_FOUND)
    fwLoadProperties()
    
    fwUseForwardInclude(
        fwCore
        fwGui
        fwRuntime
        fwTools
    )
    
    foreach(wxDef ${wxWidgets_DEFINITIONS})
        add_definitions(-D${wxDef})
    endforeach()
    
    fwForwardInclude(${wxWidgets_INCLUDE_DIRS})
    fwLink(${wxWidgets_LIBRARIES})    
else(wxWidgets_FOUND)
    message(WARNING "Required package wxWidgets not found, lib 'fwGuiWx' disabled.")
endif(wxWidgets_FOUND)
