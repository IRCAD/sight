/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWXML_XML_ROITRAITSXMLTRANSLATOR_HPP__
#define __FWXML_XML_ROITRAITSXMLTRANSLATOR_HPP__

#include <fwData/Object.hpp>
#include <fwData/ROITraits.hpp>

#include "fwXML/XML/XMLTranslator.hpp"

namespace fwXML
{

class FWXML_CLASS_API ROITraitsXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API ROITraitsXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~ROITraitsXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwData::Object::sptr obj );

    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source);

};

}

#endif // __FWXML_XML_ROITRAITSXMLTRANSLATOR_HPP__
