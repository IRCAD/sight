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

#include "gdcmIO/reader/DicomLandmarkReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/DicomDictionarySR.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomLandmarkReader::DicomLandmarkReader()
{
    SLM_TRACE_FUNC();

    this->m_dicomLandmarks = ::boost::shared_ptr<DicomLandmark>( new DicomLandmark );
}

//------------------------------------------------------------------------------

DicomLandmarkReader::~DicomLandmarkReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomLandmarkReader::read() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr   image   = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    SLM_ASSERT("gdcm::Reader not instanced", this->getReader());

    const ::gdcm::DataSet & gDsRoot = this->getDataSet();
    if (helper::GdcmData::getTagValue<0x0040,0xa040>(gDsRoot) == DicomDictionarySR::getTypeString(DicomDictionarySR::CONTAINER))
    {
        // Define the continuity of content
        bool                continuity      = false;
        const std::string   continuityStr   = helper::GdcmData::getTagValue<0x0040,0xa050>(gDsRoot);   // Continuity Of Content
        if ( continuityStr == DicomDictionarySR::getContinuityString(DicomDictionarySR::CONTINUOUS) )
        {
            continuity = true;
        }

        // Search child in the root of the document SR
        if (gDsRoot.FindDataElement( ::gdcm::Tag(0x0040,0xa730) ))        // Content Sequence
        {
            const ::gdcm::DataElement &     gDeLandmarks    = gDsRoot.GetDataElement( ::gdcm::Tag(0x0040,0xa730) );
            const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqLandmarks    = gDeLandmarks.GetValueAsSQ();

            // Search and read landmark(s) in children of the document SR root
            ::gdcm::SequenceOfItems::ConstIterator it       = gSqLandmarks->Begin();
            ::gdcm::SequenceOfItems::ConstIterator itEnd    = gSqLandmarks->End();
            // For each child
            for (; it != itEnd; ++it)
            {
                try
                {// Try to read a child as a landmark
                    // Read one landmark and add it to m_dicomLandmark
                    this->readLandmark(it->GetNestedDataSet(), continuity);
                }
                catch(::fwTools::Failed & e)
                {
                    OSLM_ERROR("Structured reporting reading error : "<<e.what());
                }
            }
            // Add landmarks to fwData::Image
            this->m_dicomLandmarks->convertToData(image);
        }
        else
        {
            throw ::fwTools::Failed("Empty structured reporting container");
        }
    }
    // else nothing because root of SR data set is always a container
}

//------------------------------------------------------------------------------

void DicomLandmarkReader::readLandmark(const ::gdcm::DataSet & a_ds, const bool a_continuity/* = false*/) throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if ( a_continuity )
    {// if continuous content
        throw ::fwTools::Failed("Continuous content not handled");
    }

    // Initialization
    std::string         label       = "", refFrame = "";
    SCoord              scoord;             // Spatial COORDinates of a frame point
    const std::string   separator   = ",";  // Separator of binary content

    //*****     Get a label     *****//
    const ::gdcm::DataSet * gDsTEXT = helper::DicomSR::getCodedContainer(DicomDictionarySR::getCodeValue(DicomDictionarySR::LANDMARK),
                                                                             a_ds);
    if ( gDsTEXT == 0 )
        throw ::fwTools::Failed("No landmark found");

    label = helper::GdcmData::getTagValue<0x0040,0xa160>(*gDsTEXT);             // Text Value

    //*****     Get a point     *****//
    //***       Get SCOORD      *****//
    const ::gdcm::DataSet * gDsSCOORD = helper::DicomSR::getTypedContainer(DicomDictionarySR::getTypeString(DicomDictionarySR::SCOORD),
                                                                           *gDsTEXT);
    if ( gDsSCOORD == 0 )
        throw ::fwTools::Failed("No point found");

    scoord = helper::DicomSR::readSCOORD(*gDsSCOORD);

    if ( scoord.getCRefGraphicType() != DicomDictionarySR::getGraphicTypeString(DicomDictionarySR::POINT) )
    {
        throw ::fwTools::Failed("Graphic Type not found or unappropriated");
    }

    // Check SCOORD comply with one point of a landmark
    const unsigned int nbCoords = scoord.getGraphicDataSize();
    if ( nbCoords != 2 )
        throw ::fwTools::Failed("Graphic data are corrupted.");

    //***       Get IMAGE      *****//
    const ::gdcm::DataSet * gDsIMAGE = helper::DicomSR::getTypedContainer(DicomDictionarySR::getTypeString(DicomDictionarySR::IMAGE),
                                                                          *gDsSCOORD);
    if ( gDsIMAGE == 0 )
        throw ::fwTools::Failed("Referenced Type not found or unappropriated");

    if ( !gDsIMAGE->FindDataElement( ::gdcm::Tag(0x0008,0x1199) ) ) // Referenced SOP Sequence
        throw ::fwTools::Failed("Referenced sequence not found");

    const ::gdcm::DataElement & gDeRefSeq = gDsIMAGE->GetDataElement( ::gdcm::Tag(0x0008,0x1199) ); // Referenced SOP Sequence
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefSeq = gDeRefSeq.GetValueAsSQ();

    if ( gSqRefSeq->GetNumberOfItems() < 1 )
        throw ::fwTools::Failed("Referenced sequence empty");

    const ::gdcm::Item &    gItRefSeq = gSqRefSeq->GetItem(1);
    const ::gdcm::DataSet & gDsRefSeq = gItRefSeq.GetNestedDataSet();

    ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();
    // For multiframe image, we search one indexes of frame
    if (dicomInstance->getIsMultiFrame())
    {
        // Referenced Frame Number
        refFrame   = helper::GdcmData::getTagValue<0x0008,0x1160>(gDsRefSeq);   // Type 1C
        if (refFrame.empty())
        {// If referenced frames number are not found
            refFrame = "1"; // First frame is referenced for 2D image

            ::fwData::Image::csptr image = this->getConcreteObject();
            if (image->getDimension() > 2)
            {// All frames are referenced for 3D image
                const unsigned int nbFrames = image->getCRefSize()[2];
                for (unsigned int i = 2; i <= nbFrames; ++i)
                {
                    refFrame += separator + ::fwTools::getString(i);
                }
            }
        }
    }
    else
    {
        // Referenced SOP Instance UID
        const std::string                   SOPInstanceUID              = helper::GdcmData::getTagValue<0x0008,0x1155>(gDsRefSeq);   // Type 1
        if (SOPInstanceUID.empty())
            throw ::fwTools::Failed("Referenced image empty");

        const std::vector< std::string > &  referencedSOPInstanceUIDs   = dicomInstance->getCRefReferencedSOPInstanceUIDs();
        try
        {// Try to translate the SOP instance UID into the corresponding frame number
            refFrame = ::fwTools::getString( helper::DicomSR::instanceUIDToFrameNumber(SOPInstanceUID, referencedSOPInstanceUIDs) );
        }
        catch(::fwTools::Failed & e)
        {
            throw;
        }
    }


    // Add one landmark
    if (!a_continuity)
    {// if separate content
        this->m_dicomLandmarks->addLabel( label );
        this->m_dicomLandmarks->addSCoord( scoord );
        this->m_dicomLandmarks->addRefFrame( atoi( refFrame.c_str() ) );
    }
}

}  // namespace reader

}  // namespace gdcmIO

