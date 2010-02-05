#include "fwXML/XML/FieldXMLTranslator.hpp"
#include <fwTools/Object.hpp>

#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLParser.hpp"

namespace fwXML
{

FieldXMLTranslator::FieldXMLTranslator() {};

FieldXMLTranslator::~FieldXMLTranslator() {};






xmlNodePtr FieldXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{
    // FIXME perharps call Generic Before !!!

    // check obj is well realated to RealObject
    assert( boost::dynamic_pointer_cast< ::fwTools::Field >(obj) );
     ::boost::shared_ptr< ::fwTools::Field >  lobj= boost::dynamic_pointer_cast< ::fwTools::Field >(obj);


    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    assert( lobj->label().size() ); // Pas de label dans le field, pas de serialisation.
    // append label= Property
    xmlNewProp( node, BAD_CAST "label", xmlStrdup( BAD_CAST lobj->label().c_str() ));



    return node;

}


void FieldXMLTranslator::updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
{
    assert( toUpdate ); // object should exist
    //get its label
    ::fwTools::Field *lobj=dynamic_cast< ::fwTools::Field *>(toUpdate.get());
    assert( lobj );
        assert( XMLParser::getAttribute(source,"label").size() );
    lobj->label() = XMLParser::getAttribute(source,"label");
}


}
