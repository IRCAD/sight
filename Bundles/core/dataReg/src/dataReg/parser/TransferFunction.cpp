/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "dataReg/parser/TransferFunction.hpp"

#include <fwData/Color.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::TransferFunction, ::fwData::TransferFunction );

namespace dataReg
{
namespace parser
{

// ------------------------------------------------------------------------------

void TransferFunction::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("This method is deprecated, and this shouldn't be used.");
}

// ------------------------------------------------------------------------------

void TransferFunction::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::dynamicCast( _obj );
    SLM_ASSERT("TransferFunction not instanced", tf);

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    const ConfigurationType colorCfg = m_cfg->findConfigurationElement("colors");

    if(colorCfg)
    {
        const ::fwRuntime::ConfigurationElementContainer stepsConfig = colorCfg->findAllConfigurationElement("step");
        SLM_ASSERT("element 'step' is missing.", stepsConfig.size() != 0);

        for(ConfigurationType stepConfig :  stepsConfig.getElements())
        {
            SLM_ASSERT("Missing attribute 'color'", stepConfig->hasAttribute("color"));
            SLM_ASSERT("Missing attribute 'value'", stepConfig->hasAttribute("value"));

            double value         = std::stod(stepConfig->getAttributeValue("value"));
            std::string strColor = stepConfig->getAttributeValue("color");

            ::fwData::Color::sptr newColor = ::fwData::Color::New();
            newColor->setRGBA(strColor);

            ::fwData::TransferFunction::TFColor color(newColor->red(), newColor->green(),
                                                      newColor->blue(), newColor->alpha());
            tf->addTFColor(value, color);
        }
        tf->setWLMinMax(tf->getMinMaxTFValues());
    }
}

// ------------------------------------------------------------------------------

} // namespace parser
} // namespace dataReg

