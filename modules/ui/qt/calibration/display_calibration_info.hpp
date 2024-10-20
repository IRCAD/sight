/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <app/config_manager.hpp>

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/calibration_info.hpp>

#include <service/controller.hpp>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   Launches an config to display calibration images.
 *
 *  This service works on a data::map.
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
     <service type="sight::module::ui::qt::calibration::display_calibration_info">
         <in key="calInfo1" uid="..." />
         <in key="calInfo2" uid="..." />
     </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [sight::data::calibration_info]: calibration information for first camera.
 * - \b calInfo2 [sight::data::calibration_info] (optional): calibration information for optional second camera.
 *
 * @section Slots Slots
 * - \b display_image(std::size_t index): launches the configuration to display the calibration image at the given index
 * on
 *   an external window.
 * - \b stopConfig() : Stop the displayed configuration.
 */
class display_calibration_info final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(display_calibration_info, service::controller);

    /// Constructor. Does nothing.
    display_calibration_info() noexcept;

    /// Destructor. Does nothing.
    ~display_calibration_info() noexcept final = default;

protected:

    /// Set the configurations to start for intrinsic and extrinsic
    void configuring(const config_t& _config) final;

    /// Starts the config
    void starting() final;

    /// Stops the config
    void stopping() final;

    /// Does nothing
    void updating() final;

private:

    /**
     * @name Slots
     * @}
     */
    /// Slot: stop the config.
    void stop_config();

    /// Slot: Launch an appConfig to display an image on an external window.
    void display_image(std::size_t _idx);
    /**
     * @}
     */

    /// config manager, used to launch the config
    app::config_manager::sptr m_config_mgr;

    std::string m_single_image_config {"displayImageConfig"};
    std::string m_two_images_config {"displayTwoImagesConfig"};

    std::string m_proxychannel; ///< Name of the channel used to connect stopConfig slot to the config frame closing.

    static constexpr std::string_view CALIBRATION_INFO_1 = "calInfo1";
    static constexpr std::string_view CALIBRATION_INFO_2 = "calInfo2";
    data::ptr<data::calibration_info, data::access::in> m_calibration_info1 {this, CALIBRATION_INFO_1};
    data::ptr<data::calibration_info, data::access::in> m_calibration_info2 {this, CALIBRATION_INFO_2, true};
};

} // namespace sight::module::ui::qt::calibration
