
fwLoadProperties()

find_package (Boost REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})
find_package(CppUnit)
fwInclude(${Boost_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIR})
fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})

