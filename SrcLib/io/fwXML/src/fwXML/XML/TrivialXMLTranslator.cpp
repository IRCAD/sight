#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "libxml/tree.h"
#include <fwData/Object.hpp>


namespace fwXML
{

TrivialXMLTranslator::TrivialXMLTranslator() {};

TrivialXMLTranslator::~TrivialXMLTranslator() {};







xmlNodePtr TrivialXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    xmlAddChild( node, xmlNewComment( BAD_CAST "Trivial XML Translator" ) );
    return node;
}


void TrivialXMLTranslator::updateDataFromXML(::boost::shared_ptr<fwTools::Object>, xmlNode*)
{
    return;
}


}
