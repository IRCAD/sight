/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/writer/ie/Series.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <data/ImageSeries.hpp>
#include <data/Series.hpp>
#include <data/types.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Series::Series(const SPTR(::gdcm::Writer)& writer,
               const SPTR(io::dicom::container::DicomInstance)& instance,
               const data::Series::csptr& series,
               const core::log::Logger::sptr& logger,
               ProgressCallback progress,
               CancelRequestedCallback cancel) :
    io::dicom::writer::ie::InformationEntity< data::Series>(writer, instance, series,
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

    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0060>(m_object->getModality(), dataset);

    // As the data may have been updated between two export, we regenerate an UID
    ::gdcm::UIDGenerator uidGenerator;
    const std::string instanceUID = uidGenerator.Generate();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000e>(instanceUID, dataset);

    if(!m_object->getNumber().empty())
    {
        io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0020, 0x0011>
            (std::stoi(m_object->getNumber()), dataset);
    }

    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x0060>(m_object->getLaterality(), dataset);

    ::boost::posix_time::ptime ptime = ::boost::posix_time::second_clock::local_time();
    const std::string fulldate = ::boost::posix_time::to_iso_string(ptime);
    // Split iso time in YYYYMMDDTHHMMSS
    ::boost::char_separator<char> sep("T");
    ::boost::tokenizer< ::boost::char_separator<char> > tokens(fulldate, sep);
    ::boost::tokenizer< ::boost::char_separator<char> >::iterator tok_iter = tokens.begin();
    const std::string date = *tok_iter;
    tok_iter++;
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0021>(date, dataset);

    const std::string time = *tok_iter;
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0031>(time, dataset);

    // Performing physicians name - Type 3
    data::DicomValuesType performingPhysicians = m_object->getPerformingPhysiciansName();
    if (!performingPhysicians.empty())
    {
        ::gdcm::String<>* physicians = new ::gdcm::String<>[performingPhysicians.size()];
        unsigned int count = 0;
        for(std::string physician: performingPhysicians)
        {
            physicians[count++] = ::gdcm::String<>(physician);
        }
        io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String<>, 0x0008, 0x1050>(physicians, count,
                                                                                            dataset);
    }

    io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x1030>(m_object->getProtocolName(), dataset);

    // Serie's description
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x103e>(m_object->getDescription(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x0015>(m_object->getBodyPartExamined(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x5100>(m_object->getPatientPosition(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0010, 0x2210>(
        m_object->getAnatomicalOrientationType(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0253>(
        m_object->getPerformedProcedureStepID(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0244>(
        m_object->getPerformedProcedureStepStartDate(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0245>(
        m_object->getPerformedProcedureStepStartTime(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0250>(
        m_object->getPerformedProcedureStepEndDate(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0251>(
        m_object->getPerformedProcedureStepEndTime(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0254>(
        m_object->getPerformedProcedureStepDescription(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0x0280>(
        m_object->getPerformedProcedureComments(), dataset);

    const data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicCast(m_object);
    if(imageSeries)
    {
        io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x0010>(imageSeries->getContrastAgent(), dataset);
        io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x1040>(imageSeries->getContrastRoute(), dataset);
        if(!imageSeries->getContrastVolume().empty())
        {
            io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0018, 0x1041>
                (std::stoi(imageSeries->getContrastVolume()), dataset);
        }
        io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x1042>(imageSeries->getContrastStartTime(), dataset);
        io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x1043>(imageSeries->getContrastStopTime(), dataset);
        if(!imageSeries->getContrastTotalDose().empty())
        {
            io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0018, 0x1044>
                (std::stoi(imageSeries->getContrastTotalDose()),  dataset);
        }
        if(!imageSeries->getContrastFlowRate().empty())
        {
            io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0018, 0x1046>
                (std::stoi(imageSeries->getContrastFlowRate()), dataset);
        }
        if(!imageSeries->getContrastFlowDuration().empty())
        {
            io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0018, 0x1047>
                (std::stoi(imageSeries->getContrastFlowDuration()), dataset);
        }
        io::dicom::helper::DicomDataWriter::setTagValue<0x0018, 0x1048>
            (imageSeries->getContrastIngredient(), dataset);
        if(!imageSeries->getContrastIngredientConcentration().empty())
        {
            io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0018, 0x1049>
                (std::stoi(imageSeries->getContrastIngredientConcentration()), dataset);
        }
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0022>(imageSeries->getAcquisitionDate(), dataset);
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0032>(imageSeries->getAcquisitionTime(), dataset);
    }
}

//------------------------------------------------------------------------------

void Series::writeSegmentationSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Series's modality - Type 1
    dataset.Remove(::gdcm::Tag(0x0008, 0x0060));
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0060>("SEG", dataset);
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
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0060>("SR", dataset);

    // Serie's instance UID - Type 1
    dataset.Remove(::gdcm::Tag(0x0020, 0x000e));
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000e>(uidGenerator.Generate(), dataset);

    // Serie's number - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue< int, 0x0020, 0x0011>(0, dataset);

    // Referenced Performed Procedure Step Sequence  - Type 2
    io::dicom::helper::DicomDataWriter::createAndSetSequenceTagValue<0x0008, 0x1111>(dataset);
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
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000e>(uidGenerator.Generate(), dataset);

    // Series's modality - Type 1
    dataset.Remove(::gdcm::Tag(0x0008, 0x0060));
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0060>("FID", dataset);

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace sight::io::dicom
