#ifndef COMPOSITEOBJECTXMLTRANSLATOR_HPP_
#define COMPOSITEOBJECTXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/Composite.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API CompositeXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API CompositeXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~CompositeXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*COMPOSITEOBJECTXMLTRANSLATOR_HPP_*/
