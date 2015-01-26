
fwLoadProperties()



find_package (Boost COMPONENTS thread date_time REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})
fwInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
