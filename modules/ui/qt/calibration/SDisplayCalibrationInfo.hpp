/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <data/CalibrationInfo.hpp>

#include <service/IAppConfigManager.hpp>
#include <service/IController.hpp>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   Launches an AppConfig to display calibration images.
 *
 *  This service works on a data::Composite.
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
     <service type="sight::module::ui::qt::calibration::SDisplayCalibrationInfo">
         <in key="calInfo1" uid="..." />
         <in key="calInfo2" uid="..." />
     </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [sight::data::CalibrationInfo]: calibration information for first camera.
 * - \b calInfo2 [sight::data::CalibrationInfo] (optional): calibration information for optional second camera.
 *
 * @section Slots Slots
 * - \b displayImage(std::size_t index): launches the configuration to display the calibration image at the given index
 * on
 *   an external window.
 * - \b stopConfig() : Stop the displayed configuration.
 */
class MODULE_UI_QT_CLASS_API SDisplayCalibrationInfo : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SDisplayCalibrationInfo, service::IController);

    /// Constructor. Does nothing.
    MODULE_UI_QT_API SDisplayCalibrationInfo() noexcept;

    /// Destructor. Does nothing.
    MODULE_UI_QT_API ~SDisplayCalibrationInfo() noexcept override;

protected:

    /// Starts the config
    void starting() override;

    /// Stops the config
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Does nothing
    void configuring() override;

private:

    /**
     * @name Slots
     * @}
     */
    /// Slot: stop the config.
    void stopConfig();

    /// Slot: Launch an appConfig to display an image on an external window.
    void displayImage(std::size_t idx);
    /**
     * @}
     */

    /// AppConfig manager, used to launch the config
    service::IAppConfigManager::sptr m_configMgr;

    static constexpr std::string_view s_ONE_IMAGE_CONFIG {"displayImageConfig"};
    static constexpr std::string_view s_TWO_IMAGES_CONFIG {"displayTwoImagesConfig"};

    std::string m_proxychannel; ///< Name of the channel used to connect stopConfig slot to the config frame closing.

    static constexpr std::string_view s_CALIBRATION_INFO_1 = "calInfo1";
    static constexpr std::string_view s_CALIBRATION_INFO_2 = "calInfo2";
    data::ptr<data::CalibrationInfo, data::Access::in> m_calibrationInfo1 {this, s_CALIBRATION_INFO_1, false};
    data::ptr<data::CalibrationInfo, data::Access::in> m_calibrationInfo2 {this, s_CALIBRATION_INFO_2, false, true};
};

} // namespace sight::module::ui::qt::calibration
