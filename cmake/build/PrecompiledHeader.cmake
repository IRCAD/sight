# Function for setting up precompiled headers. Usage:
#
#   add_library/executable(target
#       pchheader.c pchheader.cpp pchheader.h)
#
#   add_precompiled_header(target pchheader.h
#       [FORCEINCLUDE]
#       [SOURCE_C pchheader.c]
#       [SOURCE_CXX pchheader.cpp])
#
# Options:
#
#   FORCEINCLUDE: Add compiler flags to automatically include the
#   pchheader.h from every source file. Works with both GCC and
#   MSVC. This is recommended.
#
#   SOURCE_C/CXX: Specifies the .c/.cpp source file that includes
#   pchheader.h for generating the pre-compiled header
#   output. Defaults to pchheader.c. Only required for MSVC.
#
# Caveats:
#
#   * Its not currently possible to use the same precompiled-header in
#     more than a single target in the same directory (No way to set
#     the source file properties differently for each target).
#
#   * MSVC: A source file with the same name as the header must exist
#     and be included in the target (E.g. header.cpp). Name of file
#     can be changed using the SOURCE_CXX/SOURCE_C options.
#
# License:
#
# Copyright (C) 2009-2013 Lars Christensen <larsch@belunktum.dk>
# Copyright (C) 2016-2018 IRCAD France
# Copyright (C) 2016-2018 IHU Strasbourg
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the 'Software') deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

include(CMakeParseArguments)

macro(combine_arguments _variable)
  set(_result "")
  foreach(_element ${${_variable}})
    set(_result "${_result} \"${_element}\"")
  endforeach()
  string(STRIP "${_result}" _result)
  set(${_variable} "${_result}")
endmacro()

# Hook used to add a dependency on the automoc obj file for each pch target
function(pch_msvc_hook variable access value current_list_file stack)

    if(NOT CMAKE_PCH_COMPILER_TARGETS)
        return()
    endif()
    list(LENGTH CMAKE_PCH_COMPILER_TARGETS length)
    foreach(index RANGE -${length} -1)
        list(GET CMAKE_PCH_COMPILER_TARGETS ${index} target)
        list(GET CMAKE_PCH_COMPILER_TARGET_FLAGS ${index} flags)

        # Find OBJ pch target dependencies
        target_include_directories(${target}_PCH_OBJ SYSTEM PRIVATE "$<TARGET_PROPERTY:${target},INTERFACE_INCLUDE_DIRECTORIES>")

        # 2. Add the same compile definitions
        get_target_property(def ${target} COMPILE_DEFINITIONS)
        list(APPEND defines ${def})

        # 3. Add the same define symbols
        get_target_property(def ${target} DEFINE_SYMBOL)
        list(APPEND defines ${def})

        # 4. Remove symbols that would not defined by other targets using this pch
        foreach(def ${defines})
            string(REGEX REPLACE ".*_VER=\"[0-9].[0-9]\"" "" def1 ${def})
            if(def1)
                string(REGEX REPLACE ".*_EXPORTS" "" def2 ${def1})
                if(def2)
                    target_compile_definitions(${target}_PCH_OBJ PRIVATE ${def2})
                endif()
            endif()
        endforeach()
    endforeach()

endfunction()

# Only used on Linux
function(export_all_flags _filename)
    set(_compile_definitions "$<TARGET_PROPERTY:${_target},COMPILE_DEFINITIONS>")
    set(_include_directories "$<TARGET_PROPERTY:${_target},INCLUDE_DIRECTORIES>")
    set(_compile_flags "$<TARGET_PROPERTY:${_target},COMPILE_FLAGS>")
    set(_compile_options "$<TARGET_PROPERTY:${_target},COMPILE_OPTIONS>")
    set(_define_symbol "$<TARGET_PROPERTY:${_target},DEFINE_SYMBOL>")
    set(_include_directories "$<$<BOOL:${_include_directories}>:-isystem$<JOIN:${_include_directories},\n-isystem>\n>")
    set(_compile_definitions "$<$<BOOL:${_compile_definitions}>:-D$<JOIN:${_compile_definitions},\n-D>\n>")
    set(_compile_flags "$<$<BOOL:${_compile_flags}>:$<JOIN:${_compile_flags},\n>\n>")
    set(_compile_options "$<$<BOOL:${_compile_options}>:$<JOIN:${_compile_options},\n>\n>")
    set(_define_symbol "$<$<BOOL:${_define_symbol}>:-D$<JOIN:${_define_symbol},\n-D>\n>")
    file(GENERATE OUTPUT "${_filename}" CONTENT "${_compile_definitions}${_include_directories}${_compile_flags}${_compile_options}${_define_symbol}\n")
