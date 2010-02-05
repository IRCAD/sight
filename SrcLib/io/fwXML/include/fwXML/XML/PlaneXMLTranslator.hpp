#ifndef _fwXML_XML_PLANEXMLTRANSLATOR_HPP_
#define _fwXML_XML_PLANEXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/Plane.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API PlaneXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API PlaneXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~PlaneXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif // _fwXML_XML_PLANEXMLTRANSLATOR_HPP_
