/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_XML_PATIENTXMLTRANSLATOR_HPP_
#define _FWXML_XML_PATIENTXMLTRANSLATOR_HPP_

#include <fwData/Patient.hpp>

#include "fwXML/XML/XMLTranslator.hpp"

namespace fwXML
{

class FWXML_CLASS_API PatientXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API PatientXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~PatientXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwData::Object::sptr obj );

    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source);
};

}

#endif /*_FWXML_XML_PATIENTXMLTRANSLATOR_HPP_*/
