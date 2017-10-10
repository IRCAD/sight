/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomDataWriter.hxx"
#include "fwGdcmIO/writer/ie/Series.hpp"

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Series::Series(const SPTR(::gdcm::Writer)& writer,
               const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
               const ::fwMedData::Series::sptr& series,
               const ::fwLog::Logger::sptr& logger,
               ProgressCallback progress,
               CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >(writer, instance, series,
                                                                     logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::writeGeneralSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Serie's instance UID - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x000e >(m_object->getInstanceUID(), dataset);

    // Series's modality - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0060 >(m_object->getModality(), dataset);

    // Serie's date - Type 3
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0021 >(m_object->getDate(), dataset);

    // Serie's time - Type 3
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0031 >(m_object->getTime(), dataset);

    // Serie's description
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x103e >(m_object->getDescription(), dataset);

    // Serie's number - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< int, 0x0020, 0x0011 >(0, dataset);

    // Performing physicians name - Type 3
    ::fwMedData::DicomValuesType performingPhysicians = m_object->getPerformingPhysiciansName();
    if (!performingPhysicians.empty())
    {
        ::gdcm::String< >* physicians = new ::gdcm::String< >[performingPhysicians.size()];
        unsigned int count = 0;
        for(std::string physician: performingPhysicians)
        {
            physicians[count++] = ::gdcm::String<>(physician);
        }
        ::fwGdcmIO::helper::DicomDataWriter::setTagValues< ::gdcm::String< >, 0x0008, 0x1050 >(physicians, count, dataset);
    }

    // Laterality - Type 2C - FIXME: Fake Value - Should be absent for the abdomen or chest
    if(m_instance->getSOPClassUID() !=
            ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::SurfaceSegmentationStorage))
    {
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x0060 >("R", dataset);
    }

    // Patient Position - Type 2C
    ::gdcm::Attribute< 0x0018, 0x5100 > patientPositionAttribute;
    dataset.Insert(patientPositionAttribute.GetAsDataElement());
}

//------------------------------------------------------------------------------

void Series::writeSegmentationSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Series's modality - Type 1
    dataset.Remove(::gdcm::Tag(0x0008, 0x0060));
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0060 >("SEG", dataset);
}

//------------------------------------------------------------------------------

void Series::writeSRDocumentSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Create generator
    ::gdcm::UIDGenerator uidGenerator;

    // Series's modality - Type 1
    dataset.Remove(::gdcm::Tag(0x0008, 0x0060));
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0060 >("SR", dataset);

    // Serie's instance UID - Type 1
    dataset.Remove(::gdcm::Tag(0x0020, 0x000e));
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x000e >(uidGenerator.Generate(), dataset);

    // Serie's number - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< int, 0x0020, 0x0011 >(0, dataset);

    // Referenced Performed Procedure Step Sequence  - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0008,0x1111 >(dataset);
}

//------------------------------------------------------------------------------

void Series::writeSpatialFiducialsSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Create uid generator
    ::gdcm::UIDGenerator uidGenerator;

    // Serie's instance UID - Type 1
    dataset.Remove(::gdcm::Tag(0x0020, 0x000e));
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x000e >(uidGenerator.Generate(), dataset);

    // Series's modality - Type 1
    dataset.Remove(::gdcm::Tag(0x0008, 0x0060));
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0060 >("FID", dataset);

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
