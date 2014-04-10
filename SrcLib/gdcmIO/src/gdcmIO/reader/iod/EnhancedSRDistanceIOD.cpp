/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/helper/DicomSR.hpp"

#include "gdcmIO/reader/iod/EnhancedSRDistanceIOD.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

EnhancedSRDistanceIOD::EnhancedSRDistanceIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
    // Instantiates a specific reader
    m_gdcmReader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Create distance container
    this->m_dicomDistances =
            ::boost::shared_ptr< ::gdcmIO::container::DicomDistance>( new ::gdcmIO::container::DicomDistance );
}

//------------------------------------------------------------------------------

EnhancedSRDistanceIOD::~EnhancedSRDistanceIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void EnhancedSRDistanceIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    // Retrieve images
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("ImageSeries should not be null.", imageSeries);
    ::fwData::Image::sptr image = imageSeries->getImage();
    SLM_ASSERT("::fwData::Image not instanced", image);

    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    OSLM_WARN_IF("More than one SR file was found in series. Only the first one will be read.", pathContainer.size() >1);

    // Read the first file
    m_gdcmReader->SetFileName( pathContainer.begin()->second.string().c_str() );
    if (m_gdcmReader->Read())
    {
        // Retrieve dataset
        const ::gdcm::DataSet & datasetRoot = m_gdcmReader->GetFile().GetDataSet();

        // Retrieve ValueType tag
        if (helper::DicomData::getTrimmedTagValue<0x0040,0xa040>(datasetRoot) == "CONTAINER")
        {
            // Retrieve ContinuityOfContent tag
            std::string continuityStr  =::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0040,0xa050>(datasetRoot);
            bool continuity = ( continuityStr == "CONTINUOUS");

            // Search child in the root of the document SR
            // Retrieve ContentSequence tag
            ::gdcm::Tag contentSequenceTag(0x0040,0xa730);
            if (datasetRoot.FindDataElement( contentSequenceTag ))
            {
                const ::gdcm::DataElement &gdcmDistanceDataElement = datasetRoot.GetDataElement( contentSequenceTag );
                const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gdcmDistanceSequence =
                        gdcmDistanceDataElement.GetValueAsSQ();

                ::gdcm::SequenceOfItems::ConstIterator it;
                // For each child
                for (it = gdcmDistanceSequence->Begin(); it != gdcmDistanceSequence->End(); ++it)
                {
                    try
                    {
                        // Try to read a child as a distance
                        // Read one distance and add it to m_dicomDistance
                        this->readDistance(image, it->GetNestedDataSet(), continuity);
                    }
                    catch(::gdcmIO::exception::Failed & e)
                    {
                        OSLM_ERROR("Structured reporting reading error : "<<e.what());
                    }
                }
                // Add distances to fwData::Image
                this->m_dicomDistances->convertToData(image);
            }
            else
            {
                FW_RAISE_EXCEPTION(::gdcmIO::exception::Failed("Empty structured reporting container"));
            }
        }
    }
}

//------------------------------------------------------------------------------

