/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <data/Study.hpp>

#include <gdcmReader.h>
#include <gdcmScanner.h>
#include <gdcmUIDGenerator.h>

namespace sight::io::dicom
{

namespace container
{

//------------------------------------------------------------------------------

DicomInstance::DicomInstance() :
    m_isMultiFiles(true),
    m_SOPClassUID(""),
    m_logger(nullptr)
{
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(
    const data::Series::csptr& series,
    const core::log::Logger::sptr& logger,
    bool isMultiFiles
) :
    m_isMultiFiles(isMultiFiles),
    m_SOPClassUID(""),
    m_studyInstanceUID(series->getStudy()->getInstanceUID()),
    m_seriesInstanceUID(series->getInstanceUID()),
    m_logger(logger)
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
    const core::log::Logger::sptr& logger
) :
    m_isMultiFiles(dicomSeries->getDicomContainer().size() > 1),
    m_studyInstanceUID(dicomSeries->getStudy()->getInstanceUID()),
    m_seriesInstanceUID(dicomSeries->getInstanceUID()),
    m_logger(logger)
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

DicomInstance::DicomInstance(const DicomInstance& dicomInstance)
{
    m_isMultiFiles            = dicomInstance.m_isMultiFiles;
    m_SOPClassUID             = dicomInstance.m_SOPClassUID;
    m_SOPInstanceUIDContainer = dicomInstance.m_SOPInstanceUIDContainer;
    m_logger                  = dicomInstance.m_logger;
}

//------------------------------------------------------------------------------

DicomInstance::~DicomInstance()
{
}

//------------------------------------------------------------------------------

void DicomInstance::computeSOPClassUID(const data::Series::csptr& series)
{
    // Retrieve series type
    data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicCast(series);
    data::ModelSeries::csptr modelSeries = data::ModelSeries::dynamicCast(series);

    // Create result
    std::string sopClassUID = "";

    if(imageSeries)
    {
        // Retrieve image from series
        data::Image::csptr image = imageSeries->getImage();

        // Compute instance dimension
        unsigned int dimension =
            (this->getIsMultiFiles()) ? 2 : static_cast<unsigned int>(image->numDimensions());

        // Define SOP Class UID from the modality
        gdcm::MediaStorage mediaStorage;
        mediaStorage.GuessFromModality(series->getModality().c_str(), dimension);

        // Identify the SOPClassUID from a guess
        if(mediaStorage != gdcm::MediaStorage::MS_END && mediaStorage.GetString() != 0)
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
    data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicConstCast(series);

    // Compute number of instances
    const std::size_t nbInstances = (imageSeries && m_isMultiFiles) ? (imageSeries->getImage()->getSize()[2]) : (1);

    // Create generator
    gdcm::UIDGenerator uidGenerator;

    // Generate UIDs
    for(std::size_t i = 0 ; i < nbInstances ; ++i)
    {
        m_SOPInstanceUIDContainer.push_back(uidGenerator.Generate());
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
        const core::memory::BufferObject::sptr bufferObj         = item.second;
        const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
        SPTR(std::istream) is = streamInfo.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);
        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->getStreamInfo().fsFile.string() << "' "
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

} //namespace container

} //namespace sight::io::dicom
