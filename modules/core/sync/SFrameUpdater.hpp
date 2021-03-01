/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/sync/config.hpp"

#include <core/HiResClock.hpp>

#include <data/FrameTL.hpp>
#include <data/Image.hpp>
#include <data/Matrix4.hpp>

#include <service/IController.hpp>

namespace sight::module::sync
{

/**
 * @brief   Service used to update frame and trigger render once timelines have been updated.
 *
 * @section Signals Signals
 * - \b renderRequested(): Emitted when the frame has been updated.
 *
 * @section Slots Slots
 * - \b updateFrame(core::HiResClock::HiResClockType): Called to extract the frame at the given timestamp.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::sight::module::sync::SFrameUpdater">
            <in key="frameTL" uid="..." autoConnect="yes" />
            <inout key="frame" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b key1 [sight::data::FrameTL]: timeline from which we synchronize images.
 * @subsection In-Out In-Out:
 * - \b key2 [sight::data::Image]: frame where to extract a buffer of a timeline.
 */
class MODULE_SYNC_CLASS_API SFrameUpdater : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SFrameUpdater, service::IController)

    static const core::com::Slots::SlotKeyType s_UPDATE_FRAME_SLOT;

    /// Type of signal m_sigRenderRequested
    typedef core::com::Signal< void () > RenderRequestedSignalType;

    /// Key in m_signals map of signal m_sigObjectModified
    static const core::com::Signals::SignalKeyType s_RENDER_REQUESTED_SIG;

    /// Constructor
    MODULE_SYNC_API SFrameUpdater() noexcept;

    /// Destructorin
    MODULE_SYNC_API virtual ~SFrameUpdater() noexcept;

protected:

    /// Starting method. Initialize timer.
    MODULE_SYNC_API virtual void starting() override;

    /// Configuring method.
    MODULE_SYNC_API virtual void configuring() override;

    /// Stopping method.
    MODULE_SYNC_API virtual void stopping() override;

    /// Updating method. Deforms the mesh
    MODULE_SYNC_API virtual void updating() override;

    /// Update frame slots
    MODULE_SYNC_API virtual void updateFrame( core::HiResClock::HiResClockType timestamp );

    /// Update the image according to the frame timeline.
    MODULE_SYNC_API virtual void updateImage();

    /// Request Render
    MODULE_SYNC_API void requestRender();

    /// Reset the last timestamp when the timeline is cleared
    MODULE_SYNC_API void resetTimeline();

private:

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connections
     *
     * Connect data::TimeLine::s_OBJECT_PUSHED_SIG to s_UPDATE_FRAME_SLOT
     * Connect data::TimeLine::s_CLEARED_SIG to s_RESET_TIMELINE_SLOT
     */
    service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Frame timeline
    data::FrameTL::csptr m_frameTL;

    /// Image
    data::Image::sptr m_image;

    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp;

    /// Hight resolution timer to log information about computing function time
    core::HiResTimer m_hiRestimer;

    bool m_imageInitialized;
};

} //namespace sight::module::sync
