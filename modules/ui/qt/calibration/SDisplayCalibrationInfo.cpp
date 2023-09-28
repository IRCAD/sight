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

#include "modules/ui/qt/calibration/SDisplayCalibrationInfo.hpp"

#include <core/com/proxy.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/String.hpp>

#include <service/extension/AppConfig.hpp>

#include <sstream>

namespace sight::module::ui::qt::calibration
{

//------------------------------------------------------------------------------

static const core::com::slots::key_t DISPLAY_IMAGE_SLOT = "displayImage";
static const core::com::slots::key_t STOP_CONFIG_SLOT   = "stopConfig";

static const std::string s_SINGLE_IMAGE_CONFIG = "singleImageConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::SDisplayCalibrationInfo() noexcept
{
    new_slot(DISPLAY_IMAGE_SLOT, &SDisplayCalibrationInfo::displayImage, this);
    new_slot(STOP_CONFIG_SLOT, &SDisplayCalibrationInfo::stopConfig, this);
}

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::~SDisplayCalibrationInfo() noexcept =
    default;

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::starting()
{
    m_proxychannel = this->get_id() + "_stopConfig";
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::stopping()
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

void SDisplayCalibrationInfo::configuring()
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::updating()
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::stopConfig()
{
    if(m_configMgr)
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::displayImage(std::size_t idx)
{
    if(!m_configMgr)
    {
        // Grab images from our composite data
        const auto calInfo1 = m_calibrationInfo1.lock();
        SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", calInfo1);

        const auto calInfo2 = m_calibrationInfo2.lock();

        std::string strConfig = std::string(s_ONE_IMAGE_CONFIG);

        // Prepare configuration
        service::FieldAdaptorType replaceMap;

        data::Image::csptr img1 = calInfo1->getImage(idx);
        replaceMap["imageId1"]        = img1->get_id();
        replaceMap["calibrationData"] = calInfo1->get_id();
        data::PointList::csptr pointList1 = calInfo1->getPointList(img1);
        replaceMap["pointListId1"] = pointList1->get_id();

        if(calInfo2)
        {
            strConfig = std::string(s_TWO_IMAGES_CONFIG);

            data::Image::csptr img2 = calInfo2->getImage(idx);
            replaceMap["imageId2"] = img2->get_id();
            data::PointList::csptr pointList2 = calInfo2->getPointList(img2);
            replaceMap["pointListId2"] = pointList2->get_id();
        }

        replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;

        const auto config = service::extension::AppConfig::getDefault()->getAdaptedTemplateConfig(
            strConfig,
            replaceMap,
            true
        );

        // Launch configuration
        m_configMgr = service::app_config_manager::make();
        m_configMgr->setConfig(config);
        m_configMgr->launch();

        // Proxy to be notified of the window closure
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->connect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
