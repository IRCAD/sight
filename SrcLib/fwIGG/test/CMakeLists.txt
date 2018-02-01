
fwLoadProperties()

# Boost shouldn't be necessary but not linking it results in weird linker errors (duplicate symbols).
find_package(Boost QUIET COMPONENTS regex thread system REQUIRED)
find_package(ZLIB QUIET REQUIRED )
find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    ${ZLIB_INCLUDE_DIRS}
)
fwLink(
    # Boost libraries need to be linked before cgogn (who depends on them)
    # otherwise we end up having duplicate symbols on windows builds.
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${cgogn_LIBRARY}
    ${ZLIB_LIBRARIES}
)
