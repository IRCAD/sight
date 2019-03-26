/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "videoVLC/SFrameStreamer.hpp"

#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_unique.hpp>

namespace videoVLC
{

struct MemoryVideoData
{
    const unsigned char* m_videoBuffer;   //  pointer to video in memory
    size_t m_readingPos;
    size_t m_bytes; // Size of the whole frame, each frame should have the same size
};

/* LibVlc callback oepn method
 * Parameters :
 *   - opaque : pointer to the user set data. This is specified when calling libvlc_media_new_callbacks, in our case
 *       this is a MemoryVideoData pointer
 *   - data   : pointer to fill the opaque passed to media_read_cb with, in theory we could restrain to pass a simple
 *       buffer here
 *   - sizep  : size of the buffer to read.
 * Returns 0 if everything went well
 */
int media_open_cb(void* opaque, void** datap, uint64_t* sizep)
{
    //cast opaque to our video data struct
    MemoryVideoData* mVid = static_cast<MemoryVideoData*> (opaque);

    *sizep = static_cast<uint64_t>(mVid->m_bytes);    //set stream length
    *datap = mVid;

    return 0;
}

#ifdef _WIN32
typedef std::int64_t vlcSizeType;
#else
typedef long vlcSizeType;
#endif

/* LibVlc callback read method
 * Parameters :
 *   - opaque : pointer to the user set data. Specified in media_open_cb with datap.
 *   - buf    : pointer to fill, this might be a full or partial frame depending on len.
 *   - len    : required size of bug
 * Returns the number of bytes to read.
 */
vlcSizeType media_read_cb(void* opaque, unsigned char* buf, size_t len)
{
    MemoryVideoData* mVid = static_cast<MemoryVideoData*>(opaque);

    // Determine if there are enough bytes left in the current frame, if so, then the requested size 'len' is set.
    size_t copyLen = std::min(mVid->m_bytes - mVid->m_readingPos, len);

    // If ever we have reached the end of the frame, we simply start from the top again.
    if(copyLen == 0)
    {
        // We assume we have enough bytes to read.
        copyLen = len;
        // And we restart from the beginning.
        mVid->m_readingPos = 0;
    }

    // Continue where we left off.
    const unsigned char* start = mVid->m_videoBuffer + mVid->m_readingPos;

    // Convert from RGB to BGR, somwhere in libvlc a switch is done, difficult to tell where.
    for(size_t i = 0; i < copyLen; i += 4)
    {
        buf[i]   = start[i+2];
        buf[i+1] = start[i+1];
        buf[i+2] = start[i];
        buf[i+3] = start[i+3];
    }

    mVid->m_readingPos += copyLen;

    return static_cast<vlcSizeType>(copyLen);
}

/* LibVlc callback seek method, this is not yet handled on stream
 * Parameters :
 *   - opaque : pointer to the user set data. Specified in media_open_cb with datap.
 *   - offset : portion to move forward or backwards.
 * Returns 0 if everything went well.
 */
int media_seek_cb(void*, uint64_t)
{
    OSLM_INFO("seek callback was used but it's not implemented");

    return 0;
}

/* LibVlc callback stop method, nothing was allocated before, nothing to do.
 */
void media_close_cb(void*)
{
}

// Object keys
static const ::fwServices::IService::KeyType s_FRAMETL = "frameTL";

//  Public slot
const ::fwCom::Slots::SlotKeyType SFrameStreamer::s_UPDATE_FRAME_SLOT = "updateFrame";

// return the preference value if found, otherwise, cast the string into value as it is
std::string getPreferenceKey(const std::string& key)
{
    std::string keyResult(key);
    const size_t first = key.find('%');
    const size_t last  = key.rfind('%');
    if (first == 0 && last == key.size() - 1)
    {
        keyResult = ::fwPreferences::getPreference( key.substr(1, key.size() - 2) );
    }
    return keyResult;
}

//------------------------------------------------------------------------------

fwServicesRegisterMacro(::fwServices::IOperator, ::videoVLC::SFrameStreamer);

//------------------------------------------------------------------------------

SFrameStreamer::SFrameStreamer() noexcept
{
    newSlot(s_UPDATE_FRAME_SLOT, &SFrameStreamer::updateFrame, this);
}

//------------------------------------------------------------------------------

SFrameStreamer::~SFrameStreamer() noexcept
{
}

//------------------------------------------------------------------------------

void SFrameStreamer::starting()
{
}

//------------------------------------------------------------------------------

void SFrameStreamer::stopping()
{
    if(m_mediaPlayer)
    {
        // Stop playing
        libvlc_media_player_stop(m_mediaPlayer);
        // Free the media_player
        libvlc_media_player_release(m_mediaPlayer);
    }
}

//------------------------------------------------------------------------------

void SFrameStreamer::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_portCfg    = config.get("port", "9090");
    m_outAddrCfg = config.get("outAddr", "224.0.1.200");
}

