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
#include "fwXML/boostSerializer/PatientDB.hpp"

#include "fwXML/XML/PatientDBXMLTranslator.hpp"

namespace fwXML
{

PatientDBXMLTranslator::PatientDBXMLTranslator()
{}

//------------------------------------------------------------------------------

PatientDBXMLTranslator::~PatientDBXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr PatientDBXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::dynamicCast(obj);

    GenericXMLTranslator< ::fwData::PatientDB > patientDB2xmlbase;
    SLM_ASSERT("PatientDB not instanced", patientDB);

    xmlNodePtr masterNode = patientDB2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("Node not instanced", masterNode);

    XMLTranslatorHelper::addAttributeVector( masterNode, "patients", patientDB->getPatients() );

    return masterNode;
}

//-----------------------------------------------------------------------------

void PatientDBXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::dynamicCast(toUpdate);

    GenericXMLTranslator< ::fwData::PatientDB > patientDB2xmlbase;
    patientDB2xmlbase.updateDataFromXML(toUpdate, source);

    patientDB->setPatients(XMLTranslatorHelper::getAttributeVector< ::fwData::Patient >( source, "patients" ) );
}

//------------------------------------------------------------------------------

}
