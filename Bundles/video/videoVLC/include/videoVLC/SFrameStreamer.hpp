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

#pragma once

#include "videoVLC/config.hpp"

#include <fwServices/IOperator.hpp>

#include <vlc/vlc.h>

namespace videoVLC
{

struct MemoryVideoData;

/**
 * @brief This service streams frames pushed into an ::arData::FrameTL using rtp and VLC library.
 *
 * @section Signals Signals
 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::videoVLC::SFrameStreamer">
       <in key="frameTL" uid="..." />
       <outAddr>...</outAddr>
        <port>...</port>
   </service>
   @endcode
 * @subsection Input Input
 * - \b frameTL [::arData::FrameTL]: timeline containing video frames.
 */
class VIDEOVLC_CLASS_API SFrameStreamer : public ::fwServices::IOperator
{
public:
    fwCoreServiceClassDefinitionsMacro((SFrameStreamer)(::fwServices::IOperator))

    /**
     * @brief SFrameStreamer constructor.
     */
    VIDEOVLC_API SFrameStreamer() noexcept;

    /**
     * @brief SFrameStreamer destructor.
     */
    VIDEOVLC_API virtual ~SFrameStreamer() noexcept override;

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_FRAME_SLOT;

protected:

    /// Do nothing
    VIDEOVLC_API virtual void starting() override;

    /// Do nothing
    VIDEOVLC_API virtual void stopping() override;

    /// Do nothing
    VIDEOVLC_API virtual void configuring() override;

    /// Do nothing
    VIDEOVLC_API virtual void updating() override;

    /// Update frame slots
    VIDEOVLC_API virtual void updateFrame( ::fwCore::HiResClock::HiResClockType timestamp );

private:
    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connections
     *
     * Connect ::arData::TimeLine::s_OBJECT_PUSHED_SIG to s_UPDATE_SLOT
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    std::unique_ptr<MemoryVideoData> m_imemData;

    libvlc_media_player_t* m_mediaPlayer;

    bool m_isInitialized{false};

    /// RTP video stream output address config key
    std::string m_outAddrCfg;
    /// RTP video stream port config key
    std::string m_portCfg;

};

} // namespace videoVLC
