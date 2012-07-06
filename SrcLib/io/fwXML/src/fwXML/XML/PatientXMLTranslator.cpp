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
#include "fwXML/boostSerializer/Patient.hpp"

#include "fwXML/XML/PatientXMLTranslator.hpp"

namespace fwXML
{

PatientXMLTranslator::PatientXMLTranslator()
{}

//------------------------------------------------------------------------------

PatientXMLTranslator::~PatientXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr PatientXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Patient::sptr patient = ::fwData::Patient::dynamicCast(obj);

    GenericXMLTranslator< ::fwData::Patient > patient2xmlbase;
    SLM_ASSERT("Patient not instanced", patient);

    xmlNodePtr masterNode = patient2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("Node not instanced", masterNode);

    XMLTranslatorHelper::addAttributeVector( masterNode, "studies", patient->getStudies() );
    XMLTranslatorHelper::addAttribute( masterNode, "toolBox", patient->getToolBox(), false );
    XMLTranslatorHelper::addAttribute( masterNode, "scenarios", patient->getScenarios(), false );

    return masterNode;
}

//-----------------------------------------------------------------------------

void PatientXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::Patient::sptr patient = ::fwData::Patient::dynamicCast(toUpdate);

    GenericXMLTranslator< ::fwData::Patient > patient2xmlbase;
    patient2xmlbase.updateDataFromXML(toUpdate, source);

    patient->setStudies(XMLTranslatorHelper::getAttributeVector< ::fwData::Study >( source, "studies" ) );
    patient->setToolBox(XMLTranslatorHelper::getAttribute< ::fwData::Composite >( source, "toolBox", false ) );
    patient->setScenarios(XMLTranslatorHelper::getAttribute< ::fwData::Composite >( source, "scenarios", false) );
}

//------------------------------------------------------------------------------

}
