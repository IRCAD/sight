/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_XML_ARRAYXMLTRANSLATOR_HPP_
#define _FWXML_XML_ARRAYXMLTRANSLATOR_HPP_

#include <fwData/Array.hpp>

#include "fwXML/XML/XMLTranslator.hpp"

namespace fwXML
{

class FWXML_CLASS_API ArrayXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API ArrayXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~ArrayXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

    /// manage extra serialization : read array from path extracted from filename attribute in <Buffer> xml node
    FWXML_API void manageLoadingBuffer( xmlNodePtr boostXMLBuffer, ::fwData::Array::sptr );

    /// manage extra serialization : only inform <Buffer filename attribute />
    FWXML_API void manageSavingBuffer( xmlNodePtr boostXMLBuffer, ::fwData::Array::sptr );

    static std::string S_NOFILEPROTOCOL;
};

}

#endif /*_FWXML_XML_ARRAYXMLTRANSLATOR_HPP_*/
