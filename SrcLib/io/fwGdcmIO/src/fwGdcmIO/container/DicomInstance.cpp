/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwData/Image.hpp>
#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <gdcmUIDGenerator.h>
#include <gdcmScanner.h>

namespace fwGdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomInstance::DicomInstance() :
    m_isMultiFiles(true),
    m_logger(nullptr),
    m_SOPClassUID("")
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(const ::fwMedData::Series::sptr& series,
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

DicomInstance::DicomInstance(const ::fwMedData::DicomSeries::sptr& dicomSeries,
                             const ::fwLog::Logger::sptr& logger) :
    m_isMultiFiles(dicomSeries->getLocalDicomPaths().size()>1),
    m_studyInstanceUID(dicomSeries->getStudy()->getInstanceUID()),
    m_seriesInstanceUID(dicomSeries->getInstanceUID()),
    m_logger(logger)
{
    SLM_ASSERT("DicomSeries is not instantiated", dicomSeries);
    SLM_ASSERT("DicomSeries availability shall be PATHS.",
        dicomSeries->getDicomAvailability() == ::fwMedData::DicomSeries::PATHS);

    // Get SOPClassUID
    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
    if(!sopClassUIDContainer.empty())
    {
        m_SOPClassUID = sopClassUIDContainer.begin()->c_str();
    }

    this->readUIDFromDicomSeries(dicomSeries);

}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance(const DicomInstance &dicomInstance)
{
    SLM_TRACE_FUNC();
    m_isMultiFiles = dicomInstance.m_isMultiFiles;
    m_SOPClassUID = dicomInstance.m_SOPClassUID;
    m_SOPInstanceUIDContainer = dicomInstance.m_SOPInstanceUIDContainer;
    m_logger = dicomInstance.m_logger;
}

//------------------------------------------------------------------------------

DicomInstance::~DicomInstance()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomInstance::computeSOPClassUID(const ::fwMedData::Series::csptr& series)
{
    // Retrieve series type
    ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicConstCast(series);
    ::fwMedData::ModelSeries::csptr modelSeries = ::fwMedData::ModelSeries::dynamicConstCast(series);

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
    std::size_t nbInstances = (imageSeries && m_isMultiFiles) ? (imageSeries->getImage()->getSize()[2]) : (1);

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
    ::gdcm::Scanner scanner;
    ::gdcm::Tag SOPInstanceUIDTag = ::gdcm::Tag(0x0008, 0x0018);    // SOP Instance UID
    scanner.AddTag(SOPInstanceUIDTag);
    ::gdcm::Tag frameOfReferenceUIDTag = ::gdcm::Tag(0x0020, 0x0052);    // Frame of Reference UID
    scanner.AddTag(frameOfReferenceUIDTag);

    std::vector< std::string > filenames;
    for(const auto& entry : dicomSeries->getLocalDicomPaths())
    {
        const std::string path = entry.second.string();
        filenames.push_back(path);
    }

    bool status = scanner.Scan( filenames );
    FW_RAISE_IF("Unable to read the files.", !status);

    ::gdcm::Directory::FilenamesType keys = scanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator it;
    for(it = keys.begin(); it != keys.end(); ++it)
    {
        const std::string filename = *it;

        // SOP Instance UID
        const std::string SOPInstanceUID = scanner.GetValue( filename.c_str(), SOPInstanceUIDTag );
        m_SOPInstanceUIDContainer.push_back(SOPInstanceUID);
    }

    // Retrieve frame of reference UID
    std::set<std::string> frameOfReferenceUIDContainer = scanner.GetValues(frameOfReferenceUIDTag);

    if(frameOfReferenceUIDContainer.size() == 1)
    {
        m_frameOfReferenceUID = *(frameOfReferenceUIDContainer.begin());
    }
    else if(frameOfReferenceUIDContainer.size() > 1)
    {
        const std::string msg = "The selected DICOM series contain several Frame of Reference.";
        SLM_WARN_IF(msg,!m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }
    else
    {
        const std::string msg = "No Frame of Reference has been found in the selected series.";
        SLM_WARN_IF(msg,!m_logger);
        if(m_logger)
        {
            m_logger->critical(msg);
        }
    }

}

//------------------------------------------------------------------------------
} //namespace container
} //namespace fwGdcmIO
