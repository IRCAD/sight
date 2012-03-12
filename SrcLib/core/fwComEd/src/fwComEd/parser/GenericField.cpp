/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwData/GenericField.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>

#include "fwComEd/parser/GenericField.hpp"


REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::parser::BooleanParser, ::fwData::Boolean );

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::parser::IntegerParser, ::fwData::Integer );

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::parser::FloatParser, ::fwData::Float );

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::parser::StringParser, ::fwData::String );


namespace fwComEd
{
namespace parser
{

//------------------------------------------------------------------------------

void GenericField::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("Sorry, this method is depreciated.");
}

//------------------------------------------------------------------------------

void GenericField::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwData::GenericFieldBase::sptr field = ::fwData::GenericFieldBase::dynamicCast( _obj );
    SLM_ASSERT("GenericField not instanced", field);

    SLM_ASSERT("GenericField config must contain at most one tag <value>...</value>", m_cfg->getElements().size() <= 1);
    BOOST_FOREACH( ::fwRuntime::ConfigurationElement::csptr elem, m_cfg->getElements() )
    {
        if ( elem->getName() == "value" )
        {
            std::string input = elem->getValue();
            field->fromString(input);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace fwComEd

