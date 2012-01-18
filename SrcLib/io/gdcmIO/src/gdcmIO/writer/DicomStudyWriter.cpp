/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// gdcm
#include <gdcmUIDGenerator.h>

// fwTools
#include <fwTools/dateAndTime.hpp>

#include "gdcmIO/writer/DicomStudyWriter.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomStudyWriter::DicomStudyWriter():
        m_studyID("1")
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomStudyWriter::~DicomStudyWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomStudyWriter::write(::gdcm::DataSet & a_gDs)
{
    SLM_TRACE_FUNC();

    ::fwData::Study::csptr study = this->getConcreteObject();
    SLM_ASSERT("fwData::Study not instanced", study);

    // Study's date
    helper::GdcmData::setTagValue<0x0008,0x0020>(study->getDate(), a_gDs);   // Type 2
    OSLM_TRACE("Study's date : "<<study->getDate());

    // Study's time
    helper::GdcmData::setTagValue<0x0008,0x0030>(study->getTime(), a_gDs);   // Type 2
    OSLM_TRACE("Study's time : "<<study->getTime());

    // Study 's accession number
    helper::GdcmData::setTagValue<0x0008,0x0050>("", a_gDs);                // Type 2
    OSLM_TRACE("Study's accession number : "<<"");

    // Study's description
    helper::GdcmData::setTagValue<0x0008,0x1030>(study->getDescription(), a_gDs);   // Type 3
    OSLM_TRACE("Study's description : "<<study->getDescription());

    // Study's UID
    std::string studyUID = study->getUID();
    if (studyUID.empty())
    {// Generate an UID
        ::gdcm::UIDGenerator gUIDgen;
        studyUID = gUIDgen.Generate();
    }
    helper::GdcmData::setTagValue<0x0020,0x000d>(studyUID, a_gDs);   // Type 1
    OSLM_TRACE("Study's UID : " << studyUID);

    // Study's ID
    helper::GdcmData::setTagValue<0x0020,0x0010>(m_studyID, a_gDs);   // Type 2
    OSLM_TRACE("Study's ID : " << m_studyID);

    //*****     Set the DicomInstance       *****//
    ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();
    dicomInstance->setStudyInstanceUID( studyUID );             // Store study UID to be used and written later

    dicomInstance->setModality( study->getModality() );         // Store modality to be used and written later

    dicomInstance->setInstitutionName( study->getHospital() );  // Store hospital name to be used and written later
}

//------------------------------------------------------------------------------

void DicomStudyWriter::setStudyID(const unsigned int a_studyID)
{
    this->m_studyID = ::fwTools::getString(a_studyID);
}

}   // namespace writer

}   // namespace gdcmIO
