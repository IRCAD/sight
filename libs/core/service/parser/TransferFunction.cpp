/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "service/parser/TransferFunction.hpp"

#include "service/macros.hpp"

#include <core/runtime/Convert.hpp>

#include <data/Color.hpp>
#include <data/TransferFunction.hpp>

namespace sight::service
{

namespace parser
{

// ------------------------------------------------------------------------------

void TransferFunction::updating()
{
    SIGHT_FATAL("This method is deprecated, and this shouldn't be used.");
}

// ------------------------------------------------------------------------------

void TransferFunction::createConfig(core::tools::Object::sptr _obj)
{
    data::TransferFunction::sptr tf = data::TransferFunction::dynamicCast(_obj);
    SIGHT_ASSERT("TransferFunction not instanced", tf);

    const ConfigType config = core::runtime::Convert::toPropertyTree(m_cfg).get_child("object");
    if(config.count("colors"))
    {
        const ConfigType colorCfg = config.get_child("colors");

        const bool isDefault = colorCfg.get("<xmlattr>.default", false);
        if(isDefault)
        {
            data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF();
            tf->deepCopy(defaultTf);
        }
        else
        {
            const auto stepsConfig = colorCfg.equal_range("step");

            auto tfData            = tf->pieces().emplace_back(data::TransferFunctionPiece::New());
            const std::string name = config.get<std::string>("name", "");
            if(!name.empty())
            {
                tf->setName(name);
            }

            for(auto itStepCfg = stepsConfig.first ; itStepCfg != stepsConfig.second ; ++itStepCfg)
            {
                const double value         = itStepCfg->second.get<double>("<xmlattr>.value");
                const std::string strColor = itStepCfg->second.get<std::string>("<xmlattr>.color");

                data::Color::sptr newColor = data::Color::New();
                newColor->setRGBA(strColor);

                const data::TransferFunction::color_t color(newColor->red(), newColor->green(),
                                                            newColor->blue(), newColor->alpha());
                (*tfData)[value] = color;
            }

            tfData->setWindowMinMax(tfData->minMax());

            const bool isClamped = colorCfg.get<bool>("<xmlattr>.isClamped", true);
            tfData->setClamped(isClamped);
        }
    }
    else
    {
        data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF();
        tf->deepCopy(defaultTf);
    }
}

// ------------------------------------------------------------------------------

} // namespace parser

} // namespace sight::service
