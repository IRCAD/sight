/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "dataReg/parser/TransferFunction.hpp"

#include <fwData/Color.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwRuntime/Convert.hpp>

#include <fwServices/macros.hpp>

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
