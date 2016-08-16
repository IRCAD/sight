/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGdcmIO/helper/DicomData.hpp"
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

Patient::Patient(::fwMedData::DicomSeries::sptr dicomSeries,
                 SPTR(::gdcm::Reader)reader,
                 SPTR(::fwGdcmIO::container::DicomInstance)instance,
                 ::fwMedData::Patient::sptr patient,
                 ::fwLog::Logger::sptr logger,
                 const ProgressCallback& callback,
                 const bool& cancelled) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Patient >(dicomSeries, reader, instance, patient, logger,
                                                                      callback, cancelled)
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
    ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Patient's name - Type 2
    const std::string& patientName = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0010 >(dataset);
    m_object->setName(patientName);

    // Patient's ID - Type 2
    const std::string& patientId = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0020 >(dataset);
    m_object->setPatientId(patientId);

    // Patient's birth date - Type 2
    const std::string& birthDate = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0030 >(dataset);
    m_object->setBirthdate(birthDate);

    // Patient's sex - Type 2
    const std::string& sex = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0010, 0x0040 >(dataset);
    m_object->setSex(sex);
}

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
