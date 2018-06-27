/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomInstance.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <fwData/Image.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <gdcmReader.h>
#include <gdcmScanner.h>
#include <gdcmUIDGenerator.h>

namespace fwGdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomInstance::DicomInstance() :
    m_isMultiFiles(true),
    m_SOPClassUID(""),
    m_logger(nullptr)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(const ::fwMedData::Series::csptr& series,
                             const ::fwLog::Logger::sptr& logger,
                             bool isMultiFiles) :
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
    ::gdcm::UIDGenerator uidGenerator;
    m_frameOfReferenceUID = uidGenerator.Generate();
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                             const ::fwLog::Logger::sptr& logger) :
    m_isMultiFiles(dicomSeries->getDicomContainer().size() > 1),
    m_studyInstanceUID(dicomSeries->getStudy()->getInstanceUID()),
    m_seriesInstanceUID(dicomSeries->getInstanceUID()),
    m_logger(logger)
{
    SLM_ASSERT("DicomSeries is not instantiated", dicomSeries);

    // Get SOPClassUID
    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
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

void DicomInstance::computeSOPClassUID(const ::fwMedData::Series::csptr& series)
{
    // Retrieve series type
    ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    ::fwMedData::ModelSeries::csptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);

    // Create result
    std::string sopClassUID = "";

    if(imageSeries)
    {
        // Retrieve image from series
        ::fwData::Image::csptr image = imageSeries->getImage();

        // Compute instance dimension
        unsigned int dimension =
            (this->getIsMultiFiles()) ? 2 : static_cast<unsigned int>(image->getNumberOfDimensions());

        // Define SOP Class UID from the modality
        ::gdcm::MediaStorage mediaStorage;
        mediaStorage.GuessFromModality(series->getModality().c_str(), dimension);

        // Identify the SOPClassUID from a guess
        if (mediaStorage != ::gdcm::MediaStorage::MS_END && mediaStorage.GetString() != 0)
        {
            sopClassUID = mediaStorage.GetString();
        }
        // Force SOPClassUID to be CTImageStorage
        else
        {
            sopClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::CTImageStorage);
        }
    }
    else if(modelSeries)
    {
        sopClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::SurfaceSegmentationStorage);
    }

    // Update instance information
    this->setSOPClassUID(sopClassUID);
}

//------------------------------------------------------------------------------

void DicomInstance::generateSOPInstanceUIDs(const ::fwMedData::Series::csptr& series)
{
    // Retrieve ImageSeries
    ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicConstCast(series);

    // Compute number of instances
    const std::size_t nbInstances = (imageSeries && m_isMultiFiles) ? (imageSeries->getImage()->getSize()[2]) : (1);

    // Create generator
    ::gdcm::UIDGenerator uidGenerator;

    // Generate UIDs
    for(std::size_t i = 0; i < nbInstances; ++i)
    {
        m_SOPInstanceUIDContainer.push_back(uidGenerator.Generate());
    }
}

//------------------------------------------------------------------------------

void DicomInstance::readUIDFromDicomSeries(const ::fwMedData::DicomSeries::csptr& dicomSeries)
{
    const ::gdcm::Tag SOPInstanceUIDTag      = ::gdcm::Tag(0x0008, 0x0018); // SOP Instance UID
    const ::gdcm::Tag frameOfReferenceUIDTag = ::gdcm::Tag(0x0020, 0x0052);    // Frame of Reference UID
    std::set< ::gdcm::Tag > selectedtags;
    selectedtags.insert( SOPInstanceUIDTag );
    selectedtags.insert( frameOfReferenceUIDTag );

    std::set<std::string> frameOfReferenceUIDContainer;
    for(const auto& item : dicomSeries->getDicomContainer())
    {
        const ::fwMemory::BufferObject::sptr bufferObj         = item.second;
        const ::fwMemory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
        SPTR(std::istream) is = streamInfo.stream;

        ::gdcm::Reader reader;
        reader.SetStream(*is);
        if(!reader.ReadSelectedTags(selectedtags))
        {
            FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"' "<<
                     "(slice: '" << item.first << "')");
        }
        const ::gdcm::DataSet& dataset = reader.GetFile().GetDataSet();
        // SOP Instance UID
        m_SOPInstanceUIDContainer.push_back(::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0008, 0x0018>(dataset));
        // Retrieve frame of reference UID
        frameOfReferenceUIDContainer.insert(::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0020, 0x0052>(dataset));
    }

    if(frameOfReferenceUIDContainer.size() == 1)
    {
        m_frameOfReferenceUID = *(frameOfReferenceUIDContainer.begin());
    }
    else if(frameOfReferenceUIDContainer.size() > 1)
    {
        const std::string msg = "The selected DICOM series contain several Frame of Reference.";
        SLM_WARN_IF(msg, !m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
    else
    {
        const std::string msg = "No Frame of Reference has been found in the selected series.";
        SLM_WARN_IF(msg, !m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
}

//------------------------------------------------------------------------------
} //namespace container
} //namespace fwGdcmIO
