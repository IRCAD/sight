add_library(pchCoreOg STATIC pch.cpp)

target_precompile_headers(pchCoreOg PUBLIC ${SIGHT_PCH_HEADERS})

# Set the debug optimized flags the same way it is done in sight_add_target
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    if(UNIX)
        target_compile_options(pchCoreOg PRIVATE "-Og")
    elseif(MSVC)
        set_fast_debug_cxx_flags(pchCoreOg OFF)
    endif()
endif()

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
