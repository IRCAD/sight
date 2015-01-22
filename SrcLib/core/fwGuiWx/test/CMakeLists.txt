find_package(wxWidgets COMPONENTS base core adv aui)
if(wxWidgets_FOUND)
    fwLoadProperties()

    find_package(CppUnit)
    find_package(Boost COMPONENTS chrono REQUIRED)
    
    foreach(wxDef ${wxWidgets_DEFINITIONS})
        add_definitions(-D${wxDef})
    endforeach()
   
    fwInclude(
        ${CPPUNIT_INCLUDE_DIR}
        ${wxWidgets_INCLUDE_DIRS}
    )
    fwLink(
        ${CPPUNIT_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${wxWidgets_LIBRARIES}
    )

else(wxWidgets_FOUND)
    message(WARNING "Required package wxWidgets not found, lib 'fwGuiWxTest' disabled.")
endif(wxWidgets_FOUND)
