/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWXML_FOLDERXMLTRANSLATOR_HPP__
#define __FWXML_FOLDERXMLTRANSLATOR_HPP__

#include <fwData/location/Folder.hpp>

#include "fwXML/XML/XMLTranslator.hpp"

namespace fwXML
{

class FWXML_CLASS_API FolderXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API FolderXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~FolderXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );

    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate, xmlNodePtr source);

};

}

#endif /*__FWXML_FOLDERXMLTRANSLATOR_HPP__*/
