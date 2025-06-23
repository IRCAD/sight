/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "module/ui/qt/calibration/display_calibration_info.hpp"

#include <core/com/proxy.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/string.hpp>

#include <service/extension/config.hpp>

#include <sstream>

namespace sight::module::ui::qt::calibration
{

//------------------------------------------------------------------------------

static const core::com::slots::key_t DISPLAY_IMAGE_SLOT = "display_image";
static const core::com::slots::key_t STOP_CONFIG_SLOT   = "stopConfig";

static const std::string SINGLE_IMAGE_CONFIG = "singleImageConfig";
static const std::string TWO_IMAGES_CONFIG   = "twoImagesConfig";

static const std::string CLOSE_CONFIG_ID = "CLOSE_CONFIG";

//------------------------------------------------------------------------------

display_calibration_info::display_calibration_info() noexcept
{
    new_slot(DISPLAY_IMAGE_SLOT, &display_calibration_info::display_image, this);
    new_slot(STOP_CONFIG_SLOT, &display_calibration_info::stop_config, this);
}

//------------------------------------------------------------------------------

void display_calibration_info::configuring(const config_t& _config)
{
    m_single_image_config = _config.get<std::string>("config.<xmlattr>." + SINGLE_IMAGE_CONFIG, m_single_image_config);
    m_two_images_config   = _config.get<std::string>("config.<xmlattr>." + TWO_IMAGES_CONFIG, m_two_images_config);
}

//------------------------------------------------------------------------------

void display_calibration_info::starting()
{
    m_proxychannel = this->get_id() + "_stopConfig";
}

//------------------------------------------------------------------------------

void display_calibration_info::stopping()
{
    if(m_config_mgr)
    {
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->disconnect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
        m_config_mgr->stop_and_destroy();
        m_config_mgr.reset();
    }
}

//------------------------------------------------------------------------------

void display_calibration_info::updating()
{
}

//------------------------------------------------------------------------------

void display_calibration_info::stop_config()
{
    if(m_config_mgr)
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

void display_calibration_info::display_image(std::size_t _idx)
{
    if(!m_config_mgr)
    {
        // Grab images from our map data
        const auto cal_info1 = m_calibration_info1.lock();
        SIGHT_ASSERT("Object " << CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

        const auto cal_info2 = m_calibration_info2.lock();

        std::string str_config = m_single_image_config;

        // Prepare configuration
        sight::app::field_adaptor_t replace_map;

        data::image::csptr img1 = cal_info1->get_image(_idx);
        replace_map["imageId1"]        = img1->get_id();
        replace_map["calibrationData"] = cal_info1->get_id();
        data::point_list::csptr point_list1 = cal_info1->get_point_list(img1);
        replace_map["pointListId1"] = point_list1->get_id();

        if(cal_info2)
        {
            str_config = m_two_images_config;

            data::image::csptr img2 = cal_info2->get_image(_idx);
            replace_map["imageId2"] = img2->get_id();
            data::point_list::csptr point_list2 = cal_info2->get_point_list(img2);
            replace_map["pointListId2"] = point_list2->get_id();
        }

        replace_map[CLOSE_CONFIG_ID] = m_proxychannel;

        const auto config = app::extension::config::get()->get_adapted_template_config(
            str_config,
            replace_map,
            sight::app::extension::config::get_unique_identifier(str_config)
        );

        // Launch configuration
        m_config_mgr = app::config_manager::make();
        m_config_mgr->set_config(config);
        m_config_mgr->launch();

        // Proxy to be notified of the window closure
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->connect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
