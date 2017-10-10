/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/reader/ie/Patient.hpp"

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Patient.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Patient::Patient(const ::fwMedData::DicomSeries::sptr& dicomSeries,
                 const SPTR(::gdcm::Reader)& reader,
                 const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                 const ::fwMedData::Patient::sptr& patient,
                 const ::fwLog::Logger::sptr& logger,
                 ProgressCallback progress,
                 CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Patient >(dicomSeries, reader, instance, patient, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{
}

//------------------------------------------------------------------------------

void Patient::readPatientModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Patient's name - Type 2
    const std::string& patientName =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0010 >(dataset);
    m_object->setName(patientName);

    // Patient's ID - Type 2
    const std::string& patientId =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0020 >(dataset);
    m_object->setPatientId(patientId);

    // Patient's birth date - Type 2
    const std::string& birthDate =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0030 >(dataset);
    m_object->setBirthdate(birthDate);

    // Patient's sex - Type 2
    const std::string& sex =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0010, 0x0040 >(dataset);
    m_object->setSex(sex);
}

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
