/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/container/DicomInstance.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/DicomSeries.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Series.hpp>

#include <gdcmReader.h>
#include <gdcmScanner.h>
#include <gdcmUIDGenerator.h>

#include <utility>

namespace sight::io::dicom::container
{

//------------------------------------------------------------------------------

DicomInstance::DicomInstance() :

    m_logger(nullptr)
{
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(
    const data::Series::csptr& series,
    core::log::logger::sptr logger,
    bool isMultiFiles
) :
    m_isMultiFiles(isMultiFiles),
    m_studyInstanceUID(series->getStudyInstanceUID()),
    m_seriesInstanceUID(series->getSeriesInstanceUID()),
    m_logger(std::move(logger))
{
    // Compute SOPClassUID
    this->computeSOPClassUID(series);

    // Generate SOPInstanceUIDs
    this->generateSOPInstanceUIDs(series);

    // Generate Frame of Reference UID
    gdcm::UIDGenerator uidGenerator;
    m_frameOfReferenceUID = uidGenerator.Generate();
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(
    const data::DicomSeries::csptr& dicomSeries,
    core::log::logger::sptr logger
) :
    m_isMultiFiles(dicomSeries->getDicomContainer().size() > 1),
    m_studyInstanceUID(dicomSeries->getStudyInstanceUID()),
    m_seriesInstanceUID(dicomSeries->getSeriesInstanceUID()),
    m_logger(std::move(logger))
{
    SIGHT_ASSERT("DicomSeries is not instantiated", dicomSeries);

    // Get SOPClassUID
    data::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
    if(!sopClassUIDContainer.empty())
    {
        m_SOPClassUID = *(sopClassUIDContainer.begin());
    }

    this->readUIDFromDicomSeries(dicomSeries);
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(const DicomInstance& dicomInstance) :
    m_isMultiFiles(dicomInstance.m_isMultiFiles),
    m_SOPClassUID(dicomInstance.m_SOPClassUID),
    m_SOPInstanceUIDContainer(dicomInstance.m_SOPInstanceUIDContainer),
    m_logger(dicomInstance.m_logger)
{
}

//------------------------------------------------------------------------------

DicomInstance::~DicomInstance()
= default;

//------------------------------------------------------------------------------

void DicomInstance::computeSOPClassUID(const data::Series::csptr& series)
{
    // Retrieve series type
    data::ImageSeries::csptr imageSeries = std::dynamic_pointer_cast<const data::ImageSeries>(series);
    data::ModelSeries::csptr modelSeries = std::dynamic_pointer_cast<const data::ModelSeries>(series);

    // Create result
    std::string sopClassUID;

    if(imageSeries)
    {
        // Compute instance dimension
        unsigned int dimension = getIsMultiFiles() ? 2 : static_cast<unsigned int>(imageSeries->numDimensions());

        // Define SOP Class UID from the modality
        gdcm::MediaStorage mediaStorage;
        mediaStorage.GuessFromModality(series->getModality().c_str(), dimension);

        // Identify the SOPClassUID from a guess
        if(mediaStorage != gdcm::MediaStorage::MS_END && mediaStorage.GetString() != nullptr)
        {
            sopClassUID = mediaStorage.GetString();
        }
        // Force SOPClassUID to be CTImageStorage
        else
        {
            sopClassUID = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage);
        }
    }
    else if(modelSeries)
    {
        sopClassUID = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::SurfaceSegmentationStorage);
    }

    // Update instance information
    this->setSOPClassUID(sopClassUID);
}

//------------------------------------------------------------------------------

void DicomInstance::generateSOPInstanceUIDs(const data::Series::csptr& series)
{
    // Retrieve ImageSeries
    data::ImageSeries::csptr imageSeries = std::dynamic_pointer_cast<const data::ImageSeries>(series);

    // Compute number of instances
    const std::size_t nb_instances = (imageSeries && m_isMultiFiles) ? (imageSeries->size()[2]) : (1);

    // Create generator
    gdcm::UIDGenerator uidGenerator;

    // Generate UIDs
    for(std::size_t i = 0 ; i < nb_instances ; ++i)
    {
        m_SOPInstanceUIDContainer.emplace_back(uidGenerator.Generate());
    }
}

//------------------------------------------------------------------------------

void DicomInstance::readUIDFromDicomSeries(const data::DicomSeries::csptr& dicomSeries)
{
    const gdcm::Tag SOPInstanceUIDTag      = gdcm::Tag(0x0008, 0x0018); // SOP Instance UID
    const gdcm::Tag frameOfReferenceUIDTag = gdcm::Tag(0x0020, 0x0052); // Frame of Reference UID
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(SOPInstanceUIDTag);
    selectedtags.insert(frameOfReferenceUIDTag);

    std::set<std::string> frameOfReferenceUIDContainer;
    for(const auto& item : dicomSeries->getDicomContainer())
    {
        const core::memory::buffer_object::sptr bufferObj          = item.second;
        const core::memory::buffer_manager::stream_info streamInfo = bufferObj->get_stream_info();
        SPTR(std::istream) is = streamInfo.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);
        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();
        // SOP Instance UID
        m_SOPInstanceUIDContainer.push_back(io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0018>(dataset));
        // Retrieve frame of reference UID
        frameOfReferenceUIDContainer.insert(io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0052>(dataset));
    }

    if(frameOfReferenceUIDContainer.size() == 1)
    {
        m_frameOfReferenceUID = *(frameOfReferenceUIDContainer.begin());
    }
    else if(frameOfReferenceUIDContainer.size() > 1)
    {
        const std::string msg = "The selected DICOM series contain several Frame of Reference.";
        SIGHT_WARN_IF(msg, !m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
    else
    {
        const std::string msg = "No Frame of Reference has been found in the selected series.";
        SIGHT_WARN_IF(msg, !m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container
