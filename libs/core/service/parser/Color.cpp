/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "service/parser/Color.hpp"

#include "service/macros.hpp"

#include <core/runtime/ConfigurationElement.hpp>

#include <data/Color.hpp>

namespace sight::service
{
namespace parser
{

static const std::string s_VALUE_CONFIG = "value";

//------------------------------------------------------------------------------

void Color::createConfig(core::tools::Object::sptr _obj)
{
    const data::Color::sptr color = data::Color::dynamicCast(_obj);
    SLM_ASSERT("Color does not exist.", color);

    core::runtime::ConfigurationElementContainer configs = m_cfg->findAllConfigurationElement(s_VALUE_CONFIG);
    SLM_ASSERT("Color config must contain at most one tag <value>...</value>", configs.size() <= 1);

    if(configs.size() == 1)
    {
        const core::runtime::ConfigurationElement::csptr config = *configs.begin();
        const std::string hexaColor                             = config->getValue();
        SLM_ASSERT(
            "Color string should start with '#' and followed by 6 or 8 "
            "hexadecimal digits. Given color: " << hexaColor,
                hexaColor[0] == '#'
                && ( hexaColor.length() == 7 || hexaColor.length() == 9)
            );
        color->setRGBA(hexaColor);
    }
}

//------------------------------------------------------------------------------

} // namespace parser.
} // namespace sight::service.
