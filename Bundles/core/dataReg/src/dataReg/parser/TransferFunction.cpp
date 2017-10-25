/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "dataReg/parser/TransferFunction.hpp"

#include <fwData/Color.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwRuntime/Convert.hpp>

#include <fwServices/macros.hpp>

#include <boost/property_tree/xml_parser.hpp>
fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::TransferFunction, ::fwData::TransferFunction );

namespace dataReg
{
namespace parser
{

// ------------------------------------------------------------------------------

void TransferFunction::updating( )
{
    SLM_FATAL("This method is deprecated, and this shouldn't be used.");
}

// ------------------------------------------------------------------------------

void TransferFunction::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::dynamicCast( _obj );
    SLM_ASSERT("TransferFunction not instanced", tf);

    const ConfigType config = ::fwRuntime::Convert::toPropertyTree(m_cfg).get_child("object");
    if(config.count("colors"))
    {
        const ConfigType colorCfg = config.get_child("colors");

        const bool isDefault = colorCfg.get("<xmlattr>.default", false);
        if(isDefault)
        {
            ::fwData::TransferFunction::sptr defaultTf = ::fwData::TransferFunction::createDefaultTF();
            tf->deepCopy(defaultTf);
        }
        else
        {
            const auto stepsConfig = colorCfg.equal_range("step");

            for (auto itStepCfg = stepsConfig.first; itStepCfg != stepsConfig.second; ++itStepCfg)
            {
                const double value         = itStepCfg->second.get<double>("<xmlattr>.value");
                const std::string strColor = itStepCfg->second.get<std::string>("<xmlattr>.color");

                ::fwData::Color::sptr newColor = ::fwData::Color::New();
                newColor->setRGBA(strColor);

                const ::fwData::TransferFunction::TFColor color(newColor->red(), newColor->green(),
                                                                newColor->blue(), newColor->alpha());
                tf->addTFColor(value, color);
            }
            tf->setWLMinMax(tf->getMinMaxTFValues());

            if (colorCfg.count("<xmlattr>.isClamped"))
            {
                const bool isClamped = colorCfg.get<bool>("<xmlattr>.isClamped");
                tf->setIsClamped(isClamped);
            }
        }
    }
}

// ------------------------------------------------------------------------------

} // namespace parser
} // namespace dataReg

