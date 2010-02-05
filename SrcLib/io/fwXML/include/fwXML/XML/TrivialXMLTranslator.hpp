#ifndef TRIVIALXMLTRANSLATOR_HPP_
#define TRIVIALXMLTRANSLATOR_HPP_

#include <iostream>
#include <sstream>




#include "fwXML/XML/XMLTranslator.hpp"
#include "libxml/tree.h"



namespace fwXML
{



class FWXML_CLASS_API TrivialXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API TrivialXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~TrivialXMLTranslator() ;

    /// get XML from current object aka <CLASSNAME/>
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object>  obj );

    /// get Object from an XML node : in this case doesnot change anything to fwTools::Object
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*TRIVIALXMLTRANSLATOR_HPP_*/
