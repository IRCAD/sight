#Fix error with BOOST_JOIN and qt moc
set(CMAKE_AUTOMOC_MOC_OPTIONS "-DBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION")
