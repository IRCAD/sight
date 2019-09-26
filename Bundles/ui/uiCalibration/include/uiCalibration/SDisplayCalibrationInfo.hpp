/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "uiCalibration/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/AppConfigManager.hpp>
#include <fwServices/IController.hpp>

namespace uiCalibration
{

/**
 * @brief   Launch an AppConfig to display calibration images.
 *
 *  This service works on a ::fwData::Composite.
 *  It starts/stops a template configuration. This service can display one or two images.
 *
 *  If there is one calibration info in the service parameters, the template configuration used is
 *  "DisplayImageConfig.xml". The parameters are :
 *  - "imageId1" is the image to be displayed.
 *
 *  If there are two calibration infos, the template configuration used is "DisplayTwoImagesConfig.xml".
 *  The parameters are :
 *  - "imageId1" is the first image.
 *  - "imageId2" is the second image.
 *
 * @section XML XML Configuration
 * @code{.xml}
     <service type="::uiCalibration::SDisplayCalibrationInfo">
         <in key="calInfo1" uid="..." />
         <in key="calInfo2" uid="..." />
     </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [::arData::CalibrationInfo]: calibration information for first camera.
 * - \b calInfo2 [::arData::CalibrationInfo] (optionnal): calibration information for optionnal second camera.
 *
 * @section Slots Slots
 * - \b displayImage(size_t index) : Launchs the configuration to display the calibration image at the given index on
 *   an external window.
 * - \b stopConfig() : Stop the displayed configuration.
 */
class UICALIBRATION_CLASS_API SDisplayCalibrationInfo : public ::fwServices::IController
{

public:

    fwCoreServiceMacro(SDisplayCalibrationInfo, ::fwServices::IController);

    /// Constructor. Does nothing.
    UICALIBRATION_API SDisplayCalibrationInfo() noexcept;

    /// Destructor. Does nothing.
    UICALIBRATION_API virtual ~SDisplayCalibrationInfo() noexcept;

protected:

    /// Starts the config
    virtual void starting() override;

    /// Stops the config
    virtual void stopping() override;

    /// Does nothing
    virtual void updating() override;

    /// Does nothing
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

private:

    /**
     * @name Slots
     * @}
     */
    /// Slot: stop the config.
    void stopConfig();

    /// Slot: Launch an appConfig to display an image on an external window.
    void displayImage(size_t idx);
    /**
     * @}
     */

    /// AppConfig manager, used to launch the config
    ::fwServices::IAppConfigManager::sptr m_configMgr;

    std::string m_proxychannel; ///< Name of the channel used to connect stopConfig slot to the config frame closing.
};

} // uiCalibration