endfunction()

# Only used on Linux
function(assign_precompiled_header _target _pch _pch_header)

    # Iterate over all source files and request pch usage
    get_property(_sources TARGET ${_target} PROPERTY SOURCES)
    foreach(_source ${_sources})
        set(_pch_compile_flags "")

        if(_source MATCHES \\.\(cc|cxx|cpp\)$)
            get_source_file_property(_pch_compile_flags "${_source}" COMPILE_FLAGS)
            if(NOT _pch_compile_flags)
                set(_pch_compile_flags)
            endif()

            set(_pch_compile_flags "${_pch_compile_flags} -Winvalid-pch")

            if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
                set(_pch_compile_flags "${_pch_compile_flags} -include-pch \"${_pch}\"")
            else()
                set(_pch_compile_flags "-include \"${_pch_header}\" ${_pch_compile_flags}")
            endif()

            get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
            if(NOT _object_depends)
                set(_object_depends)
            endif()
            list(APPEND _object_depends "${_pch}")

            set_source_files_properties(${_source} PROPERTIES
                COMPILE_FLAGS "${_pch_compile_flags}"
                OBJECT_DEPENDS "${_object_depends}")
        endif()
    endforeach()

endfunction()

macro(add_precompiled_header_cpp _target)
    # Add an "object" library to compile the pch
    # That allows to share the pch, targets using this pch can then link with the pch.obj thanks to this fake library
    # This also help us to remove some unwanted compile definitions (see pch_msvc_hook function)
    add_library(${_target}_PCH_OBJ OBJECT "${FWCMAKE_RESOURCE_PATH}/build/pch.cpp")
    set_target_properties(${_target}_PCH_OBJ PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY
                          "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_target}.dir/")

endmacro()

