/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_DICTIONARYXMLTRANSLATOR_HPP_
#define _FWXML_DICTIONARYXMLTRANSLATOR_HPP_


#include "fwXML/XML/XMLTranslator.hpp"


namespace fwXML
{

class FWXML_CLASS_API DictionaryOrganXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API DictionaryOrganXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~DictionaryOrganXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwData::Object::sptr obj );


    /// get Object from an XML Graph
    FWXML_API void updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*_FWXML_DICTIONARYXMLTRANSLATOR_HPP_*/
