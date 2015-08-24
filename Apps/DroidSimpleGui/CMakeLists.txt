fwLoadProperties()

if(DroidSimpleGui_INSTALL AND ANDROID)
    include("${CMAKE_SOURCE_DIR}/CMake/android/droid_setup.cmake" REQUIRED)

    set(CMAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/CMake/android)
    set(APK_BUILD_DIR ${CMAKE_BINARY_DIR}/apk)

    configure_file("${CMAKE_DIRECTORY}/droid_builder.cmake.in" "${CMAKE_BINARY_DIR}/CMakeFiles/droid_builder.cmake" @ONLY)
    install(SCRIPT "${CMAKE_BINARY_DIR}/CMakeFiles/droid_builder.cmake")

endif()
