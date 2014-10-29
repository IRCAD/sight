/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Study.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Study.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Study::Study(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwMedData::Study::sptr study):
        ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Study >(dicomSeries, reader, instance, study)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{
}

//------------------------------------------------------------------------------

void Study::readGeneralStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Study's date - Type 2
    const std::string& studyDate = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0020 >(dataset);
    m_object->setDate(studyDate);

    // Study's time - Type 2
    const std::string& studyTime = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0030 >(dataset);
    m_object->setTime(studyTime);

    // Study's accession number - Type 2
    // NOTE: Not used in FW4SPL

    // Study's description - Type 3
    const std::string& description = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x1030 >(dataset);
    m_object->setDescription(description);

    // Study's UID - Type 1
    const std::string& instanceUID = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0020, 0x000d >(dataset);
    m_object->setInstanceUID(instanceUID);

    // Study's ID - Type 2
    // NOTE: Not used in FW4SPL

    // Study's referring physician name
    const std::string& referringPhysicianName = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0090 >(dataset);
    m_object->setReferringPhysicianName(referringPhysicianName);
}

//------------------------------------------------------------------------------

void Study::readPatientStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // PatientAge - Type 3
    const std::string& patientAge = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x1010 >(dataset);
    m_object->setPatientAge(patientAge);
}

//------------------------------------------------------------------------------

}// namespace ie
} // namespace reader
} // namespace gdcmIO
