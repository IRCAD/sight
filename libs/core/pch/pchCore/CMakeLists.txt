add_library(pchCore STATIC pch.cpp)

# Ensure we use the same compile flags with the other targets
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND MSVC)
    restore_cxx_flags(pchCore ${SIGHT_CMAKE_CXX_FLAGS_DEBUG} OFF)
endif()

target_precompile_headers(pchCore PUBLIC ${SIGHT_PCH_HEADERS})

find_package(Filesystem REQUIRED)
find_package(Boost QUIET COMPONENTS system date_time iostreams log log_setup REQUIRED)

target_link_libraries(pchCore PUBLIC std::filesystem)
target_link_libraries(pchCore PUBLIC Boost::date_time Boost::log Boost::log_setup Boost::iostreams)

target_compile_definitions(pchCore PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS)

find_package(glm QUIET REQUIRED)
if(WIN32)
    target_link_libraries(pchCore PRIVATE glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(pchCore SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()
