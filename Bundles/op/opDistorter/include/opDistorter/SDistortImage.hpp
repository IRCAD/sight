/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opDistorter/config.hpp"

#include <fwServices/IController.hpp>

namespace opDistorter
{

/**
 * @brief   Distort an image as if it is viewed by a lens of a camera. We thus need a camera calibration.
 *
 * @section Slots Slots
 * - \b changeState() : enabled/disabled the distort correction.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="::opDistorter::SDistortImage">
         <in key="camera" uid="..." />
         <inout key="image" uid="..." />
     </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera containing calibration information.
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to distort.
 */
class OPDISTORTER_CLASS_API SDistortImage : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDistortImage)(::fwServices::IController) );

    /// Constructor.
    OPDISTORTER_API SDistortImage() noexcept;

    /// Destructor. Does nothing
    OPDISTORTER_API virtual ~SDistortImage() noexcept;

    /**
     * @name Slots API
     * @{
     */
    OPDISTORTER_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_STATE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeStateSlotType;
    ///@}
    ///
protected:

    /// Does nothing
    OPDISTORTER_API virtual void configuring() override;

    /// Retrieve the camera.
    OPDISTORTER_API virtual void starting() override;

    /// Do nothing.
    OPDISTORTER_API virtual void stopping() override;

    /// Distort the image.
    OPDISTORTER_API virtual void updating() override;

    /// Distort the video.
    OPDISTORTER_API void distort();

    /// SLOT: enabled/disabled the distort correction.
    OPDISTORTER_API void changeState();

private:

    /// True if the undistortion is enabled.
    bool m_isEnabled;
};

} // opDistorter
