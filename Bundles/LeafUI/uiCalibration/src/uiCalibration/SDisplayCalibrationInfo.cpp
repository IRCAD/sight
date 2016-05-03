/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SDisplayCalibrationInfo.hpp"

#include <fwCom/Slots.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <arData/CalibrationInfo.hpp>

#include <sstream>

namespace uiCalibration
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro(::fwServices::IController, ::uiCalibration::SDisplayCalibrationInfo, ::fwData::Composite);

const ::fwCom::Slots::SlotKeyType SDisplayCalibrationInfo::s_DISPLAY_IMAGE_SLOT = "displayImage";
static const ::fwCom::Slots::SlotKeyType s_STOP_CONFIG_SLOT                     = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::SDisplayCalibrationInfo() throw ()
{
    newSlot( s_DISPLAY_IMAGE_SLOT, &SDisplayCalibrationInfo::displayImage, this );
    newSlot(s_STOP_CONFIG_SLOT, &SDisplayCalibrationInfo::stopConfig, this);
}

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::~SDisplayCalibrationInfo() throw ()
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::starting() throw (::fwTools::Failed)
{
    m_proxychannel = this->getID() + "_stopConfig";
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::stopping() throw (::fwTools::Failed)
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

void SDisplayCalibrationInfo::configuring() throw (fwTools::Failed)
{
    SLM_ASSERT("Parameter CalInfo1 are not found in xml", m_configuration->findConfigurationElement("CalInfo1") );
    std::string calInfoKey = m_configuration->findConfigurationElement("CalInfo1")->getValue();
    m_calInfoKeys.push_back(calInfoKey);

    if(m_configuration->findConfigurationElement("CalInfo2"))
    {
        calInfoKey = m_configuration->findConfigurationElement("CalInfo2")->getValue();
        m_calInfoKeys.push_back(calInfoKey);
    }
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::updating() throw (::fwTools::Failed)
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

void SDisplayCalibrationInfo::info(std::ostream &_sstream)
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::displayImage(size_t idx)
{
    if( !m_configMgr )
    {
        // Grab images from our composite data
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        ::fwData::Composite::ConstIteratorType cmpIt1 = composite->find(m_calInfoKeys[0]);
        SLM_ASSERT("Key : "+m_calInfoKeys[0]+" not found !",cmpIt1 != composite->end());

        ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(cmpIt1->second);
        SLM_ASSERT("Object "+m_calInfoKeys[0]+" is not a CalibrationInfo !",calInfo1);

        ::fwData::Composite::ConstIteratorType cmpIt2;
        ::arData::CalibrationInfo::sptr calInfo2;
        std::string strConfig = "displayImageConfig";

        if(m_calInfoKeys.size() > 1)
        {
            cmpIt2 = composite->find(m_calInfoKeys[1]);
            SLM_ASSERT("Key : "+m_calInfoKeys[1]+" not found !",cmpIt2 != composite->end());
            calInfo2 = ::arData::CalibrationInfo::dynamicCast(cmpIt2->second);
            SLM_ASSERT("Object "+m_calInfoKeys[1]+" is not a CalibrationInfo !",calInfo2);
            strConfig = "displayTwoImagesConfig";
        }

        // Prepare configuration
        std::ostringstream os;
        os << idx;

        ::fwServices::registry::AppConfig::FieldAdaptorType replaceMap;
        replaceMap["GENERIC_UID"] = ::fwServices::registry::AppConfig::getUniqueIdentifier(strConfig);

        ::fwData::Image::sptr img1           = calInfo1->getImage(idx);
        replaceMap["imageId1"]               = img1->getID();
        ::fwData::PointList::sptr pointList1 = calInfo1->getPointList(img1);
        replaceMap["pointListId1"]           = pointList1->getID();

        ::fwRuntime::ConfigurationElement::csptr config;
        if(m_calInfoKeys.size() > 1)
        {
            ::fwData::Image::sptr img2           = calInfo2->getImage(idx);
            replaceMap["imageId2"]               = img2->getID();
            ::fwData::PointList::sptr pointList2 = calInfo2->getPointList(img2);
            replaceMap["pointListId2"]           = pointList2->getID();
        }

        replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;

        config = ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig(strConfig, replaceMap);

        // Launch configuration
        m_configMgr = ::fwServices::AppConfigManager::New();
        m_configMgr->setConfig( config );
        m_configMgr->launch();

        // Proxy to be notified of the window closure
        ::fwServices::registry::Proxy::sptr proxies = ::fwServices::registry::Proxy::getDefault();
        proxies->connect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
    }
}

//------------------------------------------------------------------------------
} // uiCalibration

