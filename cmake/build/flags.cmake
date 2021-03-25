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

# We simply use CMAKE one plus some very basic stuff
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU>:-march=sandybridge;-mtune=generic;-mfpmath=sse>"
    "$<$<CXX_COMPILER_ID:Clang>:-march=sandybridge;-mtune=generic;-mfpmath=sse>"
    "$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fno-limit-debug-info>" # Needed to debug STL classes
    "$<$<CXX_COMPILER_ID:MSVC>:/favor:blend;/fp:precise;/Qfast_transcendentals;/arch:AVX;/MP;/bigobj>"
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
# On macOS fwComTest fails without -fvisibility-ms-compat and -fvisibility=hidden
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU>:-fvisibility=hidden;-fvisibility-inlines-hidden>"
    "$<$<CXX_COMPILER_ID:Clang>:-fvisibility=hidden;-fvisibility-inlines-hidden>"
)

# Warning level
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU>:-Wall;-Wextra;-Wconversion;-Wno-unused-parameter;-Wno-ignored-qualifiers>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wconversion;-Wno-unused-parameter;-Wno-ignored-qualifiers>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

# AES support is enabled with pragmas on GCC, Clang needs the explicit CLI flag
add_compile_options(
    "$<$<CXX_COMPILER_ID:Clang>:-maes>"
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
