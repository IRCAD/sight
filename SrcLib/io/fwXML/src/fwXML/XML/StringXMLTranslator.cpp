#include "fwXML/XML/StringXMLTranslator.hpp"
#include <fwData/String.hpp>

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwCore/base.hpp"

namespace fwXML
{

StringXMLTranslator::StringXMLTranslator() {};

StringXMLTranslator::~StringXMLTranslator() {};






xmlNodePtr StringXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{

    ::boost::shared_ptr< ::fwData::String >  integer= boost::dynamic_pointer_cast< ::fwData::String >(obj);
    assert( integer );

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    // append value= Property
    std::string val= integer->value() ;
    xmlNodeAddContent( node,  xmlStrdup( BAD_CAST val.c_str() ) );

    return node;

}


void StringXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( toUpdate ); // object should exist
    //get its label
    ::fwData::String *str=dynamic_cast< ::fwData::String *>(toUpdate.get());
    assert( str );
    str->value() =  (const char*)xmlNodeGetContent(source) ;
}


}