//------------------------------------------------------------------------------

void SFrameStreamer::updating()
{
}

//-----------------------------------------------------------------------------

void SFrameStreamer::updateFrame(::fwCore::HiResClock::HiResClockType timestamp )
{
    // Get our current frame buffer.
    ::arData::FrameTL::csptr frameTL            = this->getInput< ::arData::FrameTL >(s_FRAMETL);
    CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

    const std::uint8_t* frame = &buffer->getElement(0);

    // Initialize the vlc instance and media while setting all streaming parameters (url, ip..) and encoder options.
    if(!m_isInitialized)
    {
        const size_t imageSize = frameTL->getWidth() * frameTL->getHeight() * frameTL->getNumberOfComponents();

        // Memory - vlc callback transition object.

        m_imemData = ::boost::make_unique<MemoryVideoData>();

        m_imemData->m_videoBuffer = frame;
        m_imemData->m_bytes       = imageSize;
        m_imemData->m_readingPos  = 0;

        // add Verbose option to instance set it to 0 for no verbose up to 4.
        std::string verboseOption = std::string("--verbose=")+ std::to_string(0);

        std::vector<const char*> options;
        options.push_back(verboseOption.c_str());

        // create instance with the given options.
        libvlc_instance_t* vlc = libvlc_new(int(options.size()), options.data());

        // Pass to VLC the different callback options, along with the our own memory structure.
        libvlc_media_t* media = libvlc_media_new_callbacks(vlc,
                                                           media_open_cb,
                                                           media_read_cb,
                                                           media_seek_cb,
                                                           media_close_cb,
                                                           m_imemData.get()
                                                           );

        // The following string was extracted from the VCL application. "sout" means we want a stream.
        std::string transcodingOptions(":sout=#transcode{");

        // Passing to VLC all of the needed for encoding (or transcoding rather, from raw -> h264 in our case).
        transcodingOptions += "vcodec=h264," // Codec of the video tracks of the input stream to be used.
                              "venc=x264," //This allows to set the encoder to use to encode the videos stream.
                              "vb=0," // Set the bitrate for the output stream. Note : '0' seems to tell vlc to find the
                                      // corresponding bitrate
                              "acodec=none" // No audio codec.
                              "}";

        const std::string port    = ::videoVLC::getPreferenceKey(m_portCfg);
        const std::string outAddr = ::videoVLC::getPreferenceKey(m_outAddrCfg);

        // Set streaming port and IP, mux=ts defines the muxing protocol. This allows a client to stream with a known
        // profile and thus, no .sdp file is needed
        std::string streamingOptions(":rtp{");

        streamingOptions += "dst="+outAddr+","
                            "port="+port+","
                            "mux=ts"
                            "}";

        libvlc_media_add_option(media, (transcodingOptions+streamingOptions).c_str());

        // As we encode a raw video buffer, we must pass to vlc the desired video specification.
        const std::string heightOption(":rawvid-height="+std::to_string(frameTL->getHeight()));
        const std::string widthOption(":rawvid-width="+std::to_string(frameTL->getWidth()));

        libvlc_media_add_option(media, ":demux=rawvid");
        libvlc_media_add_option(media, ":rawvid-fps=25");
        libvlc_media_add_option(media, heightOption.c_str());
        libvlc_media_add_option(media, widthOption.c_str());
        libvlc_media_add_option(media, ":rawvid-chroma=RV32"); // Chroma encodes the color portions : RV32 = RGBA ushort

        // Create a media player playing environment
        m_mediaPlayer = libvlc_media_player_new_from_media(media);

        // play the media_player
        libvlc_media_player_play(m_mediaPlayer);

        m_isInitialized = true;
    }

    // Whilst vlc instance is initialized, we need only to copy our buffer to the memory structure.
    m_imemData->m_videoBuffer = frame;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrameStreamer::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( s_FRAMETL, ::arData::TimeLine::s_OBJECT_PUSHED_SIG,
                      ::videoVLC::SFrameStreamer::s_UPDATE_FRAME_SLOT );

    return connections;
}

} // namespace videoVLC
