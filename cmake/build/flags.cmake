# Replace a string occurrence by another in CMAKE_<LANG>_FLAGS
macro(replace_flags FLAGS_BEFORE FLAGS_AFTER)
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_CXX_FLAGS_RELWITHDEBINFO
                         "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
    )
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REGEX REPLACE "${FLAGS_BEFORE}" "${FLAGS_AFTER}" CMAKE_C_FLAGS_RELWITHDEBINFO
                         "${CMAKE_C_FLAGS_RELWITHDEBINFO}"
    )
endmacro()

if(MSVC)
    set(SIGHT_ARCH "AVX2" CACHE STRING
                                "Value passed to /arch: compiler flag. Common value are 'AVX', or 'AVX2' (default)"
    )
else()
    set(SIGHT_ARCH "x86-64-v3"
        CACHE STRING "Value passed to -march= compiler flag. Common value are 'native', or 'x86-64-v3' (default)"
    )
endif()

mark_as_advanced(SIGHT_ARCH)

if(MSVC)
    # Add a global definition to silence a windows warning when using boost, saying _WIN32_WINNT is not defined.
    # Beware this must match the value used to compile BOOST, otherwise libraries that uses Boost::Log will NOT link.
    add_compile_definitions(WINVER=0x0601)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(_WIN32_WINNT=0x0601)
    add_compile_definitions(BOOST_USE_WINAPI_VERSION=0x0601)
    # We need this because of OgreUTFString.h, which is removed in more recent Ogre versions
    add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

    # Basically needed for DCMTK
    add_compile_options("/Zc:__cplusplus")

    # Add support for __VA_OPT__
    add_compile_options("/Zc:preprocessor")
endif()

# Add a global compile definition to help strip __FILE__ to show a relative file path
add_compile_definitions(SIGHT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# We simply use CMAKE one plus some very basic stuff
# On MSVC, we also use absolute path for __FILE__ macro (/FC), otherwise paths in log are not stripped.
add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-march=${SIGHT_ARCH};-mtune=generic;-mfpmath=sse>"
    "$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:-fno-limit-debug-info>" # Needed to debug STL classes
    "$<$<CXX_COMPILER_ID:MSVC>:/favor:blend;/fp:precise;/Qfast_transcendentals;/arch:${SIGHT_ARCH};/bigobj;/FC>"
    "$<$<CONFIG:Debug>:-DDEBUG;-D_DEBUG>"
)

find_package(OpenMP QUIET REQUIRED)

if(OPENMP_CXX_FOUND)
    add_compile_options(${OpenMP_CXX_FLAGS})
endif()

add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:_ENABLE_EXTENDED_ALIGNED_STORAGE>")

# C/C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++ visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Because of PCH, we need also to specify default visibility in compile options
add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility=hidden;-fvisibility-inlines-hidden>")

# Warning level
add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wconversion>" "$<$<CXX_COMPILER_ID:MSVC>:/W4>")

# Disable "undefined-var-template" for Clang, because it only helps to diagnose early linking errors when some
# specialized template are defined in another translation unit, and the "fix" would imply a lot of boilerplate code to
# declare the missing definitions as "extern". Because, in case of wrong code or bad linking, linking errors will anyway
# occur, the internet consensus about this, is to disable it, to avoid boilerplate code and to have the same behavior as
# GCC or MSVC.
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-Wno-undefined-var-template>")

# AES support is enabled with pragmas on GCC, Clang needs the explicit CLI flag
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-maes>")

# Add better default for debugging to have a better debugging experience
# Don't do it if we use clang-tidy, because "-gmodules" will make clang-tidy crash
# -ggdb3: allows macro expansion and other useful things
# -gmodules: (Clang) faster debugging information generation
# -ggnu-pubnames: generates data to ease gdb indexing by the linker
add_compile_options(
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-ggdb3;-ggnu-pubnames>"
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:RelWithDebInfo>>:-ggdb;-gz>"
)

# -gmodules is problematic for clang-tidy
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS AND NOT SIGHT_ENABLE_CLANG_TIDY)
    add_compile_options("$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RelWithDebInfo,Debug>>:-gmodules>")
