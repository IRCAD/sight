/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/Series.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/reader/iod/ComprehensiveSRIOD.hpp"
#include "fwGdcmIO/reader/iod/CTMRImageIOD.hpp"
#include "fwGdcmIO/reader/iod/SpatialFiducialsIOD.hpp"
#include "fwGdcmIO/reader/iod/SurfaceSegmentationIOD.hpp"

#include <fwData/Image.hpp>

#include <fwDicomTools/Series.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/make_shared.hpp>

#include <gdcmImageReader.h>
#include <gdcmScanner.h>

namespace fwGdcmIO
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
    SLM_TRACE_FUNC();
}

// ----------------------------------------------------------------------------

::fwMedData::Series::sptr Series::read(const ::fwMedData::DicomSeries::csptr& dicomSeries)
throw(::fwGdcmIO::exception::Failed)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Logger should not be null.", m_logger);

    // Create instance
    SPTR(::fwGdcmIO::container::DicomInstance) instance =
        std::make_shared< ::fwGdcmIO::container::DicomInstance >(dicomSeries);

    // Create result
    ::fwMedData::Series::sptr result;

    if(!dicomSeries->getLocalDicomPaths().empty())
    {
        // Get SOPClassUID
        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
        std::string sopClassUID = *sopClassUIDContainer.begin();

        // If the DicomSeries contains an image (ImageSeries)
        if (::gdcm::MediaStorage::IsImage(::gdcm::MediaStorage::GetMSType(sopClassUID.c_str())) &&
            ::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) != ::gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Read the image
            ::fwMedData::ImageSeries::sptr imageSeries = ::fwDicomTools::Series::convertToImageSeries(dicomSeries);
            ::fwData::Image::sptr image                = ::fwData::Image::New();
            imageSeries->setImage(image);

            // Create IOD Reader
            ::fwGdcmIO::reader::iod::CTMRImageIOD iod(dicomSeries, instance, m_logger,
                                                      m_progressCallback, m_cancelRequestedCallback);
            iod.setBufferRotationEnabled(m_enableBufferRotation);

            try
            {
                iod.read(imageSeries);
            }
            catch (const ::fwGdcmIO::exception::Failed& e)
            {
                // NOTE : if there is no image, reading is stopped.
                m_logger->critical(e.what());
                imageSeries = ::fwMedData::ImageSeries::sptr();
            }

            // Set result
            result = imageSeries;
        }

        // Get the RT file names (ModelSeries)
        else if (::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) ==
                 ::gdcm::MediaStorage::SurfaceSegmentationStorage)
        {
            ::fwMedData::ModelSeries::sptr modelSeries = ::fwDicomTools::Series::convertToModelSeries(dicomSeries);

            // Create IOD Reader
            ::fwGdcmIO::reader::iod::SurfaceSegmentationIOD iod(dicomSeries, instance, m_logger,
                                                                m_progressCallback, m_cancelRequestedCallback);

            try
            {
                iod.read(modelSeries);
            }
            catch (const ::fwGdcmIO::exception::Failed& e)
            {
                // NOTE : if there is no image, reading is stopped.
                m_logger->critical(e.what());
                modelSeries = ::fwMedData::ModelSeries::sptr();
            }

            // Set result
            result = modelSeries;
        }
        // If the DicomSeries contains a Spatial Fiducials
        else if (::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Retrieve referenced image instance
            SPTR(::fwGdcmIO::container::DicomInstance) imageInstance =
                this->getSpatialFiducialsReferencedSeriesInstance(dicomSeries);

            if(imageInstance)
            {
                ::fwMedData::ImageSeries::sptr imageSeries =
                    ::fwMedData::ImageSeries::dynamicCast(m_seriesContainerMap[imageInstance]);

                // Create IOD Reader
                ::fwGdcmIO::reader::iod::SpatialFiducialsIOD iod(dicomSeries, instance, m_logger,
                                                                 m_progressCallback, m_cancelRequestedCallback);

                try
                {
                    iod.read(imageSeries);
                }
                catch (const ::fwGdcmIO::exception::Failed& e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    m_logger->critical("Spatial Fiducials reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                m_logger->critical("The spatial fiducials series \"" + dicomSeries->getInstanceUID() +
                                   "\" could not be read as it refers to an unknown series UID.");
            }
        }
        // If the DicomSeries contains a SR
        else if (::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR ||
                 ::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR ||
                 sopClassUID == "1.2.840.10008.5.1.4.1.1.88.34") // FIXME Replace hard coded string by
                                                                 // "::gdcm::MediaStorage::GetMSType(sopClassUID.c_str())
                                                                 // == ::gdcm::MediaStorage::Comprehensive3DSR"
        {
            // Retrieve referenced image instance
            SPTR(::fwGdcmIO::container::DicomInstance) imageInstance =
                this->getStructuredReportReferencedSeriesInstance(dicomSeries);

            ::fwMedData::ImageSeries::sptr imageSeries =
                ::fwMedData::ImageSeries::dynamicCast(m_seriesContainerMap[imageInstance]);

            if(imageInstance && imageSeries)
            {
                // Create readers
                ::fwGdcmIO::reader::iod::ComprehensiveSRIOD iod(dicomSeries, imageInstance, m_logger,
                                                                m_progressCallback, m_cancelRequestedCallback);

                try
                {
                    iod.read(imageSeries);
                }
                catch (const ::fwGdcmIO::exception::Failed& e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    m_logger->critical("Structured Report reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                m_logger->critical("The structured report series \"" + dicomSeries->getInstanceUID() +
                                   "\" could not be read as it refers to an unknown series UID.");
            }

        }
        else
        {
            m_logger->critical("DICOM SOP Class UID \"" + sopClassUID +"\" is not supported by the selected reader.");
        }

    }

    // Store series in instance map
    m_seriesContainerMap[instance] = result;

    return result;
}

