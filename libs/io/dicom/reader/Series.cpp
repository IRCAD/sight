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

#include "io/dicom/reader/Series.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/reader/iod/ComprehensiveSRIOD.hpp"
#include "io/dicom/reader/iod/CTMRImageIOD.hpp"
#include "io/dicom/reader/iod/SpatialFiducialsIOD.hpp"
#include "io/dicom/reader/iod/SurfaceSegmentationIOD.hpp"

#include <data/dicom/Series.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/SeriesDB.hpp>
#include <data/Study.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <gdcmImageReader.h>
#include <gdcmScanner.h>

namespace sight::io::dicom
{

namespace reader
{

//------------------------------------------------------------------------------

Series::Series() :
    m_enableBufferRotation(true)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

// ----------------------------------------------------------------------------

data::Series::sptr Series::read(const data::DicomSeries::csptr& dicomSeries)
{
    SIGHT_ASSERT("DicomSeries should not be null.", dicomSeries);
    SIGHT_ASSERT("Logger should not be null.", m_logger);

    // Create instance
    SPTR(io::dicom::container::DicomInstance) instance =
        std::make_shared<io::dicom::container::DicomInstance>(dicomSeries);

    // Create result
    data::Series::sptr result;

    if(!dicomSeries->getDicomContainer().empty())
    {
        // Get SOPClassUID
        data::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
        const std::string sopClassUID                                    = *sopClassUIDContainer.begin();

        // If the DicomSeries contains an image (ImageSeries)
        if(::gdcm::MediaStorage::IsImage(::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()))
           && ::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) != ::gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Read the image
            data::ImageSeries::sptr imageSeries = data::dicom::Series::convertToImageSeries(dicomSeries);
            imageSeries->setDicomReference(dicomSeries);
            data::Image::sptr image = data::Image::New();
            imageSeries->setImage(image);

            // Create IOD Reader
            io::dicom::reader::iod::CTMRImageIOD iod(dicomSeries, instance, m_logger,
                                                     m_progressCallback, m_cancelRequestedCallback);
            iod.setBufferRotationEnabled(m_enableBufferRotation);

            try
            {
                iod.read(imageSeries);
            }
            catch(const io::dicom::exception::Failed& e)
            {
                // NOTE : if there is no image, reading is stopped.
                m_logger->critical(e.what());
                imageSeries = data::ImageSeries::sptr();
            }

            // Set result
            result = imageSeries;
        }
        // Get the RT file names (ModelSeries)
        else if(::gdcm::MediaStorage::GetMSType(sopClassUID.c_str())
                == ::gdcm::MediaStorage::SurfaceSegmentationStorage)
        {
            data::ModelSeries::sptr modelSeries = data::dicom::Series::convertToModelSeries(dicomSeries);
            modelSeries->setDicomReference(dicomSeries);
            // Create IOD Reader
            io::dicom::reader::iod::SurfaceSegmentationIOD iod(dicomSeries, instance, m_logger,
                                                               m_progressCallback, m_cancelRequestedCallback);

            try
            {
                iod.read(modelSeries);
            }
            catch(const io::dicom::exception::Failed& e)
            {
                // NOTE : if there is no image, reading is stopped.
                m_logger->critical(e.what());
                modelSeries = data::ModelSeries::sptr();
            }

            // Set result
            result = modelSeries;
        }
        // If the DicomSeries contains a Spatial Fiducials
        else if(::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Retrieve referenced image instance
            SPTR(io::dicom::container::DicomInstance) imageInstance =
                this->getSpatialFiducialsReferencedSeriesInstance(dicomSeries);

            if(imageInstance)
            {
                data::ImageSeries::sptr imageSeries =
                    data::ImageSeries::dynamicCast(m_seriesContainerMap[imageInstance]);

                imageSeries->setDicomReference(dicomSeries);

                // Create IOD Reader
                io::dicom::reader::iod::SpatialFiducialsIOD iod(dicomSeries, instance, m_logger,
                                                                m_progressCallback, m_cancelRequestedCallback);

                try
                {
                    iod.read(imageSeries);
                }
                catch(const io::dicom::exception::Failed& e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    m_logger->critical("Spatial Fiducials reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                m_logger->critical(
                    "The spatial fiducials series \"" + dicomSeries->getInstanceUID()
                    + "\" could not be read as it refers to an unknown series UID."
                );
            }
        }
        // If the DicomSeries contains a SR
        else if(::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR
                || ::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR
                || sopClassUID == "1.2.840.10008.5.1.4.1.1.88.34") // FIXME Replace hard coded string by
                                                                   // "::gdcm::MediaStorage::GetMSType(sopClassUID.c_str())
                                                                   // == ::gdcm::MediaStorage::Comprehensive3DSR"
        {
            // Retrieve referenced image instance
            SPTR(io::dicom::container::DicomInstance) referencedInstance =
                this->getStructuredReportReferencedSeriesInstance(dicomSeries);

            data::ImageSeries::sptr imageSeries;
            const auto& iter = m_seriesContainerMap.find(referencedInstance);
            if(iter != m_seriesContainerMap.end())
            {
                imageSeries = data::ImageSeries::dynamicCast(iter->second);
            }

            if(referencedInstance && imageSeries)
            {
                imageSeries->setDicomReference(dicomSeries);

                // Create readers
                io::dicom::reader::iod::ComprehensiveSRIOD iod(dicomSeries, referencedInstance, m_logger,
                                                               m_progressCallback, m_cancelRequestedCallback);

                try
                {
                    iod.read(imageSeries);
                }
                catch(const io::dicom::exception::Failed& e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    m_logger->critical("Structured Report reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                m_logger->critical(
                    "The structured report series \"" + dicomSeries->getInstanceUID()
                    + "\" could not be read as it refers to an unknown series UID."
                );
            }
        }
        else
        {
            m_logger->critical("DICOM SOP Class UID \"" + sopClassUID + "\" is not supported by the selected reader.");
        }
    }

    // Store series in instance map
    if(result)
    {
        m_seriesContainerMap[instance] = result;
    }

    return result;
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::DicomInstance) Series::getSpatialFiducialsReferencedSeriesInstance(
    const data::DicomSeries::csptr& dicomSeries
)
{
    SPTR(io::dicom::container::DicomInstance) result;

    // Dicom container
    data::DicomSeries::DicomContainerType dicomContainer = dicomSeries->getDicomContainer();

    // Create Reader
    std::shared_ptr< ::gdcm::Reader> reader =
        std::shared_ptr< ::gdcm::Reader>(new ::gdcm::Reader);
    const core::memory::BufferObject::sptr bufferObj         = dicomContainer.begin()->second;
    const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);

