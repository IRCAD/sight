# Replace a string occurrence by another in CMAKE_<LANG>_FLAGS
macro(replace_flags BEFORE AFTER)
    # Replace
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT
                         "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}"
    )
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_CXX_FLAGS_MINSIZEREL_INIT "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}")

    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_RELEASE_INIT}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_DEBUG_INIT "${CMAKE_C_FLAGS_DEBUG_INIT}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_RELWITHDEBINFO_INIT
                         "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}"
    )
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
    string(REGEX REPLACE "${BEFORE}" "${AFTER}" CMAKE_C_FLAGS_MINSIZEREL_INIT "${CMAKE_C_FLAGS_MINSIZEREL_INIT}")

    # Strip
    string(STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)
    string(STRIP "${CMAKE_CXX_FLAGS_INIT}" CMAKE_CXX_FLAGS_INIT)
    string(STRIP "${CMAKE_CXX_FLAGS_RELEASE}" CMAKE_CXX_FLAGS_RELEASE)
    string(STRIP "${CMAKE_CXX_FLAGS_RELEASE_INIT}" CMAKE_CXX_FLAGS_RELEASE_INIT)
    string(STRIP "${CMAKE_CXX_FLAGS_DEBUG}" CMAKE_CXX_FLAGS_DEBUG)
    string(STRIP "${CMAKE_CXX_FLAGS_DEBUG_INIT}" CMAKE_CXX_FLAGS_DEBUG_INIT)
    string(STRIP "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(STRIP "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}" CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT)
    string(STRIP "${CMAKE_CXX_FLAGS_MINSIZEREL}" CMAKE_CXX_FLAGS_MINSIZEREL)
    string(STRIP "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}" CMAKE_CXX_FLAGS_MINSIZEREL_INIT)

    string(STRIP "${CMAKE_C_FLAGS}" CMAKE_C_FLAGS)
    string(STRIP "${CMAKE_C_FLAGS_INIT}" CMAKE_C_FLAGS_INIT)
    string(STRIP "${CMAKE_C_FLAGS_RELEASE}" CMAKE_C_FLAGS_RELEASE)
    string(STRIP "${CMAKE_C_FLAGS_RELEASE_INIT}" CMAKE_C_FLAGS_RELEASE_INIT)
    string(STRIP "${CMAKE_C_FLAGS_DEBUG}" CMAKE_C_FLAGS_DEBUG)
    string(STRIP "${CMAKE_C_FLAGS_DEBUG_INIT}" CMAKE_C_FLAGS_DEBUG_INIT)
    string(STRIP "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CMAKE_C_FLAGS_RELWITHDEBINFO)
    string(STRIP "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}" CMAKE_C_FLAGS_RELWITHDEBINFO_INIT)
    string(STRIP "${CMAKE_C_FLAGS_MINSIZEREL}" CMAKE_C_FLAGS_MINSIZEREL)
    string(STRIP "${CMAKE_C_FLAGS_MINSIZEREL_INIT}" CMAKE_C_FLAGS_MINSIZEREL_INIT)

    # Save back
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_DEBUG_INIT "${CMAKE_C_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_MINSIZEREL_INIT "${CMAKE_C_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
endmacro()

#
# CMake flags configuration
#

# C/C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# C++ visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Use /Z7 (embedded) debug information
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>" CACHE STRING "Z7 forced" FORCE)

#
# Definitions
#

# Add a global compile definition to help strip __FILE__ to show a relative file path
add_compile_definitions("SIGHT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

# Add Debug global definition
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG;_DEBUG>")
add_compile_definitions("$<$<CONFIG:MinSizeRel,Release,RelWithDebInfo>:NDEBUG>")

# Add a global definition to silence a windows warning when using boost, saying _WIN32_WINNT is not defined.
# Beware this must match the value used to compile BOOST, otherwise libraries that uses Boost::Log will NOT link.
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:WINVER=0x0601>")
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:_WIN32_WINNT=0x0601>")
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:BOOST_USE_WINAPI_VERSION=0x0601>")

# Avoid conflicts with std::min / std::max when including windows.h
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>")

# Suppress MSVC specific warnings
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:_ENABLE_EXTENDED_ALIGNED_STORAGE>")

# We need this because of OgreUTFString.h, which is removed in more recent Ogre versions
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING>")

#
# Options: Warnings
#

# Warning level
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/W4>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-W4>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wconversion>")

# Disable "undefined-var-template" for Clang, because it only helps to diagnose early linking errors when some
# specialized template are defined in another translation unit, and the "fix" would imply a lot of boilerplate code to
# declare the missing definitions as "extern". Because, in case of wrong code or bad linking, linking errors will anyway
# occur, the internet consensus about this, is to disable it, to avoid boilerplate code and to have the same behavior as
# GCC or MSVC.
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-Wno-undefined-var-template>")

#
# Options: Debug
#

# Add better default for debugging to have a better debugging experience
# Don't do it if we use clang-tidy, because "-gmodules" will make clang-tidy crash
# -ggdb3: allows macro expansion and other useful things
# -gmodules: (Clang) faster debugging information generation
# -ggnu-pubnames: generates data to ease gdb indexing by the linker
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-ggdb3;-ggnu-pubnames>")

# Compress debug symbols for RelWithDebInfo
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:RelWithDebInfo>>:-ggdb;-gz>")

# -gmodules allows faster debugging but is problematic for clang-tidy-14
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS AND NOT SIGHT_ENABLE_CLANG_TIDY)
    add_compile_options("$<$<AND:$<COMPILE_LANG_AND_ID:CXX,Clang>,$<CONFIG:RelWithDebInfo,Debug>>:-gmodules>")
    add_compile_options("$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang>,$<CONFIG:RelWithDebInfo,Debug>>:-gmodules>")
endif()

# Needed to debug STL classes with Clang
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:Clang>,$<COMPILE_LANGUAGE:C,CXX>>:-fno-limit-debug-info>")

#
# Options: Architecture
#

if(MSVC)
    set(SIGHT_ARCH "AVX2" CACHE STRING "'/arch' flag: 'AVX' or 'AVX2' (default)")
else()
    set(SIGHT_ARCH "x86-64-v3" CACHE STRING "'-march' flag: 'native', 'x86-64-v2' or 'x86-64-v3' (default)")
endif()

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/arch:${SIGHT_ARCH};/favor:blend>")
add_compile_options(
    "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-arch:${SIGHT_ARCH} -favor:blend>"
)
add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-march=${SIGHT_ARCH};-mtune=generic>")

# Floating point configuration
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/fp:precise;/Qfast_transcendentals>")
add_compile_options(
    "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-fp:precise;-Qfast_transcendentals>"
)
add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-mfpmath=sse>")

#
# Options: features, workarounds
#

# On MSVC, we also use absolute path for __FILE__ macro (/FC), otherwise paths in log are not stripped.
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/FC>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-FC>")

# Force "big" object generation, otherwise some files won't compile
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/bigobj>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-bigobj>")

# Basically needed for DCMTK
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/Zc:__cplusplus>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-Zc:__cplusplus>")

# Add support for __VA_OPT__
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/Zc:preprocessor>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-Zc:preprocessor>")

# AES support is enabled with pragmas on GCC, Clang needs the explicit CLI flag
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-maes>")

# OpenMP
find_package(OpenMP QUIET REQUIRED)

if(OPENMP_CXX_FOUND)
    add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:${OpenMP_CXX_FLAGS}>")
endif()

#
# Linking
#

# -fuse-ld=gold will make use of gold linker, which is faster and allows --gdb-index
# -fuse-ld=lld is even a lot faster but only guaranteed to be installed with clang
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
if(CXX_COMPILER_ID STREQUAL "GNU" OR CXX_COMPILER_ID STREQUAL "Clang")
    option(SIGHT_ENABLE_COVERAGE "Enable coverage information generation" OFF)
    mark_as_advanced(SIGHT_ENABLE_COVERAGE)
endif()

if(NOT TARGET coverage)
    if(SIGHT_ENABLE_COVERAGE)
        add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>")
        add_link_options("$<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>")

        if(CMAKE_COMPILER_IS_GNUCXX)
            link_libraries(gcov)
        endif()

        string(
            CONCAT coverage_script
                   "gcovr -j$RUNNER_THREADS -r .. --filter ../libs --filter ../modules --exclude '.*test.*' --html"
                   " --html-details coverage/index.html --xml coverage/cobertura-coverage.xml --print-summary ."
                   "| grep lines"
                   [=[| sed -sE 's/.* \((.*) out of (.*)\)/\1\/\2/']=]
                   "| xargs -i echo 'scale=4;a={}*100;scale=2;a/1'"
                   "| bc"
                   "| xargs -i echo 'lines: {}%'"
        )
        add_custom_target(
            coverage
            COMMAND mkdir -p coverage
            COMMAND sh -c "${coverage_script}"
            COMMENT "Generating coverage reports..."
            VERBATIM
        )
        unset(coverage_script)
    elseif(CXX_COMPILER_ID STREQUAL "GNU" OR CXX_COMPILER_ID STREQUAL "Clang")
        add_custom_target(
            coverage
            COMMAND
                echo "This build directory isn\\'t configured with coverage instrumentation. Please configure it with"
                "SIGHT_ENABLE_COVERAGE=ON to be able to generate coverage reports." >&2
            COMMAND false
            COMMENT "Generating coverage reports..."
        )
    endif()
endif()

# MSVC need special treatment
if(MSVC)
    # Remove Command-Line Warning D9025
    replace_flags("/W[0-4]" "")

    # CMAKE_MSVC_DEBUG_INFORMATION_FORMAT doesn't always work
    replace_flags("/Z[iI]" "/Z7")

    # Use external/system includes available from Visual Studio 15.6
    # source https://gitlab.kitware.com/cmake/cmake/issues/17904
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
        # cmake-lint: disable=C0301
        # Last space is mandatory
        add_compile_options(
            "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:C,CXX>>:/experimental\:external;/external\:W0;/external\:I >"
        )
        add_compile_options(
            "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-experimental\:external -external\:W0 -external\:I >"
        )
    endif()

    # On MSVC, we want different optimizations depending on the target
    # CMake does allow us to override CXX_FLAGS, so we reset them here and
    # restore them later, modified or not, in restore_cxx_flags()
    set(SIGHT_CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG_INIT} CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
endif()

# Color for ninja and Clang on Linux and OSX
if(CMAKE_GENERATOR STREQUAL "Ninja")
    add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<COMPILE_LANGUAGE:C,CXX>>:-fdiagnostics-color>")
    add_compile_options(
        "$<$<AND:$<CXX_COMPILER_ID:GNU,CLANG>,$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=-fdiagnostics-color>"
    )

    add_compile_options("$<$<AND:$<CXX_COMPILER_ID:Clang>,$<COMPILE_LANGUAGE:C,CXX>>:-fcolor-diagnostics>")
endif()

# CUDA specific options: Use the Sight customized CXX/C flags, not the CMAKE default
# NVCC flags:
# --expt-relaxed-constexpr: allow usage of __host__ constexpr functions device side.
set(CMAKE_CUDA_FLAGS "--expt-relaxed-constexpr -Xcompiler=\"${CMAKE_CXX_FLAGS}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_INIT "--expt-relaxed-constexpr -Xcompiler=\"${CMAKE_CXX_FLAGS_INIT}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_MINSIZEREL "-Xcompiler=\"${CMAKE_CXX_FLAGS_MINSIZEREL}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_MINSIZEREL_INIT "-Xcompiler=\"${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_RELWITHDEBINFO "-Xcompiler=\"${CMAKE_CXX_FLAGS_RELWITHDEBINFO}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_RELWITHDEBINFO_INIT "-Xcompiler=\"${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_RELEASE "-Xcompiler=\"${CMAKE_CXX_FLAGS_RELEASE}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_RELEASE_INIT "-Xcompiler=\"${CMAKE_CXX_FLAGS_RELEASE_INIT}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_DEBUG "-Xcompiler=\"${CMAKE_CXX_FLAGS_DEBUG}\"" CACHE STRING "" FORCE)
set(CMAKE_CUDA_FLAGS_DEBUG_INIT "-Xcompiler=\"${CMAKE_CXX_FLAGS_DEBUG_INIT}\"" CACHE STRING "" FORCE)

# Restore default compiler flags
function(restore_cxx_flags SIGHT_TARGET CXX_FLAGS OBJECT_LIBRARY)
    string(STRIP "${CXX_FLAGS}" SIGHT_CXX_FLAGS)
    set(SIGHT_CUDA_FLAGS "$<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=${SIGHT_CXX_FLAGS}>")

    string(REGEX REPLACE "[ ]+" ";" SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS})
    set(SIGHT_CXX_FLAGS "$<$<COMPILE_LANGUAGE:C,CXX>:${SIGHT_CXX_FLAGS}>")

    get_target_property(TARGET_TYPE ${SIGHT_TARGET} TYPE)

    # Skip libraries without code
    # FIXME: linker language is not know at this point
    if(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")

        target_compile_options(${SIGHT_TARGET} PRIVATE ${SIGHT_CXX_FLAGS})
        target_compile_options(${SIGHT_TARGET} PRIVATE ${SIGHT_CUDA_FLAGS})

        if(OBJECT_LIBRARY)
            target_compile_options(${SIGHT_TARGET}_obj PRIVATE ${SIGHT_CXX_FLAGS})
            target_compile_options(${SIGHT_TARGET}_obj PRIVATE ${SIGHT_CUDA_FLAGS})

            if(TARGET ${SIGHT_TARGET}_obj_PCH_OBJ)
                target_compile_options(${SIGHT_TARGET}_obj_PCH_OBJ PRIVATE ${SIGHT_CXX_FLAGS})
                target_compile_options(${SIGHT_TARGET}_obj_PCH_OBJ PRIVATE ${SIGHT_CUDA_FLAGS})
            endif()
        elseif(TARGET ${SIGHT_TARGET}_PCH_OBJ)
            target_compile_options(${SIGHT_TARGET}_PCH_OBJ PRIVATE ${SIGHT_CXX_FLAGS})
            target_compile_options(${SIGHT_TARGET}_PCH_OBJ PRIVATE ${SIGHT_CUDA_FLAGS})
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

    # Remove duplicated space
    string(REGEX REPLACE "[ ]+" " " SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS})
    string(STRIP "${SIGHT_CXX_FLAGS}" SIGHT_CXX_FLAGS)

    set(SIGHT_CXX_FLAGS "${SIGHT_CXX_FLAGS} /Ob1 /Ox /Oy-")

    restore_cxx_flags(${SIGHT_TARGET} ${SIGHT_CXX_FLAGS} ${OBJECT_LIBRARY})
endfunction()
