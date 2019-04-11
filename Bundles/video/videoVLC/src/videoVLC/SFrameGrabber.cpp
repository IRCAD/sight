/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "videoVLC/SFrameGrabber.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <arPreferences/preferences.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/helper/Image.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Type.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

namespace videoVLC
{

//-----------------------------------------------------------------------------

#ifndef WIN32
void logCallback(void* data, int level, const libvlc_log_t* ctx, const char* fmt, va_list args)
{
    FwCoreNotUsedMacro(data);
    FwCoreNotUsedMacro(ctx);

    char* result;
    if (vasprintf(&result, fmt, args) < 0)
    {
        return;
    }

    std::string message(result);
    delete[] result;

    switch (level)
    {
        case LIBVLC_ERROR:
            SLM_ERROR("[libvlc]: "+message);
            break;
        case LIBVLC_WARNING:
            SLM_WARN("[libvlc]: "+message);
            break;
        case LIBVLC_NOTICE:
        case LIBVLC_DEBUG:
        default:
            SLM_DEBUG("[libvlc]: "+message);
            break;
    }
}
#endif

//-----------------------------------------------------------------------------

class VlcCallbackProxy
{
public:
    VlcCallbackProxy(const ::arData::FrameTL::sptr& timeline, libvlc_media_player_t* vlcMedia) :
        m_timeline(timeline),
        m_vlcMedia(vlcMedia)
    {
    }

    virtual ~VlcCallbackProxy()
    {
    }

    //------------------------------------------------------------------------------

    static unsigned videoFormat( void** opaque, char* chroma,
                                 unsigned int* width, unsigned int* height,
                                 unsigned int* pitches, unsigned int* lines )
    {
        const size_t numberOfComponents = 4;
        VlcCallbackProxy* callback      = reinterpret_cast<VlcCallbackProxy*>(*opaque);

        callback->m_frame.resize((*width) * (*height) * numberOfComponents);

        const char DEF_CHROMA[] = "RGBA";
        memcpy(chroma, DEF_CHROMA, sizeof( DEF_CHROMA ) - 1 );

        *pitches = *width * numberOfComponents;
        *lines   = *height;

        unsigned int num = 0;
        unsigned int px  = 0;
        unsigned int py  = 0;

        // Retrieves display resolution to initialize timeline
        libvlc_video_get_size(callback->m_vlcMedia, num, &px, &py);
        if(px == 0 || py == 0)
        {
            //TODO FIXME: with stream libvlc_video_get_size cannot retrieve video size
            callback->m_timeline->initPoolSize(*width, *height, ::fwTools::Type::s_UINT8, numberOfComponents);
        }
        else
        {
            callback->m_timeline->initPoolSize(px, py, ::fwTools::Type::s_UINT8, numberOfComponents);
        }

        return 1;
    }

    //------------------------------------------------------------------------------

    static void videoCleanup( void* opaque )
    {
        VlcCallbackProxy* callback = reinterpret_cast<VlcCallbackProxy*>(opaque);
        callback->m_frame.resize(0);
    }

    //------------------------------------------------------------------------------

    static void* videoLock( void* opaque, void** planes )
    {
        VlcCallbackProxy* callback = reinterpret_cast<VlcCallbackProxy*>(opaque);
        *planes = callback->m_frame.empty() ? 0 : &callback->m_frame[0];
        return 0;
    }

    //------------------------------------------------------------------------------

    static void videoUnlock( void* opaque, void* picture, void* const* planes )
    {
    }

    //------------------------------------------------------------------------------

    static void videoDisplay( void* opaque, void* picture )
    {
        VlcCallbackProxy* callback = reinterpret_cast<VlcCallbackProxy*>(opaque);

        // If video is paused do not push any frames.
        if(callback->m_isPaused)
        {
            return;
        }

        const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

        SPTR(::arData::FrameTL::BufferType) buffer = callback->m_timeline->createBuffer(timestamp);
        std::uint8_t* destBuffer = reinterpret_cast< std::uint8_t* >( buffer->addElement(0) );
        std::uint8_t* srcBuffer  = callback->m_frame.data();

        memcpy(destBuffer, srcBuffer, buffer->getSize());

        // push buffer and notify.
        callback->m_timeline->pushObject(buffer);

        ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = callback->m_timeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);
    }