    // Series Instance UID of the referenced Series
    std::string seriesInstanceUID = "";

    if(reader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet& datasetRoot = reader->GetFile().GetDataSet();

        if(datasetRoot.FindDataElement(::gdcm::Tag(0x0008, 0x1115)))
        {
            // Get the content sequence
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> sequence =
                datasetRoot.GetDataElement(::gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                ::gdcm::Item referencedSeriesItem                  = sequence->GetItem(1);
                const ::gdcm::DataSet& referencedSeriesItemDataset = referencedSeriesItem.GetNestedDataSet();

                // Series Instance UID - Type 1
                seriesInstanceUID =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000E>(referencedSeriesItemDataset);
            }
        }
    }

    if(!seriesInstanceUID.empty())
    {
        for(SeriesContainerMapType::value_type v : m_seriesContainerMap)
        {
            if(v.first->getSeriesInstanceUID() == seriesInstanceUID)
            {
                result = v.first;
                break;
            }
        }
    }

    return result;
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::DicomInstance) Series::getStructuredReportReferencedSeriesInstance(
    const data::DicomSeries::csptr& dicomSeries
)
{
    SPTR(io::dicom::container::DicomInstance) result;

    // Dicom container
    data::DicomSeries::DicomContainerType dicomContainer = dicomSeries->getDicomContainer();

    // Create Reader
    std::shared_ptr< ::gdcm::Reader> reader =
        std::shared_ptr< ::gdcm::Reader>(new ::gdcm::Reader);
    const core::memory::BufferObject::sptr bufferObj         = dicomContainer.begin()->second;
    const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);

    // Series Instance UID of the referenced Series
    std::string seriesInstanceUID = "";

    if(reader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet& datasetRoot = reader->GetFile().GetDataSet();

        // Pertinent Other Evidence Sequence - Type 1C
        if(datasetRoot.FindDataElement(::gdcm::Tag(0x0040, 0xa385)))
        {
            // Get the content sequence
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> sequence =
                datasetRoot.GetDataElement(::gdcm::Tag(0x0040, 0xa385)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                ::gdcm::Item studyItem                  = sequence->GetItem(1);
                const ::gdcm::DataSet& studyItemDataset = studyItem.GetNestedDataSet();

                if(studyItemDataset.FindDataElement(::gdcm::Tag(0x0008, 0x1115)))
                {
                    // Get the series sequence
                    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> seriesSequence =
                        studyItemDataset.GetDataElement(::gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

                    if(seriesSequence->GetNumberOfItems() > 0)
                    {
                        ::gdcm::Item seriesItem                  = seriesSequence->GetItem(1);
                        const ::gdcm::DataSet& seriesItemDataset = seriesItem.GetNestedDataSet();
                        seriesInstanceUID = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000E>(
                            seriesItemDataset
                        );
                    }
                }
            }
        }
    }

    if(!seriesInstanceUID.empty())
    {
        for(SeriesContainerMapType::value_type v : m_seriesContainerMap)
        {
            if(v.first->getSeriesInstanceUID() == seriesInstanceUID)
            {
                result = v.first;
                break;
            }
        }
    }

    return result;
}
//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io::dicom
