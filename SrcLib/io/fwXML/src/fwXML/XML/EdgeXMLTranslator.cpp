/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwData/Port.hpp>

#include "fwXML/XML/EdgeXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLParser.hpp"

namespace fwXML
{

EdgeXMLTranslator::EdgeXMLTranslator() {};

//------------------------------------------------------------------------------

EdgeXMLTranslator::~EdgeXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr EdgeXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::Edge::sptr edge = ::fwData::Edge::dynamicCast(obj);
    assert(edge);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    // info
    XMLTH::addProp( node, "fromPort", edge->getIdentifiers().first );
    XMLTH::addProp( node, "toPort",   edge->getIdentifiers().second );
    XMLTH::addProp( node, "nature",   edge->getNature() );

    return node;
}

//------------------------------------------------------------------------------

void EdgeXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::Edge >(toUpdate,source) );

    ::fwData::Edge::sptr edge = ::fwData::Edge::dynamicCast(toUpdate);

    std::string fromPort =  XMLTH::getProp<std::string>(source,"fromPort") ;
    std::string toPort   =  XMLTH::getProp<std::string>(source,"toPort") ;
    edge->setIdentifiers( fromPort , toPort );
    edge->setNature( XMLTH::getProp<std::string >(source,"nature") );
}

//------------------------------------------------------------------------------

}
