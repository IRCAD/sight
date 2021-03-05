sight_add_target( io_atoms TYPE LIBRARY )

find_package(CAMP QUIET REQUIRED)
find_package(Boost QUIET COMPONENTS log log_setup REQUIRED)

target_include_directories(io_atoms SYSTEM PRIVATE ${CAMP_INCLUDE_DIRS})
target_link_libraries(io_atoms PRIVATE ${CAMP_LIBRARIES}
                                                ${Boost_LOG_LIBRARY}
                                                ${Boost_LOG_SETUP_LIBRARY})

target_compile_definitions(io_atoms PRIVATE "-DCAMP_COMPILATION")

target_link_libraries(io_atoms PUBLIC 
                      atoms
                      core
                      data
                      io_zip
                      activity
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