void EnhancedSRDistanceIOD::readDistance(::fwData::Image::sptr image, const ::gdcm::DataSet &  dataset,
        const bool continuity) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Contiguous content not handled"), continuity );

    // Initialization
    std::string referencedFrame = "";   // Referenced frame where the SCOORD is placed
    ::gdcmIO::container::DicomSCoord distanceCoordinates1;
    ::gdcmIO::container::DicomSCoord distanceCoordinates2;
    const std::string separator = ",";  // Separator of binary content

    // Retrieve distance
    const ::gdcm::DataSet* distanceDataset =::gdcmIO::helper::DicomSR::getCodedContainer("121206", dataset);
    if(distanceDataset == 0)
    {
        // This container is not a distance
        return;
    }

    // Retrieve coordinates
    const ::gdcm::DataSet* coordinatesDataset1 =
            ::gdcmIO::helper::DicomSR::getTypedContainer("SCOORD", *distanceDataset, 0);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("First SCOORD not found"),  coordinatesDataset1 == 0 );
    distanceCoordinates1 =::gdcmIO::helper::DicomSR::readSCOORD(*coordinatesDataset1);

    try
    {
        // Check SCOORD comply with one point of a distance
        this->checkSCOORDContent(distanceCoordinates1);
    }
    catch(::gdcmIO::exception::Failed &)
    {
        throw;
    }

    // Retrieve distance image
    const ::gdcm::DataSet * imageDataset1 =::gdcmIO::helper::DicomSR::getTypedContainer("IMAGE", *coordinatesDataset1);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced Type not found or unappropriated"),
            imageDataset1 == 0);

    // Retrieve ReferencedSOPSequence tag
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced SOP sequence not found"),
            !imageDataset1->FindDataElement( ::gdcm::Tag(0x0008,0x1199) ));

    // Referenced SOP Sequence Data Element
    const ::gdcm::DataElement &gdcmReferencedSOPSequenceDataElement =
            imageDataset1->GetDataElement( ::gdcm::Tag(0x0008,0x1199) );
    // Referenced SOP Sequence
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gdcmReferencedSOPSequence1 =
            gdcmReferencedSOPSequenceDataElement.GetValueAsSQ();

    // Check that the sequence is not empty
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced SOP sequence empty"),
            gdcmReferencedSOPSequence1->GetNumberOfItems() < 1);

    // Get first item in sequence
    const ::gdcm::Item &gdcmReferencedSOPItem1 = gdcmReferencedSOPSequence1->GetItem(1);

    // Get dataset of the first item
    const ::gdcm::DataSet &gdcmReferencedSOPItemDataset1 = gdcmReferencedSOPItem1.GetNestedDataSet();

    // For multiframe image, we search two indexes of frames
    if (!m_instance->getIsMultiFiles())
    {
        // Referenced Frame Number (Type 1C)
        referencedFrame =::gdcmIO::helper::DicomData::getTagValue<0x0008,0x1160>(gdcmReferencedSOPItemDataset1);

        // If referenced frames number are not found, this means that all frames are referenced
        if (referencedFrame.empty())
        {
            // WARNING : the idea is that the distance is placed on the same frame
            referencedFrame = "1" + separator + "1";  // First frame is referenced for 2D image TODO: Check if it shouldn't be 0 for first frame

            if (image->getNumberOfDimensions() > 2)
            {
                // All frames are referenced for 3D image
                const unsigned int nbFrames = image->getSize()[2];
                std::string numFrame = "";
                for (unsigned int i = 2; i <= nbFrames; ++i)
                {
                    // Is it the right choice for distance?
                    numFrame = ::fwTools::getString(i);
                    referencedFrame += separator + numFrame + separator + numFrame;
                }
            }
        }
    }
    else
    {
        // For non multiframe image, we search two SOP Instance UID which reference two DICOM file (each file contains one frame)
        // Retrieve second coordinates
        const ::gdcm::DataSet* coordinatesDataset2 =::gdcmIO::helper::DicomSR::getTypedContainer("SCOORD", *distanceDataset, 1);
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("First SCOORD not found"),  coordinatesDataset2 == 0 );
        distanceCoordinates2 =::gdcmIO::helper::DicomSR::readSCOORD(*coordinatesDataset2);

        try
        {
            // Check SCOORD comply with one point of a distance
            this->checkSCOORDContent(distanceCoordinates2);
        }
        catch(::gdcmIO::exception::Failed &)
        {
            throw;
        }

        // Merge the second SCOORD with the first one
        float graphicData[4];
        graphicData[0] = distanceCoordinates1[0];
        graphicData[1] = distanceCoordinates1[1];
        graphicData[2] = distanceCoordinates2[0];
        graphicData[3] = distanceCoordinates2[1];
        distanceCoordinates1.setGraphicData(graphicData, 4);
        distanceCoordinates1.setGraphicType("POLYLINE");


        // Retrieve distance image
        const ::gdcm::DataSet *imageDataset2 =::gdcmIO::helper::DicomSR::getTypedContainer("IMAGE", *coordinatesDataset2);
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced Type not found or unappropriated"), imageDataset2 == 0);

        // Retrieve ReferencedSOPSequence tag
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced SOP sequence not found"),
                !imageDataset2->FindDataElement( ::gdcm::Tag(0x0008,0x1199) ));

        // Referenced SOP Sequence Data Element
        const ::gdcm::DataElement &gdcmReferencedSOPSequenceDataElement =
                imageDataset2->GetDataElement( ::gdcm::Tag(0x0008,0x1199) );
        // Referenced SOP Sequence
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gdcmReferencedSOPSequence2 =
                gdcmReferencedSOPSequenceDataElement.GetValueAsSQ();

        // Check that the sequence is not empty
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced SOP sequence empty"),
                gdcmReferencedSOPSequence2->GetNumberOfItems() < 1);

        // Get first item in sequence
        const ::gdcm::Item &gdcmReferencedSOPItem2 = gdcmReferencedSOPSequence2->GetItem(1);

        // Get dataset of the first item
        const ::gdcm::DataSet &gdcmReferencedSOPItemDataset2 = gdcmReferencedSOPItem2.GetNestedDataSet();

        // Retrieve ReferencedSOPInstanceUID tags (Type 1)
        const std::string referencedSOPInstanceUID1 =
                ::gdcmIO::helper::DicomData::getTagValue<0x0008,0x1155>(gdcmReferencedSOPItemDataset1);
        FW_RAISE_EXCEPTION_IF(
                ::gdcmIO::exception::Failed("Referenced image is not defined"), referencedSOPInstanceUID1.empty());
        const std::string referencedSOPInstanceUID2 =
                ::gdcmIO::helper::DicomData::getTagValue<0x0008,0x1155>(gdcmReferencedSOPItemDataset2);
        FW_RAISE_EXCEPTION_IF(
                ::gdcmIO::exception::Failed("Referenced image is not defined"), referencedSOPInstanceUID2.empty());

        // Get the SOP Instance UIDs of the referenced DICOM image previously read
        const std::vector< std::string > &referencedSOPInstanceUIDs = m_instance->getCRefSOPInstanceUIDContainer();
        try
        {
            // Try to translate the SOP instance UID into the corresponding frame number
            // TODO: Check if it shouldn't be 0 for first frame
            referencedFrame =
                    ::fwTools::getString(::gdcmIO::helper::DicomSR::instanceUIDToFrameNumber(
                            referencedSOPInstanceUID1, referencedSOPInstanceUIDs) ) + separator
                    + ::fwTools::getString(::gdcmIO::helper::DicomSR::instanceUIDToFrameNumber(
                            referencedSOPInstanceUID2, referencedSOPInstanceUIDs) );
        }
        catch(::gdcmIO::exception::Failed &)
        {
            throw;
        }

    }

    // Add the distance to be convert and set after
    if ( !continuity )
    {
        // if separate content
        this->m_dicomDistances->addDistance( "", distanceCoordinates1, referencedFrame);
    }
}

//------------------------------------------------------------------------------

void EnhancedSRDistanceIOD::checkSCOORDContent(const ::gdcmIO::container::DicomSCoord & scoord)
throw (::gdcmIO::exception::Failed)
{
    // Check the SCOORD
    const std::string &graphicType = scoord.getCRefGraphicType();
    const unsigned int nbCoords = scoord.getGraphicDataSize();

    // First, check the graphic type
    if ( graphicType == "POINT")
    {
        // Then, check the number of coordinates in the graphic data
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Graphic data are corrupted."), nbCoords != 2);
    }
    else if(graphicType == "POLYLINE" || graphicType == "MULTIPOINT")
    {
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Graphic data are corrupted."),
                (nbCoords < 4) && (nbCoords%2 == 0));
    }
    else
    {
        FW_RAISE_EXCEPTION(::gdcmIO::exception::Failed("Graphic Type unappropriated"));
    }
}

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

