#ifndef STRINGOBJECTXMLTRANSLATOR_HPP_
#define STRINGOBJECTXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/String.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API StringXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API StringXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~StringXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*STRINGOBJECTXMLTRANSLATOR_HPP_*/
