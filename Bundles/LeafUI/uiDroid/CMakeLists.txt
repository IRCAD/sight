fwLoadProperties()
fwUseForwardInclude(
        fwComEd 
        fwCore 
)

find_library(JNIHELPER NAMES JNIHelper)
find_library(JUIHELPER NAMES JUIHelper)
find_library(ANDROID_NATIVE_APP_GLUE_LIB android_native_app)
fwLink(
    ${ANDROID_NATIVE_APP_GLUE_LIB}
    log
    android
    ${JNIHELPER}
    ${JUIHELPER}
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_LIBRARIES}/src/com/sample  ${CMAKE_INSTALL_PREFIX}/apk/src/com/sample
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
)