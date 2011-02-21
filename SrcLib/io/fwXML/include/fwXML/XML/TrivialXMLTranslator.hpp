/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef TRIVIALXMLTRANSLATOR_HPP_
#define TRIVIALXMLTRANSLATOR_HPP_

#include <iostream>
#include <sstream>
#include <libxml/tree.h>

#include "fwXML/XML/XMLTranslator.hpp"

namespace fwXML
{

class FWXML_CLASS_API TrivialXMLTranslator : public XMLTranslator
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((TrivialXMLTranslator)(::fwXML::XMLTranslator), (()), new TrivialXMLTranslator);

    /// trivial constructor
    FWXML_API TrivialXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~TrivialXMLTranslator() ;

    /// get XML from current object aka <CLASSNAME/>
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );

    /// get Object from an XML node : in this case doesnot change anything to fwTools::Object
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*TRIVIALXMLTRANSLATOR_HPP_*/
