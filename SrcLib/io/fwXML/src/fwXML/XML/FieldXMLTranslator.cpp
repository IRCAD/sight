/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include "fwXML/XML/FieldXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLParser.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

FieldXMLTranslator::FieldXMLTranslator() {};

//------------------------------------------------------------------------------

FieldXMLTranslator::~FieldXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr FieldXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    // FIXME perharps call Generic Before !!!

    ::fwTools::Field::sptr  lobj= ::fwTools::Field::dynamicCast(obj);
    // check obj is well related to RealObject
    SLM_ASSERT("::fwTools::Field dynamicCast failed", lobj );

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    SLM_ASSERT("No label in field", lobj->label().size() );
    // append label= Property
    xmlNewProp( node, BAD_CAST "label", xmlStrdup( BAD_CAST lobj->label().c_str() ));

    return node;
}

//------------------------------------------------------------------------------

void FieldXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate, xmlNodePtr source)
{
    SLM_ASSERT("Object must exist", toUpdate );
    //get its label
    ::fwTools::Field::sptr lobj= ::fwTools::Field::dynamicCast(toUpdate);
    SLM_ASSERT("::fwTools::Field dynamicCast failed", lobj );
    assert( XMLParser::getAttribute(source, "label").size() );
    lobj->label() = XMLParser::getAttribute(source,"label");
}

//------------------------------------------------------------------------------

}
