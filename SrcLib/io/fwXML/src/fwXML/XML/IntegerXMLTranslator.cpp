/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Integer.hpp>

#include "fwXML/XML/IntegerXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

IntegerXMLTranslator::IntegerXMLTranslator() {};

//------------------------------------------------------------------------------

IntegerXMLTranslator::~IntegerXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr IntegerXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::Integer::sptr  integer = ::fwData::Integer::dynamicCast(obj);
    SLM_ASSERT("::fwData::Integer dynamicCast failed", integer );

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( integer );

    // append value= Property
    std::string val= ::boost::lexical_cast< std::string >( integer->value() );
    xmlNodeAddContent( node,  xmlStrdup( BAD_CAST val.c_str() ) );

    return node;
}

//------------------------------------------------------------------------------

void IntegerXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("Object must exist", toUpdate );
    ::fwData::Integer::sptr integer= ::fwData::Integer::dynamicCast(toUpdate);
    SLM_ASSERT("::fwData::Integer dynamicCast failed", integer );
    std::string litteralValue = XMLParser::getTextValue(source);
    integer->value() = ::boost::lexical_cast< int >(  litteralValue );
}

//------------------------------------------------------------------------------

}
