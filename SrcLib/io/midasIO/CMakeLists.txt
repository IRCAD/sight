fwLoadProperties()


add_definitions("-DQT_NO_KEYWORDS")


find_package(Boost REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
include(${QT_USE_FILE})


fwForwardInclude(${Boost_INCLUDE_DIRS} ${QT_INCLUDE_DIR} ${EXTERNAL_LIBRARIES}/include)
fwLink(${QT_LIBRARIES})

find_package(CryptoPP REQUIRED)
fwInclude(${CryptoPP_INCLUDE_DIR})
fwLink(${CryptoPP_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