elseif(CXX_COMPILER_ID STREQUAL "Clang" AND (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL
                                                                                  "RelWithDebInfo")
)
    message(WARNING "-gmodules flag is incompatible with Clang-tidy, it will not be added.")
endif()

# -fuse-ld=gold will make use of gold linker, which is faster and allows --gdb-index
add_link_options("$<$<CXX_COMPILER_ID:GNU>:-fuse-ld=gold>" "$<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld>")

# -Wl,--gdb-index pass the --gdb-index option to the linker to generate indexes that will speedup gdb start
add_link_options("$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-Wl,--gdb-index>")

# General linker optimization
add_link_options(
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wl,--as-needed>"
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wl,--sort-common>"
    "$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wl,-O2>"
)

# Coverage (only supported for GCC and Clang)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    option(SIGHT_ENABLE_COVERAGE "Enable coverage information generation" OFF)
    mark_as_advanced(SIGHT_ENABLE_COVERAGE)
endif()

if(SIGHT_ENABLE_COVERAGE)
    add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>")
    add_link_options("$<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>")

    if(CMAKE_COMPILER_IS_GNUCXX)
        link_libraries(gcov)
    endif()
endif()

# MSVC need special treatment
if(MSVC)
    # Remove Command-Line Warning D9025
    replace_flags("/W[0-4]" "")

    # Use external/system includes available from Visual Studio 15.6
    # source https://gitlab.kitware.com/cmake/cmake/issues/17904
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
        add_compile_options("/experimental:external;/external:W0")
        set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
    endif()

    # Store debug information in the .obj file instead of a PDB.
    replace_flags("/Z[iI]" "/Z7")

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
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)

    # On MSVC, we want different optimizations depending on the target
    # CMake does allow us to override CXX_FLAGS, so we reset them here and
    # restore them later, modified or not, in restore_cxx_flags()
    string(REGEX REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG_INIT ${CMAKE_CXX_FLAGS_DEBUG_INIT})
    set(SIGHT_CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG_INIT} CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
endif()

# Color for ninja and Clang on Linux and OSX
if(CMAKE_GENERATOR STREQUAL "Ninja")
    add_compile_options(
        "$<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>" "$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>"
    )
endif()

# Restore default compiler flags
function(restore_cxx_flags SIGHT_TARGET CXX_FLAGS OBJECT_LIBRARY)

    string(STRIP "${CXX_FLAGS}" SIGHT_CXX_FLAGS)
    string(REPLACE " " ";" SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS})

    get_target_property(TARGET_TYPE ${SIGHT_TARGET} TYPE)
    # Skip libraries without code
    if(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
        target_compile_options(${SIGHT_TARGET} PRIVATE ${SIGHT_CXX_FLAGS})

        if(OBJECT_LIBRARY)
            target_compile_options(${SIGHT_TARGET}_obj PRIVATE ${SIGHT_CXX_FLAGS})
            if(TARGET ${SIGHT_TARGET}_obj_PCH_OBJ)
                target_compile_options(${SIGHT_TARGET}_obj_PCH_OBJ PRIVATE ${SIGHT_CXX_FLAGS})
            endif()
        elseif(TARGET ${SIGHT_TARGET}_PCH_OBJ)
            target_compile_options(${SIGHT_TARGET}_PCH_OBJ PRIVATE ${SIGHT_CXX_FLAGS})
        endif()
    endif()
endfunction()

# Enable "fast" debug flags
function(set_fast_debug_cxx_flags SIGHT_TARGET OBJECT_LIBRARY)

    # /RTC[1csu] is incompatible with optimization
    string(REGEX REPLACE "/RTC[1csu]+" "" SIGHT_CXX_FLAGS ${SIGHT_CMAKE_CXX_FLAGS_DEBUG})

    # Avoid errors and command-line Warning D9025
    string(REGEX REPLACE "/Ob[012]" "" SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS})
    string(REGEX REPLACE "/O[x12d]" "" SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS})

    set(SIGHT_CXX_FLAGS "${SIGHT_CXX_FLAGS} /Ob1 /Ox /Oy-")

    restore_cxx_flags(${SIGHT_TARGET} ${SIGHT_CXX_FLAGS} ${OBJECT_LIBRARY})
endfunction()
