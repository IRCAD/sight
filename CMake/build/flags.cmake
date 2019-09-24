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

# We try to load our defaults from a conan python package because we share some settings with conan
find_package(PythonInterp 3 QUIET)

if(USE_SYSTEM_LIB OR NOT PYTHONINTERP_FOUND)
    # No conan or python, means we must use hard coded default
    # We simply use CMAKE one plus some very basic stuff
    add_compile_options(
        "$<$CXX_COMPILER_ID:Clang,AppleClang,GNU>:-march=sandybridge;-mtune=generic;-mfpmath=sse>"
        "$<$<CXX_COMPILER_ID:MSVC>:/favor:blend;/fp:precise;/Qfast_transcendentals;/arch:AVX;/MP;/bigobj>"
    )

    add_definitions(
        "$<$<CXX_COMPILER_ID:AppleClang>>:-DGL_SILENCE_DEPRECATION>"
        "$<$<CXX_COMPILER_ID:MSVC>:/D_ENABLE_EXTENDED_ALIGNED_STORAGE>"
    )
elseif(PYTHONINTERP_FOUND)
    # Get our default from common.py
    # Set PYTHONPATH environment to be able to import common
    set(OLD_PYTHONPATH $ENV{PYTHONPATH})
    set(ENV{PYTHONPATH} ${CONAN_COMMON_ROOT})

    # Common flags
    # Execute common.get_cxx_flags() and store it to ${SIGHT_CXX_FLAGS}
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -u -c "import common; print(\"{0}\".format(common.get_cxx_flags()), sep='', end='', flush=True)"
        OUTPUT_VARIABLE SIGHT_CXX_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Add common flags
    separate_arguments(SIGHT_CXX_FLAGS)

    # Release flags
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Execute common.get_cxx_flags_release() and store it to ${SIGHT_CXX_FLAGS_RELEASE}
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -u -c "import common; print(\"{0}\".format(common.get_release_cxx_flags()), sep='', end='', flush=True)"
            OUTPUT_VARIABLE SIGHT_CXX_FLAGS_RELEASE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Add Release flags
        separate_arguments(SIGHT_CXX_FLAGS_RELEASE)
        list(APPEND SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS_RELEASE})
        unset(SIGHT_CXX_FLAGS_RELEASE)

    # Debug flags
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        option(THOROUGH_DEBUG "Use slow but thorough debuging" OFF)

        if(THOROUGH_DEBUG)
            set(PYTHON_DEBUG_COMMAND "import common; print(\"{0}\".format(common.get_thorough_debug_cxx_flags()), sep='', end='', flush=True)")
        else()
            set(PYTHON_DEBUG_COMMAND "import common; print(\"{0}\".format(common.get_debug_cxx_flags()), sep='', end='', flush=True)")
        endif()

        # Execute common.get_cxx_flags_debug() and store it to ${SIGHT_CXX_FLAGS_DEBUG}
            execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -u -c "${PYTHON_DEBUG_COMMAND}"
            OUTPUT_VARIABLE SIGHT_CXX_FLAGS_DEBUG
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        unset(PYTHON_DEBUG_COMMAND)

        # Add Debug flags
        separate_arguments(SIGHT_CXX_FLAGS_DEBUG)
        list(APPEND SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS_DEBUG})
        unset(SIGHT_CXX_FLAGS_DEBUG)

    # RelWithDebInfo flags
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        # Execute common.get_cxx_flags_relwithdebinfo() and store it to ${SIGHT_CXX_FLAGS_RELWITHDEBINFO}
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -u -c "import common; print(\"{0}\".format(common.get_relwithdebinfo_cxx_flags()), sep='', end='', flush=True)"
            OUTPUT_VARIABLE SIGHT_CXX_FLAGS_RELWITHDEBINFO
        )

        # Add RelWithDebInfo flags
        separate_arguments(SIGHT_CXX_FLAGS_RELWITHDEBINFO)
        list(APPEND SIGHT_CXX_FLAGS ${SIGHT_CXX_FLAGS_RELWITHDEBINFO})
        unset(SIGHT_CXX_FLAGS_RELWITHDEBINFO)
    endif()

    # Restore old PYTHONPATH environment
    set(ENV{PYTHONPATH} ${OLD_PYTHONPATH})
    unset(OLD_PYTHONPATH)

    # Filter incompatible flags on MSVC to avoid errors and command-line warning D9025
    if(MSVC)
        if(SIGHT_CXX_FLAGS MATCHES "/O[x12]")
            # /RTC[1csu] is incompatible with optimization
            replace_flags("/RTC[1csu]+" "")

            # Avoid errors and command-line Warning D9025
            replace_flags("/O[x12d]" "")
        endif()

        if(SIGHT_CXX_FLAGS MATCHES "/Ob[012]")
            # Avoid errors and command-line Warning D9025
            replace_flags("/Ob[012]" "")
        endif()
    endif()

    # Finally, add the flags
    add_compile_options(${SIGHT_CXX_FLAGS})
    unset(SIGHT_CXX_FLAGS)
endif()

# C/C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++ visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# On macOS fwComTest fails without -fvisibility-ms-compat and -fvisibility=hidden
add_compile_options(
    "$<$<CXX_COMPILER_ID:AppleClang>:-fvisibility-ms-compat>"
)

# Warning level
add_compile_options(
    "$<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wall;-Wextra;-Wconversion;-Wno-unused-parameter;-Wno-ignored-qualifiers>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
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

    if(ENABLE_PCH)
        # Store debug information in the .obj file instead of a PDB.
        replace_flags("/Z[iI]" "/Z7")
    endif()

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
        "$<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>"
        "$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>"
    )
endif()