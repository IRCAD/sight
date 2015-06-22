fwLoadProperties()
fwUseForwardInclude(
        fwComEd 
        fwCore 
)

find_library(JNIHELPER NAMES JNIHelper.so PATH_SUFFIXES ${CMAKE_INSTALL_PREFIX}/lib)
find_library(JUIHELPER NAMES JUIHelper.so PATH_SUFFIXES ${CMAKE_INSTALL_PREFIX}/lib)
find_library(ANDROID_NATIVE_APP_GLUE_LIB android_native_app)
fwLink(
    ${ANDROID_NATIVE_APP_GLUE_LIB}
    log
    android
    /Users/fchamp/Dev/CMake/v0.10/Deps-droid/Install_debug/lib/libJNIHelper.so
    /Users/fchamp/Dev/CMake/v0.10/Deps-droid/Install_debug/lib/libJUIHelper.so
)
