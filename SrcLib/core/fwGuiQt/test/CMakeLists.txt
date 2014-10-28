
fwLoadProperties()

find_package (Boost COMPONENTS thread date_time REQUIRED)
find_package(CppUnit)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
fwInclude(${Boost_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIR})
fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})



