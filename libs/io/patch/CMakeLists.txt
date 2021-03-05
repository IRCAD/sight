sight_add_target( io_patch TYPE LIBRARY PCH pchData )


target_link_libraries(io_patch PUBLIC 
                      atoms
                      io_atoms
                      core
                      data
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
