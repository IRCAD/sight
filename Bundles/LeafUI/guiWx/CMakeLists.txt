find_package(wxWidgets COMPONENTS core)
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

    find_package(Boost REQUIRED)
    include(${wxWidgets_USE_FILE})

    fwForwardInclude(${Boost_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
    fwLink(${wxWidgets_LIBRARIES})
else(wxWidgets_FOUND)
    message(WARNING "Required package wxWidgets not found, bundle 'guiWx' disabled.")
endif(wxWidgets_FOUND)
