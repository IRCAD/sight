/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/make_shared.hpp>

#include <gdcmImageReader.h>
#include <gdcmScanner.h>

#include <fwTools/dateAndTime.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwDicomIOExt/dcmtk/helper/Series.hpp>

#include <fwData/Image.hpp>

#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/reader/Series.hpp"

#include "gdcmIO/reader/iod/CTMRImageIOD.hpp"
#include "gdcmIO/reader/iod/SurfaceSegmentationIOD.hpp"
#include "gdcmIO/reader/iod/SpatialFiducialsIOD.hpp"
#include "gdcmIO/reader/iod/ComprehensiveSRIOD.hpp"

namespace gdcmIO
{
namespace reader
{

//------------------------------------------------------------------------------

Series::Series()
{

}

//------------------------------------------------------------------------------

Series::~Series()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::fwMedData::Series::sptr Series::read(::fwDicomData::DicomSeries::sptr dicomSeries)
throw(::gdcmIO::exception::Failed)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);

    // Create instance
    SPTR(::gdcmIO::container::DicomInstance) instance =
        ::boost::make_shared< ::gdcmIO::container::DicomInstance >(dicomSeries);

    // Create result
    ::fwMedData::Series::sptr result;

    if(!dicomSeries->getLocalDicomPaths().empty())
    {
        // Get SOPClassUID
        ::fwDicomData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
        std::string sopClassUID = *sopClassUIDContainer.begin();

        // If the DicomSeries contains an image (ImageSeries)
        if (::gdcm::MediaStorage::IsImage(::gdcm::MediaStorage::GetMSType(sopClassUID.c_str())) &&
            ::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) != ::gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Read the image
            ::fwMedData::ImageSeries::sptr imageSeries =
                ::fwDicomIOExt::dcmtk::helper::Series::convertToImageSeries(dicomSeries);
            ::fwData::Image::sptr image = ::fwData::Image::New();
            imageSeries->setImage(image);

            // Create IOD Reader
            ::gdcmIO::reader::iod::CTMRImageIOD iod(dicomSeries, instance);

            try
            {
                iod.read(imageSeries);
            }
            catch (const ::gdcmIO::exception::Failed & e)
            {
                // NOTE : if there is no image, reading is stopped.
                SLM_ERROR(e.what());
                throw;
            }

            // Set result
            result = imageSeries;
        }

        // Get the RT file names (ModelSeries)
        else if (::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) ==
                 ::gdcm::MediaStorage::SurfaceSegmentationStorage)
        {
            ::fwMedData::ModelSeries::sptr modelSeries =
                ::fwDicomIOExt::dcmtk::helper::Series::convertToModelSeries(dicomSeries);

            // Create IOD Reader
            ::gdcmIO::reader::iod::SurfaceSegmentationIOD iod(dicomSeries, instance);

            try
            {
                iod.read(modelSeries);
            }
            catch (const ::gdcmIO::exception::Failed & e)
            {
                // NOTE : if there is no image, reading is stopped.
                SLM_ERROR(e.what());
                throw;
            }

            // Set result
            result = modelSeries;
        }
        // If the DicomSeries contains a Spatial Fiducials
        else if (::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage)
        {
            // Retrieve referenced image instance
            SPTR(::gdcmIO::container::DicomInstance) imageInstance =
                this->getSpatialFiducialsReferencedSeriesInstance(dicomSeries);

            if(imageInstance)
            {
                ::fwMedData::ImageSeries::sptr imageSeries =
                    ::fwMedData::ImageSeries::dynamicCast(m_seriesContainerMap[imageInstance]);

                // Create IOD Reader
                ::gdcmIO::reader::iod::SpatialFiducialsIOD iod(dicomSeries, instance);

                try
                {
                    iod.read(imageSeries);
                }
                catch (const ::gdcmIO::exception::Failed & e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    SLM_ERROR("Spatial Fiducials reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                SLM_WARN("Spatial Fiducials refers to a series that has not been read.");
            }
        }
        // If the DicomSeries contains a SR
        else if (::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR ||
                 ::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR ||
                 sopClassUID == "1.2.840.10008.5.1.4.1.1.88.34") // FIXME Replace hard coded string by "::gdcm::MediaStorage::GetMSType(sopClassUID.c_str()) == ::gdcm::MediaStorage::Comprehensive3DSR"
        {
            // Retrieve referenced image instance
            SPTR(::gdcmIO::container::DicomInstance) imageInstance =
                this->getStructuredReportReferencedSeriesInstance(dicomSeries);

            ::fwMedData::ImageSeries::sptr imageSeries =
                ::fwMedData::ImageSeries::dynamicCast(m_seriesContainerMap[imageInstance]);

            if(imageInstance && imageSeries)
            {


                // Create readers
                ::gdcmIO::reader::iod::ComprehensiveSRIOD comprehensiveSRIOD(dicomSeries, imageInstance);

                try
                {
                    comprehensiveSRIOD.read(imageSeries);
                }
                catch (const ::gdcmIO::exception::Failed & e)
                {
                    //NOTE: no throw for reading error for SR and RT doc
                    SLM_ERROR("Structured Report reading failed: " + std::string(e.what()));
                }
            }
            else
            {
                SLM_WARN("Structured Report refers to a series that has not been read.");
            }


        }

    }

    // Store series in instance map
    m_seriesContainerMap[instance] = result;

    return result;
}

