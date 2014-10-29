/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Patient.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Patient.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Patient::Patient(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwMedData::Patient::sptr patient):
        ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Patient >(dicomSeries, reader, instance, patient)
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
    const std::string& patientName = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0010 >(dataset);
    m_object->setName(patientName);

    // Patient's ID - Type 2
    const std::string& patientId = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0020 >(dataset);
    m_object->setPatientId(patientId);

    // Patient's birth date - Type 2
    const std::string& birthDate = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0030 >(dataset);
    m_object->setBirthdate(birthDate);

    // Patient's sex - Type 2
    const std::string& sex = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0040 >(dataset);
    m_object->setSex(sex);
}

} // namespace ie
} // namespace reader
} // namespace gdcmIO
