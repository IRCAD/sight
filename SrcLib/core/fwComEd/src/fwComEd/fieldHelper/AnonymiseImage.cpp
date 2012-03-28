/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
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

    std::for_each(_pPatient->getStudies().begin(), _pPatient->getStudies().end(), &AnonymiseImage::anonymiseStudy );
}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymisePatientDB( ::fwData::PatientDB::sptr _pPatientDB)
{
    std::for_each(_pPatientDB->getPatients().begin(), _pPatientDB->getPatients().end(), &AnonymiseImage::anonymisePatient );
}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymiseImage( ::fwData::Image::sptr _pImage)
{
}

//------------------------------------------------------------------------------

void AnonymiseImage::anonymiseStudy( ::fwData::Study::sptr _pStudy)
{
    _pStudy->setCRefHospital("anonymous");
    _pStudy->setCRefRISId("");
    _pStudy->setCRefUID("");

    std::for_each(_pStudy->getAcquisitions().begin(), _pStudy->getAcquisitions().end(), &AnonymiseImage::anonymiseAcquisition );
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
    pNewPatient->shallowCopy( _pPatient );

    std::map< ::fwData::Acquisition::sptr, ::fwData::Acquisition::sptr > acquisitionMap;

    BOOST_FOREACH( ::fwData::Study::sptr pStudy, _pPatient->getStudies() )
    {
        ::fwData::Study::NewSptr pNewStudy;
        pNewPatient->addStudy(pNewStudy);

        pNewStudy->shallowCopy( pStudy );

        BOOST_FOREACH( ::fwData::Acquisition::sptr pAcquisition, pStudy->getAcquisitions() )
        {
            ::fwData::Acquisition::NewSptr pNewAcquisition;
            pNewStudy->addAcquisition(pNewAcquisition);

            pNewAcquisition->shallowCopy(pAcquisition);

            acquisitionMap[pAcquisition] = pNewAcquisition;
        }
    }

    AnonymiseImage::anonymisePatient(pNewPatient);

    return pNewPatient;
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr AnonymiseImage::createAnonymisedPatientDB( ::fwData::PatientDB::sptr _pPatientDB)
{
    ::fwData::PatientDB::NewSptr pNewPatientDB;

    BOOST_FOREACH( fwData::Patient::sptr patient, _pPatientDB->getPatients() )
    {
        pNewPatientDB->addPatient(AnonymiseImage::createAnonymisedPatient(patient));
    }

    return pNewPatientDB;
}

//------------------------------------------------------------------------------


} // fieldHelper

} // fwComEd
