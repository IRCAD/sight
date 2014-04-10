/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/helper/DicomSR.hpp"

#include "gdcmIO/reader/iod/EnhancedSRLandmarkIOD.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

EnhancedSRLandmarkIOD::EnhancedSRLandmarkIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
    // Instantiates a specific reader
    m_gdcmReader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Create landmark container
    this->m_dicomLandmarks =
            ::boost::shared_ptr< ::gdcmIO::container::DicomLandmark >( new ::gdcmIO::container::DicomLandmark );
}

//------------------------------------------------------------------------------

EnhancedSRLandmarkIOD::~EnhancedSRLandmarkIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void EnhancedSRLandmarkIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    // Retrieve images
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("ImageSeries should not be null.", imageSeries);
    ::fwData::Image::sptr image = imageSeries->getImage();
    SLM_ASSERT("::fwData::Image not instanced", image);

    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    OSLM_WARN_IF("More than one SR file was found in series. Only the first one will be read.",
            pathContainer.size() >1);

    // Read the first file
    m_gdcmReader->SetFileName( pathContainer.begin()->second.string().c_str() );
    if (m_gdcmReader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet & datasetRoot = m_gdcmReader->GetFile().GetDataSet();

        // Retrieve ValueType tag
        const std::string type = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0040,0xa040>(datasetRoot);
        if (type == "CONTAINER")
        {
            // Retrieve ContinuityOfContent tag
            std::string continuityStr  = ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0040,0xa050>(datasetRoot);
            bool continuity = (continuityStr == "CONTINUOUS");

            // Search child in the root of the document SR
            // Retrieve ContentSequence tag
            ::gdcm::Tag contentSequenceTag(0x0040,0xa730);
            if (datasetRoot.FindDataElement( contentSequenceTag ))
            {
                const ::gdcm::DataElement &gdcmLandmarksDataElement = datasetRoot.GetDataElement( contentSequenceTag );
                const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gdcmLandmarksSequence =
                        gdcmLandmarksDataElement.GetValueAsSQ();

                // Search and read landmark(s) in children of the document SR root
                ::gdcm::SequenceOfItems::ConstIterator it;
                for (it = gdcmLandmarksSequence->Begin(); it != gdcmLandmarksSequence->End(); ++it)
                {
                    try
                    {
                        // Try to read a child as a landmark
                        // Read one landmark and add it to m_dicomLandmark
                        this->readLandmark(image, it->GetNestedDataSet(), continuity);
                    }
                    catch(::gdcmIO::exception::Failed & e)
                    {
                        SLM_ERROR("Structured reporting reading error : " + std::string(e.what()));
                    }
                }
                // Add landmarks to fwData::Image
                this->m_dicomLandmarks->convertToData(image);
            }
            else
            {
                FW_RAISE_EXCEPTION(::gdcmIO::exception::Failed("Empty structured reporting container"));
            }
        }
    }
}

//------------------------------------------------------------------------------

