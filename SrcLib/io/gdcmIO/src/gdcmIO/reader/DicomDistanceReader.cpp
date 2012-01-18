/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// fwComEd
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

// fwData
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include "gdcmIO/reader/DicomDistanceReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/DicomDictionarySR.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomDistanceReader::DicomDistanceReader()
{
    SLM_TRACE_FUNC();

    this->m_dicomDistances = ::boost::shared_ptr<DicomDistance>( new DicomDistance );
}

//------------------------------------------------------------------------------

DicomDistanceReader::~DicomDistanceReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomDistanceReader::read() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    SLM_ASSERT("gdcm::Reader not instanced", this->getReader());

    const ::gdcm::DataSet & gDsRoot = this->getDataSet();

    if (helper::GdcmData::getTagValue<0x0040,0xa040>(gDsRoot) == DicomDictionarySR::getTypeString(DicomDictionarySR::CONTAINER))
    {
        // Define the continuity of content
        bool continuity = false;
        const std::string continuityStr = helper::GdcmData::getTagValue<0x0040,0xa050>(gDsRoot);   // Continuity Of Content
        if ( continuityStr == DicomDictionarySR::getContinuityString(DicomDictionarySR::CONTINUOUS) )
        {
            continuity = true;
        }

        // Search child in the root of the document SR
        if (gDsRoot.FindDataElement( ::gdcm::Tag(0x0040,0xa730) ))        // Content Sequence
        {
            const ::gdcm::DataElement &     gDeLandmarks    = gDsRoot.GetDataElement( ::gdcm::Tag(0x0040,0xa730) );
            const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqLandmarks    = gDeLandmarks.GetValueAsSQ();

            ::gdcm::SequenceOfItems::ConstIterator it       = gSqLandmarks->Begin();
            ::gdcm::SequenceOfItems::ConstIterator itEnd    = gSqLandmarks->End();
            // For each child
            for (; it != itEnd; ++it)
            {
                try
                {// Try to read a child as a distance
                    // Read one distance and add it to m_dicomDistance
                    this->readDistance(it->GetNestedDataSet(), continuity);
                }
                catch(::fwTools::Failed & e)
                {
                    OSLM_ERROR("Structured reporting reading error : "<<e.what());
                }
            }
            // Add distances to fwData::Image
            this->m_dicomDistances->convertToData(image);
        }
        else
        {
            throw ::fwTools::Failed("Empty structured reporting container");
        }
    }
    // else nothing because root of SR data set is always a container
}

//------------------------------------------------------------------------------

