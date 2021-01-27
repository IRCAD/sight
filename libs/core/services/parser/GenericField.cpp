/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "services/parser/GenericField.hpp"

#include "services/macros.hpp"

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/GenericField.hpp>
#include <data/Integer.hpp>
#include <data/String.hpp>

fwServicesRegisterMacro( ::sight::services::IXMLParser, ::sight::services::parser::BooleanParser,
                         ::sight::data::Boolean )

fwServicesRegisterMacro( ::sight::services::IXMLParser, ::sight::services::parser::IntegerParser,
                         ::sight::data::Integer )

fwServicesRegisterMacro( ::sight::services::IXMLParser, ::sight::services::parser::FloatParser, ::sight::data::Float )

fwServicesRegisterMacro( ::sight::services::IXMLParser, ::sight::services::parser::StringParser, ::sight::data::String )

namespace sight::services
{
namespace parser
{

//------------------------------------------------------------------------------

void GenericField::updating( )
{
    SLM_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void GenericField::createConfig( core::tools::Object::sptr _obj )
{
    data::GenericFieldBase::sptr field = data::GenericFieldBase::dynamicCast( _obj );
    SLM_ASSERT("GenericField not instanced", field);

    core::runtime::ConfigurationElementContainer configs = m_cfg->findAllConfigurationElement("value");
    SLM_ASSERT("GenericField config must contain at most one tag <value>...</value>", configs.size() <= 1);

    if ( configs.size() >= 1 )
    {
        core::runtime::ConfigurationElement::sptr config = *configs.begin();
        std::string input                                = config->getValue();
        field->fromString(input);
    }
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace sight::services
