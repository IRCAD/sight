sight_add_target(pchCoreOg TYPE LIBRARY FAST_DEBUG ON)

find_package(Filesystem REQUIRED)
find_package(Boost QUIET COMPONENTS system date_time iostreams log log_setup REQUIRED)

target_link_libraries(pchCoreOg PUBLIC std::filesystem)
target_link_libraries(pchCoreOg PUBLIC Boost::date_time Boost::log Boost::log_setup Boost::iostreams)

target_compile_definitions(pchCoreOg PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS)

find_package(glm QUIET REQUIRED)
if(WIN32)
    target_link_libraries(pchCoreOg PRIVATE glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(pchCoreOg SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()
