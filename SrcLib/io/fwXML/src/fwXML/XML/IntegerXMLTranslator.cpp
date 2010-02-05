#include "fwXML/XML/IntegerXMLTranslator.hpp"
#include <fwData/Integer.hpp>

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
namespace fwXML
{

IntegerXMLTranslator::IntegerXMLTranslator() {};

IntegerXMLTranslator::~IntegerXMLTranslator() {};






xmlNodePtr IntegerXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{

    ::boost::shared_ptr< ::fwData::Integer >  integer= boost::dynamic_pointer_cast< ::fwData::Integer >(obj);
    assert( integer );

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );;

    // append value= Property
    std::string val= boost::lexical_cast< std::string >( integer->value() );
    xmlNodeAddContent( node,  xmlStrdup( BAD_CAST val.c_str() ) );

    return node;

}


void IntegerXMLTranslator::updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
{
    assert( toUpdate ); // object should exist
    //get its label
    ::fwData::Integer *integer=dynamic_cast< ::fwData::Integer *>(toUpdate.get());
    assert( integer );
    std::string litteralValue = XMLParser::getTextValue(source);
    integer->value() = boost::lexical_cast< int >(  litteralValue );
}


}
