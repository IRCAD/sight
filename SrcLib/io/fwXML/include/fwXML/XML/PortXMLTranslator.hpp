#ifndef PORTXMLTRANSLATOR_HPP_
#define PORTXMLTRANSLATOR_HPP_

#include <fwData/Port.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API PortXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API PortXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~PortXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*PORTXMLTRANSLATOR_HPP_*/
