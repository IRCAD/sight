#include <boost/lexical_cast.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>
#include "fwXML/XML/PortXMLTranslator.hpp"
#include <fwData/Port.hpp>


#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

namespace fwXML
{

PortXMLTranslator::PortXMLTranslator() {};

PortXMLTranslator::~PortXMLTranslator() {};






xmlNodePtr PortXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    ::boost::shared_ptr< ::fwData::Port> port = boost::dynamic_pointer_cast< ::fwData::Port>(obj);
    assert(port);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    XMLTH::addProp(node, "identifier" , port->getIdentifier() );
    XMLTH::addProp(node, "type" , port->getType() );

    return node;

}


void PortXMLTranslator::updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
{
    assert( XMLTH::check< ::fwData::Port >(toUpdate,source) );

    ::boost::shared_ptr< ::fwData::Port> port = boost::dynamic_pointer_cast< ::fwData::Port>(toUpdate);

    port->setIdentifier( XMLTH::getProp<std::string>(source,"identifier") );
    port->setType( XMLTH::getProp<std::string >(source,"type") );

}


}
