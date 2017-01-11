/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

void GenericField::updating( ) throw(fwTools::Failed)
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

