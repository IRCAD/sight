/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/ie/Study.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Study.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Study::Study(const ::fwMedData::DicomSeries::sptr& dicomSeries,
             const SPTR(::gdcm::Reader)& reader,
             const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
             const ::fwMedData::Study::sptr& study,
             const ::fwLog::Logger::sptr& logger,
             ProgressCallback progress,
             CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Study >(dicomSeries, reader, instance, study, logger,
                                                                    progress, cancel)
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
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Study's date - Type 2
    const std::string& studyDate = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0020 >(dataset);
    m_object->setDate(studyDate);

    // Study's time - Type 2
    const std::string& studyTime = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0030 >(dataset);
    m_object->setTime(studyTime);

    // Study's accession number - Type 2
    // NOTE: Not used in FW4SPL

    // Study's description - Type 3
    const std::string& description =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x1030 >(dataset);
    m_object->setDescription(description);

    // Study's UID - Type 1
    const std::string& instanceUID =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0020, 0x000d >(dataset);
    m_object->setInstanceUID(instanceUID);

    // Study's ID - Type 2
    // NOTE: Not used in FW4SPL

    // Study's referring physician name
    const std::string& referringPhysicianName =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0090 >(dataset);
    m_object->setReferringPhysicianName(referringPhysicianName);
}

//------------------------------------------------------------------------------

void Study::readPatientStudyModule()
{
    // Retrieve dataset
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // PatientAge - Type 3
    const std::string& patientAge =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x1010 >(dataset);
    m_object->setPatientAge(patientAge);
}

//------------------------------------------------------------------------------

}// namespace ie
} // namespace reader
} // namespace fwGdcmIO
