/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>

#include "fwComEd/fieldHelper/BackupHelper.hpp"
#include "fwComEd/Dictionary.hpp"
#include "fwComEd/ResectionDBMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ResectionDBMsg, ::fwComEd::ResectionDBMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ResectionDBMsg::NEW_RESECTIONDB_SELECTED = "NEW_RESECTIONDB_SELECTED";
std::string ResectionDBMsg::RESECTIONDB_SELECTED     = "RESECTIONDB_SELECTED";
std::string ResectionDBMsg::NEW_RESECTION_SELECTED   = "NEW_RESECTION_SELECTED";
std::string ResectionDBMsg::NEW_SAFE_PART_SELECTED   = "NEW_SAFE_PART_SELECTED";
std::string ResectionDBMsg::ADD_RESECTION            = "ADD_RESECTION";
std::string ResectionDBMsg::ADD_SAFE_PART            = "ADD_SAFE_PART";
std::string ResectionDBMsg::MODIFIED                 = "MODIFIED";
std::string ResectionDBMsg::RESECTIONDB_INVALIDATED  = "RESECTIONDB_INVALIDATED";

//-----------------------------------------------------------------------------

ResectionDBMsg::ResectionDBMsg() throw()
{}

//-----------------------------------------------------------------------------

ResectionDBMsg::~ResectionDBMsg() throw()
{}

//-----------------------------------------------------------------------------

::fwData::Resection::sptr ResectionDBMsg::getResectionSelected() const
{
    ::fwData::PatientDB::sptr pPatientDB = ::fwData::PatientDB::dynamicCast(this->getSubject().lock());
    SLM_ASSERT("PatientDB Object needed", pPatientDB);

    ::fwData::Patient::sptr pPatient         = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);
    ::fwTools::Field::sptr pResectionInfo    = pPatient->getField( fwComEd::Dictionary::m_resectionSelectedId );
    ::fwData::String::sptr pOpName           = ::fwData::String::dynamicCast ( pResectionInfo->children().at(0) );
    ::fwData::Integer::sptr pResectionIndex  = ::fwData::Integer::dynamicCast( pResectionInfo->children().at(1) );
    ::fwData::Composite::sptr compoResection = ::fwData::Composite::dynamicCast(pPatient->getTool(::fwComEd::Dictionary::m_resectionId));
    ::fwData::ResectionDB::sptr pResectionDB = ::fwData::ResectionDB::dynamicCast(compoResection->getRefMap()[pOpName->value()]);
    ::fwData::Resection::sptr pResection     = pResectionDB->getResections().at(pResectionIndex->value());

    return pResection;
}
//-----------------------------------------------------------------------------

std::string ResectionDBMsg::getResectionOpName() const
{
    ::fwData::PatientDB::sptr pPatientDB = ::fwData::PatientDB::dynamicCast(this->getSubject().lock());
    SLM_ASSERT("PatientDB Object needed", pPatientDB);

    ::fwData::Patient::sptr pPatient = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);
    ::fwTools::Field::sptr pResectionInfo = pPatient->getField( fwComEd::Dictionary::m_resectionSelectedId );
    ::fwData::String::sptr pOpName = ::fwData::String::dynamicCast ( pResectionInfo->children().at(0) );


    return pOpName->value();
}

//-----------------------------------------------------------------------------

::fwData::Resection::sptr  ResectionDBMsg::getSafePartSelected() const
{
    ::fwData::ResectionDB::sptr pResectionDB = this->getResectionDBSelected();
    return pResectionDB->getRefSafeResection();
}

//-----------------------------------------------------------------------------

::fwData::ResectionDB::sptr  ResectionDBMsg::getResectionDBSelected() const
{
    ::fwData::PatientDB::sptr pPatientDB = ::fwData::PatientDB::dynamicCast(this->getSubject().lock());
    SLM_ASSERT("PatientDB Object needed", pPatientDB);

    ::fwData::Patient::sptr pPatient = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);
    ::fwTools::Field::sptr pResectionInfo = pPatient->getField( fwComEd::Dictionary::m_resectionSelectedId );
    ::fwData::String::sptr pOpName = ::fwData::String::dynamicCast ( pResectionInfo->children().at(0) );
    ::fwData::Composite::sptr compoResection = ::fwData::Composite::dynamicCast(pPatient->getTool(::fwComEd::Dictionary::m_resectionId));
    ::fwData::ResectionDB::sptr pResectionDB = ::fwData::ResectionDB::dynamicCast(compoResection->getRefMap()[pOpName->value()]);

    return pResectionDB;
}


} // namespace fwComEd