    //------------------------------------------------------------------------------

    void setIsPause(bool status)
    {
        m_isPaused = status;
    }

protected:

    std::vector< uint8_t > m_frame;
    ::arData::FrameTL::sptr m_timeline;
    libvlc_media_player_t* m_vlcMedia;
    bool m_isPaused{false};
};

//-----------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() noexcept :
    m_cameraID(""),
    m_loopVideo(false),
    m_vlcInstance(nullptr),
    m_vlcPlayer(nullptr),
    m_vlcMedia(nullptr),
    m_vlcProxy(nullptr),
    m_networkCaching("500")
{

}

//-----------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() noexcept
{

}

//-----------------------------------------------------------------------------

void SFrameGrabber::starting()
{
    if(::boost::filesystem::exists(VLC_PLUGINS_DIR))
    {
        SFrameGrabber::exportVLCPluginsEnv(VLC_PLUGINS_DIR);
    }
    else
    {
        SLM_ASSERT("VLC plugins directory is not found.", ::boost::filesystem::exists("./vlc/plugins"));
#ifdef WIN32
        SFrameGrabber::exportVLCPluginsEnv(".\\vlc\\plugins");
#else
        SFrameGrabber::exportVLCPluginsEnv("./vlc/plugins");
#endif
    }

    const std::string networkCaching = "--network-caching=" + m_networkCaching;

// initialize libVLC
#if _DEBUG
    const char* const vlc_args[] = {
        "-I", "dummy",           // Don't use any interface
        "--ignore-config",       // Don't use VLC's config
        "--verbose=3",
        networkCaching.c_str()
    };
    m_vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
#else
    const char* const args[] = { networkCaching.c_str() };

    m_vlcInstance = libvlc_new(sizeof(args)/sizeof(*args), args);
#endif
    SLM_ASSERT("Could not init libVLC", m_vlcInstance);

#ifndef WIN32
    libvlc_log_set(m_vlcInstance, logCallback, this);
#endif
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopping()
{
    this->stopCamera();

    // Release libVLC instance on quit
    if (m_vlcInstance)
    {
        libvlc_release(m_vlcInstance);
        m_vlcInstance = nullptr;
    }
    delete m_vlcProxy;
}

//-----------------------------------------------------------------------------

void SFrameGrabber::configuring()
{
    const ConfigType config = this->getConfigTree();
    m_networkCaching = config.get<std::string>("networkCaching", "500");
}

//-----------------------------------------------------------------------------

void SFrameGrabber::updating()
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SFrameGrabber::startCamera()
{
    // Stop if something is playing
    if (m_vlcPlayer)
    {
        this->stopCamera();
    }

    ::arData::Camera::csptr camera = this->getCamera();

    switch(camera->getCameraSource())
    {
        case ::arData::Camera::FILE:
        {
            // Path of the video file stored in the camera description
            ::boost::filesystem::path videoPath(camera->getVideoFile());
            ::boost::filesystem::path videoDir(::arPreferences::getVideoDir());

            // For compatibility with old calibration with absolute path
            if (!videoPath.is_absolute())
            {
                videoPath = videoDir / videoPath;
            }

            // Create a new Media
            m_vlcMedia =
                libvlc_media_new_path(m_vlcInstance, videoPath.make_preferred().string().c_str());
            SLM_ASSERT("Cannot create VLC media for file '"+videoPath.string()+"'.", m_vlcMedia);

            // Create a new libvlc player
            m_vlcPlayer = libvlc_media_player_new_from_media(m_vlcMedia);
            auto vlcHandler = libvlc_media_player_event_manager(m_vlcPlayer);
            libvlc_event_attach( vlcHandler, libvlc_MediaPlayerTimeChanged, &SFrameGrabber::onEventCallback, this );
            libvlc_event_attach( vlcHandler, libvlc_MediaPlayerLengthChanged, &SFrameGrabber::onEventCallback, this );
            libvlc_event_attach( vlcHandler, libvlc_MediaPlayerEndReached, &SFrameGrabber::onEventCallback, this );

            libvlc_media_player_play(m_vlcPlayer);

            break;
        }
        case ::arData::Camera::STREAM:
        {
            // Path of the video file stored in the camera description
            ::boost::filesystem::path videoPath(camera->getStreamUrl());

            // Create a new Media
            m_vlcMedia = libvlc_media_new_location(m_vlcInstance, videoPath.string().c_str());
            SLM_ASSERT("Cannot create VLC media for file '"+videoPath.string()+"'.", m_vlcMedia);

            // Create a new libvlc player
            m_vlcPlayer = libvlc_media_player_new_from_media(m_vlcMedia);
            libvlc_media_player_play(m_vlcPlayer);
            break;
        }
        case ::arData::Camera::DEVICE:
        {
#ifdef WIN32
            const std::string mode = "dshow";
#elif defined(__APPLE__)
            const std::string mode = "qtcapture";
#else
            const std::string mode = "v4l2";
#endif
            // Create a new Media
            const std::string device_name_url = mode+"://"+camera->getCameraID();
            m_vlcMedia = libvlc_media_new_location(m_vlcInstance, device_name_url.c_str());
            SLM_ASSERT("Cannot create VLC media", m_vlcMedia);

            //libvlc_media_add_option(m_vlcMedia, mode + "://");

            std::string dshow_vdev = ":" + mode + "-vdev="+camera->getDescription();
            libvlc_media_add_option(m_vlcMedia, dshow_vdev.c_str());

            std::string dshow_fps = ":"+ mode +"-fps="+std::to_string(camera->getMaximumFrameRate());
            libvlc_media_add_option(m_vlcMedia, dshow_fps.c_str());

            if(camera->getWidth() > 0 && camera->getHeight() > 0)
            {
                std::string dshow_size = ":" + mode + "-size=" +
                                         std::to_string(camera->getWidth())+"x"+std::to_string(camera->getHeight());
                libvlc_media_add_option(m_vlcMedia, dshow_size.c_str());
            }
            // Create a new libvlc player
            m_vlcPlayer = libvlc_media_player_new_from_media(m_vlcMedia);
            libvlc_media_player_play(m_vlcPlayer);
            break;
        }
        case ::arData::Camera::UNKNOWN:
        {
            SLM_ERROR("No camera source defined.");
            break;
        }
    }

    if(m_vlcPlayer)
    {
        ::arData::FrameTL::sptr timeline = this->getInOut< ::arData::FrameTL >(s_FRAMETL_INOUT);
        m_vlcProxy                       = new VlcCallbackProxy(timeline, m_vlcPlayer);

        libvlc_video_set_callbacks(m_vlcPlayer,
                                   VlcCallbackProxy::videoLock,
                                   VlcCallbackProxy::videoUnlock,
                                   VlcCallbackProxy::videoDisplay,
                                   m_vlcProxy);
        libvlc_video_set_format_callbacks(m_vlcPlayer,
                                          VlcCallbackProxy::videoFormat,
                                          VlcCallbackProxy::videoCleanup );

        auto sig = this->signal< ::arServices::IGrabber::CameraStartedSignalType >(
            ::arServices::IGrabber::s_CAMERA_STARTED_SIG);
        sig->asyncEmit();

        m_vlcProxy->setIsPause(false);
        this->setStartState(true);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    if (m_vlcPlayer)
    {
        if (libvlc_media_player_is_playing(m_vlcPlayer))
        {
            // Pause
            libvlc_media_player_pause(m_vlcPlayer);
            m_vlcProxy->setIsPause(true);
        }
        else
        {
            // Play again
            libvlc_media_player_play(m_vlcPlayer);
            m_vlcProxy->setIsPause(false);
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopCamera()
{
    if(m_vlcPlayer)
    {
        // Re initialize slider position.
        // We deliberately send a negative value so the VideoQtEditor::SSlider can set the text to "--:--:--"
        std::int64_t position = -1;
        auto sigPosition      = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit( position );

        // Reset the slider duration to prevent crash.
        // We deliberately send a negative value so the VideoQtEditor::SSlider can set the text to "--:--:--"
        std::int64_t newDuration = -1;
        auto sigDuration         = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit( newDuration );

        libvlc_video_set_callbacks( m_vlcPlayer, 0, 0, 0, 0 );
        libvlc_video_set_format_callbacks( m_vlcPlayer, 0, 0 );

        // stop the media player
        libvlc_media_player_stop(m_vlcPlayer);

        libvlc_media_release(m_vlcMedia);

        // release the media player
        libvlc_media_player_release(m_vlcPlayer);

        delete m_vlcProxy;

        // Reset application values
        m_vlcPlayer = nullptr;
        m_vlcProxy  = nullptr;

        // Reset the timeline and send a black frame if we had a valid playback before
        ::arData::FrameTL::sptr timeline = this->getInOut< ::arData::FrameTL >(s_FRAMETL_INOUT);

        if(timeline->getWidth() * timeline->getHeight() * timeline->getNumberOfComponents() > 0)
        {
            const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec() + 1;
            SPTR(::arData::FrameTL::BufferType) buffer = timeline->createBuffer(timestamp);
            std::uint8_t* destBuffer = reinterpret_cast< std::uint8_t* >( buffer->addElement(0) );

            std::fill(destBuffer,
                      destBuffer + timeline->getWidth() * timeline->getHeight() * timeline->getNumberOfComponents(), 0);

            // push buffer and notify
            timeline->clearTimeline();
            auto sigTLCleared = timeline->signal< ::arData::FrameTL::ObjectClearedSignalType >(
                ::arData::FrameTL::s_CLEARED_SIG );
            sigTLCleared->asyncEmit();

            timeline->pushObject(buffer);
            auto sigTL = timeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
            sigTL->asyncEmit(timestamp);

            auto sig = this->signal< ::arServices::IGrabber::CameraStoppedSignalType >(
                ::arServices::IGrabber::s_CAMERA_STOPPED_SIG);
            sig->asyncEmit();

            this->setStartState(false);
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::selectCamera(const std::string& cameraID)
{
    m_cameraID = cameraID;
    this->startCamera();
}

//-----------------------------------------------------------------------------

CSPTR(::arData::Camera) SFrameGrabber::getCamera()
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >("camera");
    FW_RAISE_IF("Camera not found", !camera);
    return camera;
}

//-----------------------------------------------------------------------------

void SFrameGrabber::toggleLoopMode()
{
    m_loopVideo = !m_loopVideo;
}

//-----------------------------------------------------------------------------

void SFrameGrabber::setPosition(int64_t position)
{
    if( m_vlcPlayer )
    {
        libvlc_media_player_set_time(m_vlcPlayer, position);
    }
}
//----------------------------------------------------------------------------

void SFrameGrabber::onEventCallback(const struct libvlc_event_t* event, void* userData)
{
    SFrameGrabber* sFrameGrabber = static_cast< videoVLC::SFrameGrabber*>( userData );
    SLM_ASSERT("SFrameGrabber is null. ", sFrameGrabber);
    SLM_ASSERT("Vlc event is null. ", event);

    switch( event->type )
    {
        case libvlc_MediaPlayerTimeChanged:
        {
            std::int64_t position = event->u.media_player_time_changed.new_time;
            auto sig              = sFrameGrabber->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
            sig->asyncEmit( position );
            break;
        }
        case libvlc_MediaPlayerLengthChanged:
        {
            std::int64_t newDuration = event->u.media_player_length_changed.new_length;
            auto sig                 = sFrameGrabber->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
            sig->asyncEmit( newDuration );
            break;
        }
        case libvlc_MediaPlayerEndReached:
        {
            if (sFrameGrabber->m_loopVideo)
            {
                sFrameGrabber->slot(s_START_CAMERA_SLOT)->asyncRun();
            }
            break;
        }
        default:
            break;
    }
}

//------------------------------------------------------------------------------

void SFrameGrabber::exportVLCPluginsEnv(const std::string& path)
{
#ifdef WIN32
    {
        typedef int (_cdecl* PUTENVPROC)(const char*);

        HMODULE hmodule = GetModuleHandle("msvcrt");
        SLM_ASSERT("GetModuleHandle msvcrt failed", hmodule);

        PUTENVPROC putenvFunc = (PUTENVPROC) GetProcAddress(hmodule, "_putenv");
        SLM_ASSERT("GetProcAddress _putenv failed", hmodule);

        const std::string env = "VLC_PLUGIN_PATH=" + path;
        putenvFunc(env.c_str());
    }
#else
    setenv("VLC_PLUGIN_PATH", path.c_str(), 1);
#endif
}

//----------------------------------------------------------------------------
} // namespace videoVLC