//------------------------------------------------------------------------------

SPTR(::fwGdcmIO::container::DicomInstance) Series::getSpatialFiducialsReferencedSeriesInstance(
    const ::fwMedData::DicomSeries::csptr& dicomSeries)
{
    SPTR(::fwGdcmIO::container::DicomInstance) result;

    // Path container
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();

    // Create Reader
    ::boost::shared_ptr< ::gdcm::Reader > reader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );
    reader->SetFileName( filename.c_str() );

    // Series Instance UID of the referenced Series
    std::string seriesInstanceUID = "";

    if(reader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet& datasetRoot = reader->GetFile().GetDataSet();

        if(datasetRoot.FindDataElement(::gdcm::Tag(0x0008, 0x1115)))
        {
            // Get the content sequence
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                datasetRoot.GetDataElement(::gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                ::gdcm::Item referencedSeriesItem = sequence->GetItem(1);
                const ::gdcm::DataSet& referencedSeriesItemDataset = referencedSeriesItem.GetNestedDataSet();

                // Series Instance UID - Type 1
                seriesInstanceUID =
                    ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0020, 0x000E >(referencedSeriesItemDataset);
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

SPTR(::fwGdcmIO::container::DicomInstance) Series::getStructuredReportReferencedSeriesInstance(
    const ::fwMedData::DicomSeries::csptr& dicomSeries)
{

    SPTR(::fwGdcmIO::container::DicomInstance) result;

    // Path container
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();

    // Create Reader
    ::boost::shared_ptr< ::gdcm::Reader > reader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );
    reader->SetFileName( filename.c_str() );

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
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                datasetRoot.GetDataElement(::gdcm::Tag(0x0040, 0xa385)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                ::gdcm::Item studyItem = sequence->GetItem(1);
                const ::gdcm::DataSet& studyItemDataset = studyItem.GetNestedDataSet();

                if(studyItemDataset.FindDataElement(::gdcm::Tag(0x0008, 0x1115)))
                {
                    // Get the series sequence
                    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > seriesSequence =
                        studyItemDataset.GetDataElement(::gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

                    if(seriesSequence->GetNumberOfItems() > 0)
                    {
                        ::gdcm::Item seriesItem = seriesSequence->GetItem(1);
                        const ::gdcm::DataSet& seriesItemDataset = seriesItem.GetNestedDataSet();
                        seriesInstanceUID = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0020, 0x000E >(
                            seriesItemDataset);
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

}  // namespace reader
}  // namespace fwGdcmIO
