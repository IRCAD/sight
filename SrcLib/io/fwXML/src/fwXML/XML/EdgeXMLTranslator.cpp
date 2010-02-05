/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>
#include "fwXML/XML/EdgeXMLTranslator.hpp"
#include <fwData/Port.hpp>

#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLParser.hpp"

namespace fwXML
{

EdgeXMLTranslator::EdgeXMLTranslator() {};

EdgeXMLTranslator::~EdgeXMLTranslator() {};






xmlNodePtr EdgeXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    ::boost::shared_ptr< ::fwData::Edge> edge = boost::dynamic_pointer_cast< ::fwData::Edge>(obj);
    assert(edge);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    // info
    XMLTH::addProp( node, "fromPort", edge->getIdentifiers().first );
    XMLTH::addProp( node, "toPort",   edge->getIdentifiers().second );
    XMLTH::addProp( node, "nature",   edge->getNature() );

    return node;

}


void EdgeXMLTranslator::updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::Edge >(toUpdate,source) );

    ::boost::shared_ptr< ::fwData::Edge> edge = boost::dynamic_pointer_cast< ::fwData::Edge>(toUpdate);

    edge->setIdentifiers( XMLTH::getProp<std::string>(source,"fromPort"), XMLTH::getProp<std::string>(source,"toPort") );
    edge->setNature( XMLTH::getProp<std::string >(source,"nature") );

}


}
