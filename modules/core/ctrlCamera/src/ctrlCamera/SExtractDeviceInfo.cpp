/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <core/com/Signals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include "ctrlCamera/SExtractDeviceInfo.hpp"

#include <data/Composite.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <core/runtime/ConfigurationElement.hpp>

#include <fwServices/registry/ServiceConfig.hpp>

#ifdef ANDROID
#include <sys/system_properties.h>
#endif

namespace ctrlCamera
{

fwServicesRegisterMacro(::fwServices::IController, ::ctrlCamera::SExtractDeviceInfo, data::Object)

//-----------------------------------------------------------------------------

SExtractDeviceInfo::SExtractDeviceInfo()
{

}

//-----------------------------------------------------------------------------

SExtractDeviceInfo::~SExtractDeviceInfo()
{
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::configuring()
{
    ConfigurationType configCfg = m_configuration->findConfigurationElement("configId");
    core::runtime::ConfigurationElement::csptr deviceConfig;
    if(configCfg)
    {

        deviceConfig = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
            configCfg->getValue(),
            "::ctrlCamera::SExtractDeviceInfo");
    }
    else
    {
        deviceConfig = m_configuration->findConfigurationElement("devices");
    }

    SLM_ASSERT("'devices or TabletSelectorServiceConfig' missing", deviceConfig);

    for(const CfgContainer::value_type& elt : deviceConfig->getElements())
    {
        ConfigurationType nameCfg = elt->findConfigurationElement("name");
        SLM_ASSERT("Missing 'name' config element.", nameCfg);

        m_devicesConfig[nameCfg->getValue()] = elt;
    }
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::starting()
{
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::updating()
{
    data::Camera::sptr camera = this->getInOut< data::Camera >("camera");
    SLM_ASSERT("Camera 'camera' not found.", camera);

    std::string device;

#ifdef ANDROID
    char model_string[PROP_VALUE_MAX+1];
    // __system_property_get("ro.product.model", model_string); this is deprecated

    std::string command = "getprop ro.product.model";
    FILE* file          = popen(command.c_str(), "r");
    SLM_ASSERT("Unable to get the device name", file);

    char buffer[128];
    while (!feof(file))
    {
        if ( fgets(buffer, 128, file) == NULL )
        {
            break;
        }
        fputs(buffer, stdout);
    }

    pclose(file);

    device = std::string(buffer);
    device.pop_back(); // To remove a newline character
#else
    device = "desktop";
#endif

    SLM_DEBUG("Device: " + device);

    ConfigurationType config = m_devicesConfig[device];
    double width, height, fx, fy, cx, cy, k1, k2, p1, p2, k3, skew;
    if (config)
    {
        ConfigurationType intrinsicCfg = config->findConfigurationElement("intrinsic");
        SLM_ASSERT("Missing 'intrinsic' config element.", intrinsicCfg);

        ConfigurationType cfgWidth = intrinsicCfg->findConfigurationElement("width");
        SLM_ASSERT("Missing 'width' config element.", cfgWidth);
        width = std::stod(cfgWidth->getValue());

        ConfigurationType cfgHeight = intrinsicCfg->findConfigurationElement("height");
        SLM_ASSERT("Missing 'height' config element.", cfgHeight);
        height = std::stod(cfgHeight->getValue());

        ConfigurationType cfgFx = intrinsicCfg->findConfigurationElement("fx");
        SLM_ASSERT("Missing 'fx' config element.", cfgFx);
        fx = std::stod(cfgFx->getValue());

        ConfigurationType cfgFy = intrinsicCfg->findConfigurationElement("fy");
        SLM_ASSERT("Missing 'fy' config element.", cfgFy);
        fy = std::stod(cfgFy->getValue());

        ConfigurationType cfgCx = intrinsicCfg->findConfigurationElement("cx");
        SLM_ASSERT("Missing 'cx' config element.", cfgCx);
        cx = std::stod(cfgCx->getValue());

        ConfigurationType cfgCy = intrinsicCfg->findConfigurationElement("cy");
        SLM_ASSERT("Missing 'cy' config element.", cfgCy);
        cy = std::stod(cfgCy->getValue());

        ConfigurationType cfgK1 = intrinsicCfg->findConfigurationElement("k1");
        SLM_ASSERT("Missing 'k1' config element.", cfgK1);
        k1 = std::stod(cfgK1->getValue());

        ConfigurationType cfgK2 = intrinsicCfg->findConfigurationElement("k2");
        SLM_ASSERT("Missing 'k2' config element.", cfgK2);
        k2 = std::stod(cfgK2->getValue());

        ConfigurationType cfgP1 = intrinsicCfg->findConfigurationElement("p1");
        SLM_ASSERT("Missing 'p1' config element.", cfgP1);
        p1 = std::stod(cfgP1->getValue());

        ConfigurationType cfgP2 = intrinsicCfg->findConfigurationElement("p2");
        SLM_ASSERT("Missing 'p2' config element.", cfgP2);
        p2 = std::stod(cfgP2->getValue());

        ConfigurationType cfgK3 = intrinsicCfg->findConfigurationElement("k3");
        SLM_ASSERT("Missing 'k3' config element.", cfgK3);
        k3 = std::stod(cfgK3->getValue());

        ConfigurationType cfgSkew = intrinsicCfg->findConfigurationElement("skew");
        SLM_ASSERT("Missing 'skew' config element.", cfgSkew);
        skew = std::stod(cfgSkew->getValue());
    }
    else
    {
        SLM_DEBUG(" Device "+ device + " not found, default calibration is set ");

        width  = 640.;
        height = 480.;
        fx     = 600.;
        fy     = 600.;
        cx     = 320.;
        cy     = 240.;
        k1     = 0.;
        k2     = 0.;
        p1     = 0.;
        p2     = 0.;
        k3     = 0.;
        skew   = 0.;
    }

    camera->setWidth(width);
    camera->setHeight(height);
    camera->setFx(fx);
    camera->setFy(fy);
    camera->setCx(cx);
    camera->setCy(cy);
    camera->setDistortionCoefficient(k1, k2, p1, p2, k3);
    camera->setSkew(skew);

    SLM_DEBUG("cx: " << camera->getCx() << ", cy: "<< camera->getCy() <<
              ", fx: "<< camera->getFx() <<  ", fy: " << camera->getFy());

    SLM_DEBUG("k1: " <<k1 << ", k2: "<< k2 << ", p1: "<< p1 <<  ", p2: " << p2 << ", k3: " << k3);

    data::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal< data::Camera::IntrinsicCalibratedSignalType >
              ( data::Camera::s_INTRINSIC_CALIBRATED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace ctrlCamera
