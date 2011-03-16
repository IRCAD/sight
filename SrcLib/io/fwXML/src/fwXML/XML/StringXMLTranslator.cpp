/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/String.hpp>

#include "fwXML/XML/StringXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"


namespace fwXML
{

//------------------------------------------------------------------------------

StringXMLTranslator::StringXMLTranslator()
{}

//------------------------------------------------------------------------------

StringXMLTranslator::~StringXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr StringXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::String::sptr  str = ::fwData::String::dynamicCast(obj);
    SLM_ASSERT("::fwData::String dynamicCast failed", str );

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( str );

    // append value= Property
    std::string val= str->value() ;
    xmlNodeAddContent( node,  xmlStrdup( BAD_CAST val.c_str() ) );

    return node;
}

//------------------------------------------------------------------------------

void StringXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("Object must exist", toUpdate );
    ::fwData::String::sptr str= ::fwData::String::dynamicCast(toUpdate);
    SLM_ASSERT("::fwData::String dynamicCast failed", str );
    str->value() = XMLParser::getTextValue(source);;
}

//------------------------------------------------------------------------------

}