//------------------------------------------------------------------------------

SPTR(::gdcmIO::container::DicomInstance) Series::getSpatialFiducialsReferencedSeriesInstance(
    ::fwDicomData::DicomSeries::sptr dicomSeries)
{
    SPTR(::gdcmIO::container::DicomInstance) result;

    // Path container
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();

    // Create Reader
    ::boost::shared_ptr< ::gdcm::Reader > reader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );
    reader->SetFileName( filename.c_str() );

    // Series Instance UID of the referenced Series
    std::string seriesInstanceUID = "";

    if(reader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet &datasetRoot = reader->GetFile().GetDataSet();

        if(datasetRoot.FindDataElement(::gdcm::Tag(0x0008, 0x1115)))
        {
            // Get the content sequence
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                datasetRoot.GetDataElement(::gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                ::gdcm::Item referencedSeriesItem            = sequence->GetItem(1);
                ::gdcm::DataSet &referencedSeriesItemDataset = referencedSeriesItem.GetNestedDataSet();

                // Series Instance UID - Type 1
                seriesInstanceUID =
                    ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0020, 0x000E >(referencedSeriesItemDataset);
            }
        }
    }

    if(!seriesInstanceUID.empty())
    {
        BOOST_FOREACH(SeriesContainerMapType::value_type v, m_seriesContainerMap)
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

SPTR(::gdcmIO::container::DicomInstance) Series::getStructuredReportReferencedSeriesInstance(
    ::fwDicomData::DicomSeries::sptr dicomSeries)
{

    SPTR(::gdcmIO::container::DicomInstance) result;

    // Path container
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();

    // Create Reader
    ::boost::shared_ptr< ::gdcm::Reader > reader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );
    reader->SetFileName( filename.c_str() );

    // Series Instance UID of the referenced Series
    std::string seriesInstanceUID = "";

    if(reader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet &datasetRoot = reader->GetFile().GetDataSet();

        // Pertinent Other Evidence Sequence - Type 1C
        if(datasetRoot.FindDataElement(::gdcm::Tag(0x0040, 0xa385)))
        {
            // Get the content sequence
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                datasetRoot.GetDataElement(::gdcm::Tag(0x0040, 0xa385)).GetValueAsSQ();

            if(sequence->GetNumberOfItems() > 0)
            {
                ::gdcm::Item studyItem            = sequence->GetItem(1);
                ::gdcm::DataSet &studyItemDataset = studyItem.GetNestedDataSet();

                if(studyItemDataset.FindDataElement(::gdcm::Tag(0x0008, 0x1115)))
                {
                    // Get the series sequence
                    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > seriesSequence =
                        studyItemDataset.GetDataElement(::gdcm::Tag(0x0008, 0x1115)).GetValueAsSQ();

                    if(seriesSequence->GetNumberOfItems() > 0)
                    {
                        ::gdcm::Item seriesItem            = seriesSequence->GetItem(1);
                        ::gdcm::DataSet &seriesItemDataset = seriesItem.GetNestedDataSet();
                        seriesInstanceUID                  =
                            ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0020, 0x000E >(seriesItemDataset);
                    }
                }

            }
        }
    }

    if(!seriesInstanceUID.empty())
    {
        BOOST_FOREACH(SeriesContainerMapType::value_type v, m_seriesContainerMap)
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

}  // namespace reader
}  // namespace gdcmIO

