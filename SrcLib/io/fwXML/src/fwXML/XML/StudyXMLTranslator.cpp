/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <fwData/Vector.hpp>

#include "fwXML/Serializer.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/boostSerializer/Study.hpp"

#include "fwXML/XML/StudyXMLTranslator.hpp"

namespace fwXML
{

StudyXMLTranslator::StudyXMLTranslator()
{}

//------------------------------------------------------------------------------

StudyXMLTranslator::~StudyXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr StudyXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Study::sptr study = ::fwData::Study::dynamicCast(obj);

    GenericXMLTranslator< ::fwData::Study > study2xmlbase;
    SLM_ASSERT("Study not instanced", study);

    xmlNodePtr masterNode = study2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("node not instanced", masterNode);

    XMLTranslatorHelper::addAttributeVector( masterNode, "acquisitions", study->getAcquisitions() );

    return masterNode;
}

//-----------------------------------------------------------------------------

void StudyXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::Study::sptr study = ::fwData::Study::dynamicCast(toUpdate);

    GenericXMLTranslator< ::fwData::Study > study2xmlbase;
    study2xmlbase.updateDataFromXML(toUpdate, source);

    study->setAcquisitions( XMLTranslatorHelper::getAttributeVector< ::fwData::Acquisition >( source, "acquisitions" ) );
}

//------------------------------------------------------------------------------

}
