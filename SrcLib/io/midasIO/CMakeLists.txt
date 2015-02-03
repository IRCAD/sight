fwLoadProperties()

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(CryptoPP REQUIRED)
fwInclude(${CryptoPP_INCLUDE_DIR})
fwLink(${CryptoPP_LIBRARIES})

find_package(Qt5 COMPONENTS Core Gui Network REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

fwForwardInclude(${EXTERNAL_LIBRARIES}/include)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

