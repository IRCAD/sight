find_package(wxWidgets COMPONENTS base core adv aui)
if(wxWidgets_FOUND)
    fwLoadProperties()

    find_package (Boost COMPONENTS thread date_time REQUIRED)
    find_package(CppUnit)
    fwInclude(${Boost_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIR})
    fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})

else(wxWidgets_FOUND)
    message(WARNING "Required package wxWidgets not found, lib 'fwGuiWxTest' disabled.")
endif(wxWidgets_FOUND)
