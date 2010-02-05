#ifndef EDGEXMLTRANSLATOR_HPP_
#define EDGEXMLTRANSLATOR_HPP_

#include <fwData/Edge.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API EdgeXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API EdgeXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~EdgeXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*EDGEXMLTRANSLATOR_HPP_*/
