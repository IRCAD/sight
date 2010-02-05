#ifndef _fwXML_XML_PLANELISTXMLTRANSLATOR_HPP_
#define _fwXML_XML_PLANELISTXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/PlaneList.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API PlaneListXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API PlaneListXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~PlaneListXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif // _fwXML_XML_PLANELISTXMLTRANSLATOR_HPP_
