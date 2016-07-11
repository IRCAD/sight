/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCom/Signals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include "ctrlCamera/SExtractDeviceInfo.hpp"


#include <fwData/Composite.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/registry/ServiceConfig.hpp>


#ifdef ANDROID
#include <sys/system_properties.h>
#endif

namespace ctrlCamera
{

fwServicesRegisterMacro(::fwServices::IController, ::ctrlCamera::SExtractDeviceInfo, ::fwData::Object);


//-----------------------------------------------------------------------------

SExtractDeviceInfo::SExtractDeviceInfo()
{

}

//-----------------------------------------------------------------------------

SExtractDeviceInfo::~SExtractDeviceInfo()
{
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::configuring() throw( ::fwTools::Failed )
{
    ConfigurationType cameraCfg = m_configuration->findConfigurationElement("cameraUid");
    SLM_ASSERT("Missing 'cameraUid' config.",cameraCfg);

    m_cameraUid = cameraCfg->getValue();

    ConfigurationType configCfg = m_configuration->findConfigurationElement("configId");
    ::fwRuntime::ConfigurationElement::csptr deviceConfig;
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

    SLM_ASSERT("'devices or TabletSelectorServiceConfig' missing",deviceConfig);

    for(const CfgContainer::value_type& elt : deviceConfig->getElements())
    {
        ConfigurationType nameCfg = elt->findConfigurationElement("name");
        SLM_ASSERT("Missing 'name' config element.",nameCfg);

        m_devicesConfig[nameCfg->getValue()] = elt;
    }
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::starting() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::updating() throw( ::fwTools::Failed )
{
    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_cameraUid);
    m_camera                    = ::arData::Camera::dynamicCast(obj);
    SLM_ASSERT("Camera '" + m_cameraUid + "' not found.", m_camera);

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
        if ( fgets (buffer, 128, file) == NULL )
        {
            break;
        }
        fputs (buffer, stdout);
    }

    pclose(file);

    device = std::string(buffer);
    device.pop_back(); // To remove a newline character
#else
    device = "desktop";
#endif

    SLM_DEBUG("Device: " + device);

    ConfigurationType config = m_devicesConfig[device];
    double width,height,fx,fy,cx,cy,k1,k2,p1,p2,k3,skew;
    if (config)
    {
        ConfigurationType intrinsicCfg = config->findConfigurationElement("intrinsic");
        SLM_ASSERT("Missing 'intrinsic' config element.",intrinsicCfg);

        ConfigurationType cfgWidth = intrinsicCfg->findConfigurationElement("width");
        SLM_ASSERT("Missing 'width' config element.",cfgWidth);
        width = ::boost::lexical_cast< double >(cfgWidth->getValue());

        ConfigurationType cfgHeight = intrinsicCfg->findConfigurationElement("height");
        SLM_ASSERT("Missing 'height' config element.",cfgHeight);
        height = ::boost::lexical_cast< double >(cfgHeight->getValue());

        ConfigurationType cfgFx = intrinsicCfg->findConfigurationElement("fx");
        SLM_ASSERT("Missing 'fx' config element.",cfgFx);
        fx = ::boost::lexical_cast< double >(cfgFx->getValue());

        ConfigurationType cfgFy = intrinsicCfg->findConfigurationElement("fy");
        SLM_ASSERT("Missing 'fy' config element.",cfgFy);
        fy = ::boost::lexical_cast< double >(cfgFy->getValue());

        ConfigurationType cfgCx = intrinsicCfg->findConfigurationElement("cx");
        SLM_ASSERT("Missing 'cx' config element.",cfgCx);
        cx = ::boost::lexical_cast< double >(cfgCx->getValue());

        ConfigurationType cfgCy = intrinsicCfg->findConfigurationElement("cy");
        SLM_ASSERT("Missing 'cy' config element.",cfgCy);
        cy = ::boost::lexical_cast< double >(cfgCy->getValue());

        ConfigurationType cfgK1 = intrinsicCfg->findConfigurationElement("k1");
        SLM_ASSERT("Missing 'k1' config element.",cfgK1);
        k1 = ::boost::lexical_cast< double >(cfgK1->getValue());

        ConfigurationType cfgK2 = intrinsicCfg->findConfigurationElement("k2");
        SLM_ASSERT("Missing 'k2' config element.",cfgK2);
        k2 = ::boost::lexical_cast< double >(cfgK2->getValue());

        ConfigurationType cfgP1 = intrinsicCfg->findConfigurationElement("p1");
        SLM_ASSERT("Missing 'p1' config element.",cfgP1);
        p1 = ::boost::lexical_cast< double >(cfgP1->getValue());

        ConfigurationType cfgP2 = intrinsicCfg->findConfigurationElement("p2");
        SLM_ASSERT("Missing 'p2' config element.",cfgP2);
        p2 = ::boost::lexical_cast< double >(cfgP2->getValue());

        ConfigurationType cfgK3 = intrinsicCfg->findConfigurationElement("k3");
        SLM_ASSERT("Missing 'k3' config element.",cfgK3);
        k3 = ::boost::lexical_cast< double >(cfgK3->getValue());

        ConfigurationType cfgSkew = intrinsicCfg->findConfigurationElement("skew");
        SLM_ASSERT("Missing 'skew' config element.",cfgSkew);
        skew = ::boost::lexical_cast< double >(cfgSkew->getValue());
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

    m_camera->setWidth(width);
    m_camera->setHeight(height);
    m_camera->setFx(fx);
    m_camera->setFy(fy);
    m_camera->setCx(cx);
    m_camera->setCy(cy);
    m_camera->setDistortionCoefficient(k1,k2,p1,p2,k3);
    m_camera->setSkew(skew);

    OSLM_DEBUG("cx: " << m_camera->getCx() << ", cy: "<< m_camera->getCy() <<
               ", fx: "<< m_camera->getFx() <<  ", fy: " << m_camera->getFy());

    OSLM_DEBUG("k1: " <<k1 << ", k2: "<< k2 << ", p1: "<< p1 <<  ", p2: " << p2 << ", k3: " << k3);

    ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = m_camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >
              ( ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SExtractDeviceInfo::stopping() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------


} // namespace ctrlCamera
