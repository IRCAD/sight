/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWXML_XML_STRUCTURETRAITSDICTIONARYXMLTRANSLATOR_HPP__
#define __FWXML_XML_STRUCTURETRAITSDICTIONARYXMLTRANSLATOR_HPP__

#include <fwData/Object.hpp>
#include <fwData/StructureTraitsDictionary.hpp>

#include "fwXML/XML/XMLTranslator.hpp"

namespace fwXML
{

class FWXML_CLASS_API StructureTraitsDictionaryXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API StructureTraitsDictionaryXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~StructureTraitsDictionaryXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwData::Object::sptr obj );

    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source);

};

}

#endif // __FWXML_XML_STRUCTURETRAITSDICTIONARYXMLTRANSLATOR_HPP__
