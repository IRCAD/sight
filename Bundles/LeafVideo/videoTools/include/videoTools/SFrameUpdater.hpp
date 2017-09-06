/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SFRAMEUPDATER_HPP__
#define __VIDEOTOOLS_SFRAMEUPDATER_HPP__

#include "videoTools/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>

namespace videoTools
{

/**
 * @brief   Service used to update frame and trigger render once timelines have been updated.
 *
 * @section Signals Signals
 * - \b renderRequested(): Emitted when the frame has been updated.
 *
 * @section Slots Slots
 * - \b updateFrame(::fwCore::HiResClock::HiResClockType): Called to extract the frame at the given timestamp.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoTools::SFrameUpdater">
            <in key="frameTL" uid="..." autoConnect="yes" />
            <inout key="frame" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b key1 [::arData::FrameTL]: timeline from which we synchronize images.
 * @subsection In-Out In-Out:
 * - \b key2 [::fwData::Image]: frame where to extract a buffer of a timeline.
 */
class VIDEOTOOLS_CLASS_API SFrameUpdater : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SFrameUpdater)(::fwServices::IController) );

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_FRAME_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> UpdateFrameSlotType;

    /// Type of signal m_sigRenderRequested
    typedef ::fwCom::Signal< void () > RenderRequestedSignalType;

    /// Key in m_signals map of signal m_sigObjectModified
    static const ::fwCom::Signals::SignalKeyType s_RENDER_REQUESTED_SIG;

    /// Constructor
    VIDEOTOOLS_API SFrameUpdater() noexcept;

    /// Destructorin
    VIDEOTOOLS_API virtual ~SFrameUpdater() noexcept;

protected:

    /// Starting method. Initialize timer.
    VIDEOTOOLS_API virtual void starting();

    /// Configuring method.
    VIDEOTOOLS_API virtual void configuring();

    /// Stopping method.
    VIDEOTOOLS_API virtual void stopping();

    /// Updating method. Deforms the mesh
    VIDEOTOOLS_API virtual void updating();

    /// Update frame slots
    VIDEOTOOLS_API virtual void updateFrame( ::fwCore::HiResClock::HiResClockType timestamp );

    /// Update the image according to the frame timeline.
    VIDEOTOOLS_API virtual void updateImage();

    /// Request Render
    VIDEOTOOLS_API void requestRender();

private:

    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Slots used when the frame have been refreshed
    UpdateFrameSlotType::sptr m_slotUpdateFrame;

    /// Signal that emits signal when refresh is requested
    RenderRequestedSignalType::sptr m_sigRenderRequested;

    /// Connections
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Frame timeline key
    std::string m_frameTLKey;

    /// Frame timeline
    ::arData::FrameTL::csptr m_frameTL;

    /// Image key
    std::string m_imageKey;

    /// Image
    ::fwData::Image::sptr m_image;

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Last matrix updated key
    std::string m_lastMatrixUpdatedKey;

    /// Hight resolution timer to log information about computing function time
    ::fwCore::HiResTimer m_hiRestimer;

    bool m_imageInitialized;
};

} //namespace videoTools

#endif /* __VIDEOTOOLS_SFRAMEUPDATER_HPP__ */
