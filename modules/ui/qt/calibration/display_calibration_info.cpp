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

#include "modules/ui/qt/calibration/display_calibration_info.hpp"

#include <core/com/proxy.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/string.hpp>

#include <service/extension/config.hpp>

#include <sstream>

namespace sight::module::ui::qt::calibration
{

//------------------------------------------------------------------------------

static const core::com::slots::key_t DISPLAY_IMAGE_SLOT = "displayImage";
static const core::com::slots::key_t STOP_CONFIG_SLOT   = "stopConfig";

static const std::string s_SINGLE_IMAGE_CONFIG = "singleImageConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

display_calibration_info::display_calibration_info() noexcept
{
    new_slot(DISPLAY_IMAGE_SLOT, &display_calibration_info::displayImage, this);
    new_slot(STOP_CONFIG_SLOT, &display_calibration_info::stopConfig, this);
}

//------------------------------------------------------------------------------

display_calibration_info::~display_calibration_info() noexcept =
    default;

//------------------------------------------------------------------------------

void display_calibration_info::starting()
{
    m_proxychannel = this->get_id() + "_stopConfig";
}

//------------------------------------------------------------------------------

void display_calibration_info::stopping()
{
    if(m_configMgr)
    {
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->disconnect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
        m_configMgr->stopAndDestroy();
        m_configMgr.reset();
    }
}

//------------------------------------------------------------------------------

void display_calibration_info::configuring()
{
}

//------------------------------------------------------------------------------

void display_calibration_info::updating()
{
}

//------------------------------------------------------------------------------

void display_calibration_info::stopConfig()
{
    if(m_configMgr)
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

void display_calibration_info::displayImage(std::size_t idx)
{
    if(!m_configMgr)
    {
        // Grab images from our composite data
        const auto calInfo1 = m_calibrationInfo1.lock();
        SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", calInfo1);

        const auto calInfo2 = m_calibrationInfo2.lock();

        std::string strConfig = std::string(s_ONE_IMAGE_CONFIG);

        // Prepare configuration
        sight::app::field_adaptor_t replaceMap;

        data::image::csptr img1 = calInfo1->getImage(idx);
        replaceMap["imageId1"]        = img1->get_id();
        replaceMap["calibrationData"] = calInfo1->get_id();
        data::point_list::csptr pointList1 = calInfo1->getPointList(img1);
        replaceMap["pointListId1"] = pointList1->get_id();

        if(calInfo2)
        {
            strConfig = std::string(s_TWO_IMAGES_CONFIG);

            data::image::csptr img2 = calInfo2->getImage(idx);
            replaceMap["imageId2"] = img2->get_id();
            data::point_list::csptr pointList2 = calInfo2->getPointList(img2);
            replaceMap["pointListId2"] = pointList2->get_id();
        }

        replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;

        const auto config = app::extension::config::getDefault()->getAdaptedTemplateConfig(
            strConfig,
            replaceMap,
            true
        );

        // Launch configuration
        m_configMgr = app::config_manager::make();
        m_configMgr->setConfig(config);
        m_configMgr->launch();

        // Proxy to be notified of the window closure
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->connect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
