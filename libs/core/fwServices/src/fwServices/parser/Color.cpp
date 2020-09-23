/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "fwServices/parser/Color.hpp"

#include "fwServices/macros.hpp"

#include <fwData/Color.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

fwServicesRegisterMacro(::fwServices::IXMLParser, ::dataReg::parser::Color)
fwServicesRegisterObjectMacro(::dataReg::parser::Color, ::fwData::Color)

namespace dataReg
{
namespace parser
{

static const std::string s_VALUE_CONFIG = "value";

//------------------------------------------------------------------------------

void Color::createConfig(::fwTools::Object::sptr _obj)
{
    const ::fwData::Color::sptr color = ::fwData::Color::dynamicCast(_obj);
    SLM_ASSERT("Color does not exist.", color);

    ::fwRuntime::ConfigurationElementContainer configs = m_cfg->findAllConfigurationElement(s_VALUE_CONFIG);
    SLM_ASSERT("Color config must contain at most one tag <value>...</value>", configs.size() <= 1);

    if(configs.size() == 1)
    {
        const ::fwRuntime::ConfigurationElement::csptr config = *configs.begin();
        const std::string hexaColor                           = config->getValue();
        OSLM_ASSERT(
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
} // namespace dataReg.
