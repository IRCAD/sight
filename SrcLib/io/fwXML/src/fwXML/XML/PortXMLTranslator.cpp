/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwData/Port.hpp>

#include "fwXML/XML/PortXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

namespace fwXML
{

PortXMLTranslator::PortXMLTranslator() {};

//------------------------------------------------------------------------------

PortXMLTranslator::~PortXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr PortXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Port::sptr port = ::fwData::Port::dynamicCast(obj);
    SLM_ASSERT("port not instanced", port);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    XMLTH::addProp(node, "identifier" , port->getIdentifier() );
    XMLTH::addProp(node, "type" , port->getType() );

    return node;
}

//------------------------------------------------------------------------------

void PortXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::Port >(toUpdate,source) );

    ::fwData::Port::sptr port = ::fwData::Port::dynamicCast(toUpdate);

    port->setIdentifier( XMLTH::getProp<std::string>(source,"identifier") );
    port->setType( XMLTH::getProp<std::string >(source,"type") );
}

//------------------------------------------------------------------------------

}
