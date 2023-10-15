/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "app/parser/transfer_function.hpp"

#include <data/color.hpp>
#include <data/transfer_function.hpp>

namespace sight::app::parser
{

// ------------------------------------------------------------------------------

void transfer_function::updating()
{
    SIGHT_FATAL("This method is deprecated, and this shouldn't be used.");
}

// ------------------------------------------------------------------------------

void transfer_function::createConfig(core::tools::object::sptr _obj)
{
    data::transfer_function::sptr tf = std::dynamic_pointer_cast<data::transfer_function>(_obj);
    SIGHT_ASSERT("transfer_function not instanced", tf);

    if(const auto colorCfg = m_cfg.get_child_optional("colors"); colorCfg.has_value())
    {
        const bool isDefault = colorCfg->get("<xmlattr>.default", false);
        if(isDefault)
        {
            data::transfer_function::sptr defaultTf = data::transfer_function::createDefaultTF();
            tf->deep_copy(defaultTf);
        }
        else
        {
            const auto stepsConfig = colorCfg->equal_range("step");

            auto tfData            = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
            const std::string name = m_cfg.get<std::string>("name", "");
            if(!name.empty())
            {
                tf->setName(name);
            }

            for(auto itStepCfg = stepsConfig.first ; itStepCfg != stepsConfig.second ; ++itStepCfg)
            {
                const auto value    = itStepCfg->second.get<double>("<xmlattr>.value");
                const auto strColor = itStepCfg->second.get<std::string>("<xmlattr>.color");

                data::color::sptr newColor = std::make_shared<data::color>();
                newColor->setRGBA(strColor);

                const data::transfer_function::color_t color(newColor->red(), newColor->green(),
                                                             newColor->blue(), newColor->alpha());
                (*tfData)[value] = color;
            }

            tfData->setWindowMinMax(tfData->minMax());

            const bool isClamped = colorCfg->get<bool>("<xmlattr>.isClamped", true);
            tfData->setClamped(isClamped);

            tf->fitWindow();
        }
    }
    else
    {
        data::transfer_function::sptr defaultTf = data::transfer_function::createDefaultTF();
        tf->deep_copy(defaultTf);
    }
}

// ------------------------------------------------------------------------------

} // namespace sight::app::parser
