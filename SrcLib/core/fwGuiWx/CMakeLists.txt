find_package(wxWidgets COMPONENTS base core adv aui)
if(wxWidgets_FOUND)
    fwLoadProperties()
    
    fwUseForwardInclude(
        fwCore
        fwGui
        fwRuntime
        fwTools
    )

    find_package(Boost REQUIRED)
    include(${wxWidgets_USE_FILE})

    fwForwardInclude(${Boost_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
    fwLink(${wxWidgets_LIBRARIES})
else(wxWidgets_FOUND)
    message(WARNING "Required package wxWidgets not found, lib 'fwGuiWx' disabled.")
endif(wxWidgets_FOUND)
