fwLoadProperties()

find_path(VLC_INCLUDE_DIR vlc/vlc.h PATHS
    /usr/include
    /usr/local/include
    "${EXTERNAL_LIBRARIES}/include"
    "$ENV{ProgramW6432}/VideoLAN/VLC/sdk/include"
)

find_library(VLC_LIBRARY NAMES libvlc vlc PATHS
    /usr/lib
    /usr/local/lib
    "${EXTERNAL_LIBRARIES}/lib/vlc"
    "$ENV{ProgramW6432}/VideoLAN/VLC/sdk/lib"
)

# Find the vlc plugin directory, we check two different names :
#   - on windows, it contains directories (like video_output)
#   - on macOS, it contains only libraries (like libzip_plugin.dylib)
find_path(VLC_PLUGINS_DIR NAMES video_output libzip_plugin.dylib PATHS
    /usr/lib/vlc/plugins
    /usr/local/lib/vlc/plugins
    "${EXTERNAL_LIBRARIES}/vlc/plugins"
    "$ENV{ProgramW6432}/VideoLAN/VLC/plugins"
)

if(VLC_PLUGINS_DIR)
    install(DIRECTORY ${VLC_PLUGINS_DIR} DESTINATION "vlc")
    add_custom_command(TARGET ${FWPROJECT_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${VLC_PLUGINS_DIR} ${CMAKE_BINARY_DIR}/vlc/plugins)
endif()

fwLink( ${VLC_LIBRARY} )
fwInclude( ${VLC_INCLUDE_DIR} )
