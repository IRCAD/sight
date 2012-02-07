/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*EDGEXMLTRANSLATOR_HPP_*/
