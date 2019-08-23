fwLoadProperties()

find_path(VLC_INCLUDE_DIR vlc/vlc.h PATHS
    "${CONAN_VLC_ROOT}/include"
    "$ENV{ProgramW6432}/VideoLAN/VLC/sdk/include"
    /usr/local/include
    /usr/include    
)

find_library(VLC_LIBRARY NAMES libvlc vlc PATHS
    "${CONAN_VLC_ROOT}/lib"
    "$ENV{ProgramW6432}/VideoLAN/VLC/sdk/lib"
    /usr/local/lib
    /usr/lib
    /usr/lib/x86_64-linux-gnu
)

# Find the vlc plugin directory, we check two different names :
#   - on windows, it contains directories (like video_output)
#   - on macOS, it contains only libraries (like libzip_plugin.dylib)
find_path(VLC_PLUGINS_DIR NAMES plugins PATHS
    "${CONAN_VLC_ROOT}"
    "${CONAN_VLC_ROOT}/lib/vlc"
    "$ENV{ProgramW6432}/VideoLAN/VLC"
    /usr/local/lib/vlc
    /usr/lib/vlc
    /usr/lib/x86_64-linux-gnu/vlc
)

if(VLC_PLUGINS_DIR)
    set(VLC_PLUGINS_DIR "${VLC_PLUGINS_DIR}/plugins")
    install(DIRECTORY ${VLC_PLUGINS_DIR} DESTINATION "vlc")
    add_definitions("-DVLC_PLUGINS_DIR=\"${VLC_PLUGINS_DIR}\"")
endif()

fwLink( ${VLC_LIBRARY} )
fwInclude( ${VLC_INCLUDE_DIR} )
