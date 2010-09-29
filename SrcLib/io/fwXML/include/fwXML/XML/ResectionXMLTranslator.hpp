/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _fwXML_XML_RESECTIONXMLTRANSLATOR_HPP_
#define _fwXML_XML_RESECTIONXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/Resection.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API ResectionXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API ResectionXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~ResectionXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif // _fwXML_XML_RESECTIONXMLTRANSLATOR_HPP_
