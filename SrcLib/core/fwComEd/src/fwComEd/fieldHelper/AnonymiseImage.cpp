/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IService.hpp>

#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Study.hpp>
#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>

#include "fwComEd/PatientDBMsg.hpp"
#include "fwComEd/Dictionary.hpp"
#include "fwComEd/fieldHelper/AnonymiseImage.hpp"

namespace fwComEd
{

namespace fieldHelper
{

//------------------------------------------------------------------------------

void AnonymiseImage::anonymisePatient( ::fwData::Patient::sptr _pPatient)
{
    _pPatient->setCRefName("anonymous");
    _pPatient->setCRefFirstname("anonymous");
    _pPatient->setCRefBirthdate( ::boost::date_time::min_date_time );
    _pPatient->setCRefIDDicom("");

    ::fwData::Patient::StudyIterator studiesIter = _pPatient->getStudies().first;
    for ( ; studiesIter != _pPatient->getStudies().second ; ++studiesIter)
    {
        AnonymiseImage::anonymiseStudy(*studiesIter);
    }
}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymisePatientDB( ::fwData::PatientDB::sptr _pPatientDB)
{
    ::fwData::PatientDB::PatientIterator patientIter = _pPatientDB->getPatients().first;
    for ( ; patientIter != _pPatientDB->getPatients().second ; ++patientIter)
    {
        AnonymiseImage::anonymisePatient(*patientIter);
    }
}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymiseImage( ::fwData::Image::sptr _pImage)
{}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymiseStudy( ::fwData::Study::sptr _pStudy)
{
    _pStudy->setCRefHospital("anonymous");
    _pStudy->setCRefRISId("");
    _pStudy->setCRefUID("");

    ::fwData::Study::AcquisitionIterator acquisitionIter = _pStudy->getAcquisitions().first;
    for ( ; acquisitionIter != _pStudy->getAcquisitions().second ; ++acquisitionIter)
    {
        AnonymiseImage::anonymiseAcquisition(*acquisitionIter);
    }
}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymiseAcquisition( ::fwData::Acquisition::sptr _pAcquisition)
{
    _pAcquisition->setCRefCreationDate( ::boost::date_time::min_date_time );
    _pAcquisition->setCRefUID("");
}

//------------------------------------------------------------------------------

::fwData::Patient::sptr AnonymiseImage::createAnonymisedPatient( ::fwData::Patient::sptr _pPatient)
{
    ::fwData::Patient::NewSptr pNewPatient;
    //(* pNewPatient) = (* _pPatient);
    pNewPatient->shallowCopy( _pPatient );
    pNewPatient->setField( ::fwData::Patient::ID_STUDIES );

    std::map< ::fwData::Acquisition::sptr, ::fwData::Acquisition::sptr > acquisitionMap;

    ::fwData::Patient::StudyIterator studiesIter = _pPatient->getStudies().first;
    for ( ; studiesIter != _pPatient->getStudies().second ; ++studiesIter)
    {
        ::fwData::Study::sptr pStudy = (*studiesIter);
        ::fwData::Study::NewSptr pNewStudy;
        pNewPatient->addStudy(pNewStudy);

        //(* pNewStudy) = (*pStudy);
        pNewStudy->shallowCopy( pStudy );
        pNewStudy->setField( ::fwData::Study::ID_ACQUISITIONS );

        ::fwData::Study::AcquisitionIterator acquisitionIter = pStudy->getAcquisitions().first;
        for ( ; acquisitionIter != pStudy->getAcquisitions().second ; ++acquisitionIter)
        {
            ::fwData::Acquisition::sptr pAcquisition = (*acquisitionIter);
            ::fwData::Acquisition::NewSptr pNewAcquisition;
            pNewStudy->addAcquisition(pNewAcquisition);

            //(* pNewAcquisition) = (*pAcquisition);
            pNewAcquisition->shallowCopy(pAcquisition);

            acquisitionMap[pAcquisition] = pNewAcquisition;
        }
    }

    ::fwData::Composite::sptr ctool = ::fwData::Composite::dynamicCast(_pPatient->getTool(::fwComEd::Dictionary::m_resectionId));
    if (ctool )
    {
        pNewPatient->addTool(::fwComEd::Dictionary::m_resectionId, ctool );
    }

    ::fwData::Composite::sptr cscenario = ::fwData::Composite::dynamicCast( _pPatient->getScenario("ResectionAcquisitionPair"));
    if ( cscenario )
    {
        ::fwData::Composite::NewSptr compoScenario;
        ::fwData::Composite::Container::iterator cscenarioIter;
        for (  cscenarioIter = cscenario->getRefMap().begin() ;  cscenarioIter != cscenario->getRefMap().end();  ++cscenarioIter )
        {
            std::string opName = cscenarioIter->first;
            ::fwData::Acquisition::sptr initialAcq = ::fwData::Acquisition::dynamicCast(cscenario->getRefMap()[opName]);
            compoScenario->getRefMap()[opName] = acquisitionMap[initialAcq];
        }
        pNewPatient->addScenario("ResectionAcquisitionPair", compoScenario );
    }

    AnonymiseImage::anonymisePatient(pNewPatient);

    return pNewPatient;
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr AnonymiseImage::createAnonymisedPatientDB( ::fwData::PatientDB::sptr _pPatientDB)
{
    ::fwData::PatientDB::NewSptr pNewPatientDB;

    ::fwData::PatientDB::PatientIterator patientIter = _pPatientDB->getPatients().first;
    for ( ; patientIter != _pPatientDB->getPatients().second ; ++patientIter)
    {
        pNewPatientDB->addPatient(AnonymiseImage::createAnonymisedPatient(*patientIter));
    }

    return pNewPatientDB;
}

//------------------------------------------------------------------------------


} // fieldHelper

} // fwComEd