void DicomDistanceReader::readDistance(const ::gdcm::DataSet &  a_ds,
                                       const bool               a_continuity/*= false*/) throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if ( a_continuity )
    {// if contiguous content
        throw ::fwTools::Failed("Contiguous content not handled");
    }

    // Initialization
    SCoord              scoord;             // Spatial COORDinates of a frame point
    std::string         refFrames   = "";   // Referenced frame where the SCOORD is placed
    const std::string   separator   = ",";  // Separator of binary content

    //**        Get a distance      **//
    const ::gdcm::DataSet * gDsDistance = helper::DicomSR::getCodedContainer(DicomDictionarySR::getCodeValue(DicomDictionarySR::DISTANCE),
                                                                             a_ds);
    if ( gDsDistance == 0 )
        throw ::fwTools::Failed("No distance found");

    //**        Get SCOORD          **//
    const ::gdcm::DataSet * gDsSCOORD = helper::DicomSR::getTypedContainer(DicomDictionarySR::getTypeString(DicomDictionarySR::SCOORD),
                                                                           *gDsDistance);
    if ( gDsSCOORD == 0 )
        throw ::fwTools::Failed("SCOORD not found");

    scoord = helper::DicomSR::readSCOORD(*gDsSCOORD);
    try
    {// Check SCOORD comply with one point of a distance
        this->checkSCOORDContent(scoord);
    }
    catch(::fwTools::Failed & e)
    {
        throw;
    }

    //**        Get IMAGE       **//
    const ::gdcm::DataSet * gDsIMAGE = helper::DicomSR::getTypedContainer(DicomDictionarySR::getTypeString(DicomDictionarySR::IMAGE),
                                                                          *gDsSCOORD);
    if ( gDsIMAGE == 0 )
        throw ::fwTools::Failed("Referenced Type not found or unappropriated");

    // Search referenced image sequence
    const ::gdcm::Tag       referencedSOPSQTag(0x0008,0x1199);    // Referenced SOP Sequence
    if ( !gDsIMAGE->FindDataElement( referencedSOPSQTag ) )
        throw ::fwTools::Failed("Referenced SOP sequence not found");

    const ::gdcm::DataElement &                             gDeRefSeq = gDsIMAGE->GetDataElement( referencedSOPSQTag );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >   gSqRefSeq = gDeRefSeq.GetValueAsSQ();

    if ( gSqRefSeq->GetNumberOfItems() < 1 )
        throw ::fwTools::Failed("Referenced SOP sequence empty");

    const ::gdcm::Item &    gItRefSeq   = gSqRefSeq->GetItem(1);
    const ::gdcm::DataSet & gDsRefSeq   = gItRefSeq.GetNestedDataSet();

    const ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();
    // For multiframe image, we search two indexes of frames
    if (dicomInstance->getIsMultiFrame())
    {
        // Referenced Frame Number
        refFrames = helper::GdcmData::getTagValue<0x0008,0x1160>(gDsRefSeq);

        // If referenced frames number are not found, this means that all frames are referenced
        if (refFrames.empty())
        {
            // WARNING : the idea is that the distance is placed on the same frame
            refFrames = "1" + separator + "1";  // First frame is referenced for 2D image

            ::fwData::Image::csptr   image = this->getConcreteObject();
            if (image->getDimension() > 2)
            {// All frames are referenced for 3D image
                const unsigned int  nbFrames = image->getSize()[2];
                std::string         numFrame("");
                for (unsigned int i = 2; i <= nbFrames; ++i)
                {   // Is it the right choice for distance?
                    numFrame = ::fwTools::getString(i);
                    refFrames += separator + numFrame + separator + numFrame;
                }
            }
        }
    }
    else
    {// For non multiframe image, we search two SOP Instance UID which reference two DICOM file (each file contains one frame)
        //*****     Get first IMAGE     *****//
        // Referenced SOP Instance UID
        const std::string                   SOPInstanceUID              = helper::GdcmData::getTagValue<0x0008,0x1155>(gDsRefSeq);   // Type 1
        if (SOPInstanceUID.empty())
            throw ::fwTools::Failed("Referenced image empty");

        // Get the SOP Instance UIDs of the referenced DICOM image previously read
        const std::vector< std::string > &  referencedSOPInstanceUIDs   = dicomInstance->getCRefReferencedSOPInstanceUIDs();
        try
        {// Try to translate the SOP instance UID into the corresponding frame number
            refFrames = ::fwTools::getString( helper::DicomSR::instanceUIDToFrameNumber(SOPInstanceUID, referencedSOPInstanceUIDs) );
        }
        catch(::fwTools::Failed & e)
        {
            throw;
        }

        //*****     Get second SCOORD and IMAGE     *****//
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > contentDistanceSQ = gDsDistance->GetDataElement( ::gdcm::Tag(0x0040,0xa730) ).GetValueAsSQ();
        if (contentDistanceSQ->GetNumberOfItems() < 2)
            throw ::fwTools::Failed("Second SCOORD not found");
        const ::gdcm::DataSet & gDsSCOORD2 = contentDistanceSQ->GetItem(2).GetNestedDataSet();

        // Get second SCOORD
        SCoord scoord2 = helper::DicomSR::readSCOORD(gDsSCOORD2);
        try
        {// Check SCOORD comply with one point of a distance
            this->checkSCOORDContent(scoord2);
        }
        catch(::fwTools::Failed & e)
        {
            throw;
        }
        // Merge the second SCOORD with the first one
        float graphicData[4];
        graphicData[0] = scoord[0]; graphicData[1] = scoord[1]; graphicData[2] = scoord2[0]; graphicData[3] = scoord2[1];
        scoord.setGraphicData(graphicData, 4);
        scoord.setGraphicType(DicomDictionarySR::POLYLINE);

        // Get second IMAGE
        const ::gdcm::DataSet * gDsIMAGE2 = helper::DicomSR::getTypedContainer(DicomDictionarySR::getTypeString(DicomDictionarySR::IMAGE),
                                                                               gDsSCOORD2);
        if ( gDsIMAGE2 == 0 )
            throw ::fwTools::Failed("Referenced Type not found or unappropriated");

        if ( !gDsIMAGE2->FindDataElement( referencedSOPSQTag ) )
            throw ::fwTools::Failed("Referenced sequence not found");

        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefSeq2 = gDsIMAGE2->GetDataElement( referencedSOPSQTag ).GetValueAsSQ();

        if ( gSqRefSeq2->GetNumberOfItems() < 1 )
            throw ::fwTools::Failed("Referenced sequence empty");

        const ::gdcm::DataSet & gDsRefSeq2      = gSqRefSeq2->GetItem(1).GetNestedDataSet();

        // Referenced SOP Instance UID
        const std::string       SOPInstanceUID2 = helper::GdcmData::getTagValue<0x0008,0x1155>(gDsRefSeq2);   // Type 1
        if (SOPInstanceUID.empty())
            throw ::fwTools::Failed("Referenced image empty");
        try
        {
            // Merge second frame number with the first one
            refFrames += separator + ::fwTools::getString( helper::DicomSR::instanceUIDToFrameNumber(SOPInstanceUID2, referencedSOPInstanceUIDs) );
        }
        catch(::fwTools::Failed & e)
        {
            throw;
        }
    }

    // Add the distance to be convert and set after
    if ( !a_continuity )
    {// if separate content
        this->m_dicomDistances->addSCoord( scoord );
        this->m_dicomDistances->addRefFrame( refFrames );
    }
}

//------------------------------------------------------------------------------

void DicomDistanceReader::checkSCOORDContent(const SCoord & a_scoord) throw (::fwTools::Failed)
{
    //*****     Check the SCOORD    *****//
    const std::string & graphicType = a_scoord.getCRefGraphicType();
    const unsigned int  nbCoords    = a_scoord.getGraphicDataSize();
    // First, check the graphic type
    if ( graphicType == DicomDictionarySR::getGraphicTypeString(DicomDictionarySR::POINT) )
    {
        // Then, check the number of coordinates in the graphic data
        if (nbCoords != 2)
            throw ::fwTools::Failed("Graphic data are corrupted.");
    }
    else if( (graphicType == DicomDictionarySR::getGraphicTypeString(DicomDictionarySR::POLYLINE))
          || (graphicType == DicomDictionarySR::getGraphicTypeString(DicomDictionarySR::MULTIPOINT)) )
    {
        if ( (nbCoords < 4)
          && (nbCoords%2 == 0) )
            throw ::fwTools::Failed("Graphic data are corrupted.");
    }
    else
    {
        throw ::fwTools::Failed("Graphic Type unappropriated");
    }
}

}  // namespace reader

}  // namespace gdcmIO

