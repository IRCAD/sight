/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opDistorter/config.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>

#include <fwServices/IController.hpp>

namespace opDistorter
{
/**
 * @brief   SUndistortImage service undistort a video.
 *
 * @section Slots Slots
 * - \b changeState() : Called to enable/disable the undistortion.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::opDistorter::SUndistortImage">
            <in key="camera" uid="..." />
            <in key="input" uid="..." autoConnect="yes" />
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [::arData::Camera]: camera containing calibration information.
 * - \b input [::fwData::Image]: input image to undistort.
 * @subsection In-Out In-Out:
 * - \b output [::fwData::Image]: image undistorted.
 */

class OPDISTORTER_CLASS_API SUndistortImage : public fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SUndistortImage)(fwServices::IController));

    /**
     * @brief Constructor.
     */
    OPDISTORTER_API SUndistortImage() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SUndistortImage() noexcept
    {
    }

    /**
     * @name Slots API
     * @{
     */
    OPDISTORTER_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_STATE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeStateSlotType;
    ///@}

protected:

    /// Do nothing
    OPDISTORTER_API void configuring() override;

    /**
     * @brief This method is used to initialize the service.
     */
    OPDISTORTER_API void starting() override;

    /**
     * @brief This method is used to stop the service.
     */
    OPDISTORTER_API void stopping() override;

    /**
     * @brief Connect ::fwData::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     * and ::fwData::Image::s_BUFFER_MODIFIED_SIG to s_UPDATE_SLOT
     */
    OPDISTORTER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief This method is used to update the service.
     */
    OPDISTORTER_API void updating() override;

    /**
     * @brief Undistort the video.
     */
    OPDISTORTER_API void undistort();

    /**
     * @brief SLOT: enabled/disabled the undistort correction.
     */
    OPDISTORTER_API void changeState();

private:

    /// True if the undistortion is enabled.
    bool m_isEnabled;

    /// Camera corresponding to the video.
    ::arData::Camera::csptr m_camera;
};

} //namespace opDistorter
