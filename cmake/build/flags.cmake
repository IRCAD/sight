# Replace a string occurrence by another in CMAKE_<LANG>_FLAGS
macro(replace_flags FLAGS_BEFORE FLAGS_AFTER)
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
endmacro()

if(MSVC)
    set(SIGHT_ARCH "AVX2" CACHE STRING "Value passed to /arch: compiler flag. Common value are 'AVX', or 'AVX2' (default)")
else()
    set(SIGHT_ARCH "x86-64-v3" CACHE STRING "Value passed to -march= compiler flag. Common value are 'native', or 'x86-64-v3' (default)")
endif()

mark_as_advanced(SIGHT_ARCH)


# We simply use CMAKE one plus some very basic stuff
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-march=${SIGHT_ARCH};-mtune=generic;-mfpmath=sse>"
    "$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fno-limit-debug-info>" # Needed to debug STL classes
    "$<$<CXX_COMPILER_ID:MSVC>:/favor:blend;/fp:precise;/Qfast_transcendentals;/arch:${SIGHT_ARCH};/MP;/bigobj>"
    "$<$<CONFIG:Debug>:-DDEBUG;-D_DEBUG>"
)

add_compile_definitions(
    "$<$<CXX_COMPILER_ID:MSVC>:/D_ENABLE_EXTENDED_ALIGNED_STORAGE>"
)

# C/C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++ visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Because of PCH, we need also to specify default visibility in compile options
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility=hidden;-fvisibility-inlines-hidden>"
)

# Warning level
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wconversion;-Wno-ignored-qualifiers>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

# AES support is enabled with pragmas on GCC, Clang needs the explicit CLI flag
add_compile_options(
    "$<$<CXX_COMPILER_ID:Clang>:-maes>"
)

# Add better default for debugging to have a better debugging experience
# -ggdb3: allows macro expansion and other useful things
# -gmodules: (Clang) faster debugging information generation
# -ggnu-pubnames: generates data to ease gdb indexing by the linker
add_compile_options(
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-ggdb3;-ggnu-pubnames>"
    "$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RelWithDebInfo,Debug>>:-gmodules>"
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:RelWithDebInfo>>:-ggdb;-gz>"
)

# -fuse-ld=gold will make use of gold linker, which is faster and allows --gdb-index
# -Wl,--gdb-index pass the --gdb-index option to the linker to generate indexes that will speedup gdb start
add_link_options(
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-fuse-ld=gold;-Wl,--gdb-index>"
)

# General linker optimization
add_link_options(
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wl,--as-needed,--sort-common,-O2>"
)

# MSVC need special treatment
if(MSVC)
    # Remove Command-Line Warning D9025
    replace_flags("/W[0-4]" "")

    # Use external/system includes available from Visual Studio 15.6
    # source https://gitlab.kitware.com/cmake/cmake/issues/17904
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
        add_compile_options("/experimental:external;/external:W0")
        set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
    endif()

    if(SIGHT_ENABLE_PCH)
        # Store debug information in the .obj file instead of a PDB.
        replace_flags("/Z[iI]" "/Z7")
    endif()

    # Remove leading and trailing spaces in compile flags.
    # Required to remove unnecessary spaces added after each cmake-configure.
    string(STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)
    string(STRIP "${CMAKE_CXX_FLAGS_RELEASE}" CMAKE_CXX_FLAGS_RELEASE)
    string(STRIP "${CMAKE_CXX_FLAGS_DEBUG}" CMAKE_CXX_FLAGS_DEBUG)
    string(STRIP "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(STRIP "${CMAKE_C_FLAGS}" CMAKE_C_FLAGS)
    string(STRIP "${CMAKE_C_FLAGS_RELEASE}" CMAKE_C_FLAGS_RELEASE)
    string(STRIP "${CMAKE_C_FLAGS_DEBUG}" CMAKE_C_FLAGS_DEBUG)
    string(STRIP "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CMAKE_C_FLAGS_RELWITHDEBINFO)

    # Force set cleaned compile flags.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
endif()

# Color for ninja and Clang on Linux and OSX
if(CMAKE_GENERATOR STREQUAL "Ninja")
    add_compile_options(
        "$<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>"
        "$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>"
    )
endif()