function(add_precompiled_header _target _input)
  cmake_parse_arguments(_PCH "FORCEINCLUDE" "SOURCE_CXX:SOURCE_C" "" ${ARGN})

  if(MSVC)
    get_filename_component(_input_we ${_input} NAME_WE)
    get_filename_component(_input_pch ${_input} NAME)

    set(_PCH_SOURCE_CXX "${FWCMAKE_RESOURCE_PATH}/build/pch.cpp")

    set(_cxx_path "${CMAKE_CURRENT_BINARY_DIR}")
    set(_pch_cxx_header "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp")
    set(_pch_cxx_pch "${_cxx_path}/${_input_we}.pch")
    set(_pch_cxx_pdb "${_cxx_path}/${_input_we}.pdb")

    set_target_properties(${_target} PROPERTIES PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    # Iterate over all source files and request pch usage
    get_target_property(sources ${_target} SOURCES)
    foreach(_source ${sources})
        set(_pch_compile_flags "")
        if(_source MATCHES \\.\(cc|cxx|cpp\)$)

            set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" /Yu${_input}")
            set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_pch_cxx_pch}")
            # Force the include of the PCH on every source file
            set(_pch_compile_flags "${_pch_compile_flags} /FI${_input}")

            get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
            if(NOT _object_depends)
                set(_object_depends)
            endif()

            list(APPEND _object_depends "${_pch_cxx_header}")

            set_source_files_properties(${_source} PROPERTIES
                COMPILE_FLAGS "${_pch_compile_flags}"
                OBJECT_DEPENDS "${_object_depends}")
        endif()
    endforeach()

    # Iterate over the pch source file and compile the pch
    get_target_property(sources ${_target}_PCH_OBJ SOURCES)
    foreach(_source ${sources})
        set(_pch_compile_flags "")
        if(_source STREQUAL "${_PCH_SOURCE_CXX}")
            set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" /Yc${_input_pch} \
                                    \"/I${CMAKE_CURRENT_SOURCE_DIR}\"")
            set(_pch_source_cxx_found TRUE)
            set_source_files_properties("${_source}" PROPERTIES OBJECT_OUTPUTS "${_pch_cxx_pch}")

            get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
            if(NOT _object_depends)
                set(_object_depends)
            endif()

            list(APPEND _object_depends "${_pch_cxx_header}")

            set_source_files_properties(${_source} PROPERTIES
                COMPILE_FLAGS "${_pch_compile_flags}"
                OBJECT_DEPENDS "${_object_depends}")
        endif()
    endforeach()

    # Add this target to process in pch_msvc_hook
    # Indeed at this point all compile definitions and include directories may be not set yet
    list(APPEND CMAKE_PCH_COMPILER_TARGETS ${_target})
    set(CMAKE_PCH_COMPILER_TARGETS "${CMAKE_PCH_COMPILER_TARGETS}" PARENT_SCOPE)

    if(NOT _pch_source_cxx_found)
      message(FATAL_ERROR "A source file ${_PCH_SOURCE_CXX} for ${_input} is required for MSVC builds. Can be set with the SOURCE_CXX option.")
    endif()
  endif(MSVC)

  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    get_filename_component(_name ${_input} NAME)
    set(_pch_header "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    set(_pch_binary_dir "${CMAKE_CURRENT_BINARY_DIR}")
    set(_pchfile "${_pch_binary_dir}/include/${_target}/${_input}")

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(_outdir "${_pchfile}.pch")
    else()
        set(_outdir "${_pchfile}.gch")

        target_include_directories(${_target} PRIVATE ${_pch_binary_dir}/include/${_target})
    endif()
    set(_output_cxx "${_outdir}")

    # we use a response file to pass most compiler flags
    set(_pch_response_file "compile_flags.rsp")
    set(_pch_flags_file "${_pch_binary_dir}/${_pch_response_file}")
    export_all_flags("${_pch_flags_file}.in")
    set(_compiler_FLAGS "@${_pch_flags_file}")

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CXXFLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CXXFLAGS " ${CMAKE_CXX_FLAGS_DEBUG}")
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        set(CXXFLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CXXFLAGS " ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
        set(CXXFLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CXXFLAGS " ${CMAKE_CXX_FLAGS_MINSIZEREL}")
    else()
        set(CXXFLAGS "${CMAKE_CXX_FLAGS}")
        string(APPEND CXXFLAGS " ${CMAKE_CXX_FLAGS_RELEASE}")
    endif()
    separate_arguments(CXXFLAGS)

    # hopelessly these guys don't manage to get passed by the global CMake switch, add them manually
    list(APPEND CXXFLAGS "-std=gnu++17" "-fPIC")

    # Hacky custom command to remove the custom defines that would prevent from sharing the pch
    # and they should be useless anyway
    # Also, we strip "/usr/include" as CMake does for regular C++ files (otherwise we may hide our bin pkgs headers)
    add_custom_command(
      OUTPUT "${_pch_flags_file}"
      COMMAND sed 's/"/\\\\"/g' ${_pch_response_file}.in | sed '/_VER=\\\|_EXPORTS/d' | sed '/-isystem\\/usr\\/include$$/d' > ${_pch_flags_file}
      DEPENDS "${_pch_flags_file}.in"
      COMMENT "Fixing ${_pch_flags_file}")

    if(${CMAKE_GENERATOR} STREQUAL "Ninja")
      # Ninja generator does not support IMPLICIT_DEPENDS, thus we have to use an another way to get correct
      # dependencies so that the pch is recompiled when one of its include file changes.
      # We generate the dependencies file manually with the compiler
      file(RELATIVE_PATH relative_output_cxx "${CMAKE_BINARY_DIR}" "${_output_cxx}")

      add_custom_command(
        OUTPUT "${_pch_binary_dir}/pch.d"
        COMMAND "${CMAKE_CXX_COMPILER}" ${_compiler_FLAGS} ${CXXFLAGS} -M -MF "${_pch_binary_dir}/pch.d.in" "${_pch_header}"
        COMMAND sed 's|^pch\.o|${relative_output_cxx}|' "${_pch_binary_dir}/pch.d.in" > "${_pch_binary_dir}/pch.d"
        DEPENDS "${_pch_header}" "${_pch_flags_file}"
        COMMENT "Generating pch deps file for ${_target} (PCH)")
      add_custom_command(
        OUTPUT "${_output_cxx}"
        COMMAND "${CMAKE_CXX_COMPILER}" ${_compiler_FLAGS} -x c++-header -o "${_output_cxx}" "${_pch_header}" ${CXXFLAGS}
        DEPENDS "${_pch_header}" "${_pch_flags_file}" "${_pch_binary_dir}/pch.d"
        DEPFILE "${_pch_binary_dir}/pch.d"
        COMMENT "Precompiling ${_name} for ${_target} (PCH)")
    else()
      add_custom_command(
        OUTPUT "${_output_cxx}"
        COMMAND mkdir --parents "${_pch_binary_dir}/include/${_target}" && "${CMAKE_CXX_COMPILER}" ${_compiler_FLAGS} -x c++-header  -o "${_output_cxx}" "${_pch_header}" ${CXXFLAGS}
        DEPENDS "${_pch_header}" "${_pch_flags_file}"
        IMPLICIT_DEPENDS CXX "${_pch_header}"
        COMMENT "Precompiling ${_name} for ${_target} (PCH)")
    endif()

    assign_precompiled_header(${_target} ${_output_cxx} pch.hpp)
  endif()
endfunction()

function(use_precompiled_header _target _input)

    get_target_property(_pch_binary_dir ${_input} BINARY_DIR)

    if(MSVC)
        get_target_property(INPUT_PROJECT_DIR ${_input} PROJECT_DIR)
        target_include_directories(${_target} PRIVATE "${INPUT_PROJECT_DIR}" )

        set(_pch_header "${INPUT_PROJECT_DIR}/pch.hpp")
        set(_cxx_path "${_pch_binary_dir}")
        set(_pch_cxx_pch "${_cxx_path}/pch.pch")

        # Iterate over all source files and request pch usage
        get_target_property(sources ${_target} SOURCES)
        foreach(_source ${sources})
            set(_pch_compile_flags "")
            if(_source MATCHES \\.\(cc|cxx|cpp\)$)

                set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" /Yu${_pch_header}")
                set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_pch_cxx_pch}")
                # Force the include of the PCH on every source file
                set(_pch_compile_flags "${_pch_compile_flags} /FI${_pch_header}")

                get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
                if(NOT _object_depends)
                    set(_object_depends)
                endif()

                list(APPEND _object_depends "${_pch_header}")

                set_source_files_properties(${_source}  PROPERTIES
                                                        COMPILE_FLAGS "${_pch_compile_flags}"
                                                        OBJECT_DEPENDS "${_object_depends}")
            endif()
        endforeach()
    endif(MSVC)

    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(_pch_header "pch.hpp")
        set(_pchfile "${_pch_binary_dir}/include/${_input}/pch.hpp")

        if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            set(_output_cxx "${_pchfile}.pch")
        else()
            set(_output_cxx "${_pchfile}.gch")

            # Add the location of the pch as an include directory
            target_include_directories(${_target} PRIVATE ${_pch_binary_dir}/include/${_input} )
        endif()

        assign_precompiled_header(${_target} ${_output_cxx} ${_pch_header})

        # Unix Makefiles (default) generator does not manage to deduce dependencies properly
        # So we have to enforce it
        if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
            add_dependencies(${_target} ${_input})
        endif()

    endif()
endfunction()
