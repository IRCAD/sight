fwLoadProperties()


find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})
fwForwardInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})