void EnhancedSRLandmarkIOD::readLandmark(::fwData::Image::sptr image, const ::gdcm::DataSet & dataset,
        const bool continuity) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Continuous content not handled"), continuity);

    // Initialization
    std::string landmarkLabel = "";
    std::string referencedFrame = "";
    ::gdcmIO::container::DicomSCoord landmarkCoordinates;
    const std::string separator = ",";  // Separator of binary content

    // Retrieve landmark label using TextValue tag
    const ::gdcm::DataSet * textDataset = ::gdcmIO::helper::DicomSR::getCodedContainer("121071", dataset);

    if(textDataset == 0)
    {
        // This container is not a landmark
        return;
    }

    landmarkLabel = ::gdcmIO::helper::DicomData::getTagValue<0x0040,0xa160>(*textDataset);

    // Retrieve landmark coordinates
    const ::gdcm::DataSet * coordinatesDataset = ::gdcmIO::helper::DicomSR::getTypedContainer("SCOORD", *textDataset);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("No point found"), coordinatesDataset == 0);
    landmarkCoordinates = ::gdcmIO::helper::DicomSR::readSCOORD(*coordinatesDataset);

    // Check that the graphic type of the coordinate is a point
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Graphic Type '"+landmarkCoordinates.getCRefGraphicType()
            +"' not found or unappropriated"),
                landmarkCoordinates.getCRefGraphicType() != "POINT");
    // Check SCOORD comply with one point of a landmark
    const unsigned int nbCoords = landmarkCoordinates.getGraphicDataSize();
    if ( nbCoords != 2 )
    {
        throw ::gdcmIO::exception::Failed("Graphic data are corrupted.");
    }

    // Retrieve landmark image
    const ::gdcm::DataSet * imageDataset = ::gdcmIO::helper::DicomSR::getTypedContainer("IMAGE", *coordinatesDataset);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced Type not found or unappropriated"),
            imageDataset == 0);

    // Retrieve ReferencedSOPSequence tag
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced sequence not found"),
            !imageDataset->FindDataElement( ::gdcm::Tag(0x0008,0x1199) ));

    // Referenced SOP Sequence Data Element
    const ::gdcm::DataElement & gdcmReferencedSOPSequenceDataElement =
            imageDataset->GetDataElement( ::gdcm::Tag(0x0008,0x1199) );
    // Referenced SOP Sequence
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gdcmReferencedSOPSequence =
            gdcmReferencedSOPSequenceDataElement.GetValueAsSQ();

    // Check that the sequence is not empty
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced sequence empty"),
            gdcmReferencedSOPSequence->GetNumberOfItems() <= 0);

    // Get first item in sequence
    const ::gdcm::Item &gdcmReferencedSOPItem = gdcmReferencedSOPSequence->GetItem(1);

    // Get dataset of the first item
    const ::gdcm::DataSet &gdcmReferencedSOPItemDataset = gdcmReferencedSOPItem.GetNestedDataSet();

    // For multiframe image, we search one indexes of frame
    if (!m_instance->getIsMultiFiles())
    {
        // Retrieve ReferencedFrameNumber tag (Type 1C)
        referencedFrame   = ::gdcmIO::helper::DicomData::getTagValue<0x0008,0x1160>(gdcmReferencedSOPItemDataset);

        // If referenced frames number are not found
        if (referencedFrame.empty())
        {
            referencedFrame = "1"; // First frame is referenced for 2D image

            if (image->getNumberOfDimensions() > 2)
            {
                // All frames are referenced for 3D image
                const unsigned int nbFrames = image->getSize()[2];
                for (unsigned int i = 2; i <= nbFrames; ++i)
                {
                    referencedFrame += separator + ::fwTools::getString(i);
                }
            }
        }
    }
    else
    {
        // Retrieve ReferencedSOPInstanceUID tag (Type 1)
        const std::string referencedSOPInstanceUID =
                ::gdcmIO::helper::DicomData::getTagValue<0x0008,0x1155>(gdcmReferencedSOPItemDataset);
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced image is not defined."),
                referencedSOPInstanceUID.empty());

        const std::vector< std::string > & referencedSOPInstanceUIDs = m_instance->getCRefSOPInstanceUIDContainer();

        try
        {
            // Try to translate the SOP instance UID into the corresponding frame number
            // TODO: Check if it shouldn't be 0 for first frame
            referencedFrame = ::fwTools::getString(::gdcmIO::helper::DicomSR::instanceUIDToFrameNumber(
                    referencedSOPInstanceUID, referencedSOPInstanceUIDs) );
        }
        catch(::gdcmIO::exception::Failed & e)
        {
            throw;
        }
    }


    // Add one landmark
    if (!continuity)
    {
        // if separate content
        this->m_dicomLandmarks->addLandmark(landmarkLabel, landmarkCoordinates,
                ::boost::lexical_cast<int>(referencedFrame.c_str()));
    }
}

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

