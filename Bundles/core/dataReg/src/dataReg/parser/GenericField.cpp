/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "dataReg/parser/GenericField.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/GenericField.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>

#include <fwServices/macros.hpp>


fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::BooleanParser, ::fwData::Boolean );

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::IntegerParser, ::fwData::Integer );

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::FloatParser, ::fwData::Float );

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::StringParser, ::fwData::String );


namespace dataReg
{
namespace parser
{

//------------------------------------------------------------------------------

void GenericField::updating( )
{
    SLM_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void GenericField::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwData::GenericFieldBase::sptr field = ::fwData::GenericFieldBase::dynamicCast( _obj );
    SLM_ASSERT("GenericField not instanced", field);

    ::fwRuntime::ConfigurationElementContainer configs = m_cfg->findAllConfigurationElement("value");
    SLM_ASSERT("GenericField config must contain at most one tag <value>...</value>", configs.size() <= 1);

    if ( configs.size() >= 1 )
    {
        ::fwRuntime::ConfigurationElement::sptr config = *configs.begin();
        std::string input = config->getValue();
        field->fromString(input);
    }
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace dataReg

