/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SDisplayCalibrationInfo.hpp"

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/String.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <sstream>

namespace uiCalibration
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro(::fwServices::IController, ::uiCalibration::SDisplayCalibrationInfo, ::fwData::Object);

static const ::fwCom::Slots::SlotKeyType s_DISPLAY_IMAGE_SLOT = "displayImage";
static const ::fwCom::Slots::SlotKeyType s_STOP_CONFIG_SLOT   = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

static const std::string s_CALIBRATION_INFO_1 = "calInfo1";
static const std::string s_CALIBRATION_INFO_2 = "calInfo2";

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::SDisplayCalibrationInfo() noexcept
{
    newSlot( s_DISPLAY_IMAGE_SLOT, &SDisplayCalibrationInfo::displayImage, this );
    newSlot(s_STOP_CONFIG_SLOT, &SDisplayCalibrationInfo::stopConfig, this);
}

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::~SDisplayCalibrationInfo() noexcept
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::starting()
{
    m_proxychannel = this->getID() + "_stopConfig";
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::stopping()
{
    if (m_configMgr)
    {
        ::fwServices::registry::Proxy::sptr proxies = ::fwServices::registry::Proxy::getDefault();
        proxies->disconnect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
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
    if( m_configMgr )
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::info(std::ostream& _sstream)
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::displayImage(size_t idx)
{
    if( !m_configMgr )
    {
        // Grab images from our composite data
        ::arData::CalibrationInfo::csptr calInfo1 = this->getInput< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_1);
        SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

        ::arData::CalibrationInfo::csptr calInfo2 = this->getInput< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_2);

        std::string strConfig = "displayImageConfig";

        // Prepare configuration
        ::fwServices::registry::FieldAdaptorType replaceMap;

        ::fwData::Image::sptr img1           = calInfo1->getImage(idx);
        replaceMap["imageId1"]               = img1->getID();
        ::fwData::PointList::sptr pointList1 = calInfo1->getPointList(img1);
        replaceMap["pointListId1"]           = pointList1->getID();

        ::fwRuntime::ConfigurationElement::csptr config;
        if(calInfo2)
        {
            strConfig = "displayTwoImagesConfig";

            ::fwData::Image::sptr img2           = calInfo2->getImage(idx);
            replaceMap["imageId2"]               = img2->getID();
            ::fwData::PointList::sptr pointList2 = calInfo2->getPointList(img2);
            replaceMap["pointListId2"]           = pointList2->getID();
        }

        replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;

        config =
            ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig(strConfig, replaceMap, true);

        // Launch configuration
        m_configMgr = ::fwServices::IAppConfigManager::New();
        m_configMgr->::fwServices::IAppConfigManager::setConfig( config );
        m_configMgr->launch();

        // Proxy to be notified of the window closure
        ::fwServices::registry::Proxy::sptr proxies = ::fwServices::registry::Proxy::getDefault();
        proxies->connect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
    }
}

//------------------------------------------------------------------------------
} // uiCalibration

