/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include "gdcmIO/reader/dicom/DicomLandmark.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/container/DicomDictionarySR.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

//------------------------------------------------------------------------------

DicomLandmark::DicomLandmark()
{
    SLM_TRACE_FUNC();
    this->m_dicomLandmarks = ::boost::shared_ptr< ::gdcmIO::container::DicomLandmark >( new ::gdcmIO::container::DicomLandmark );
}

//------------------------------------------------------------------------------

DicomLandmark::~DicomLandmark()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomLandmark::read() throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr   image   = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    SLM_ASSERT("gdcm::Reader not instanced", this->getReader());

    const ::gdcm::DataSet & gDsRoot = this->getDataSet();
    const std::string type = helper::GdcmData::getTrimmedTagValue<0x0040,0xa040>(gDsRoot);
    if (type == ::gdcmIO::container::DicomDictionarySR::getTypeString(
                                                  ::gdcmIO::container::DicomDictionarySR::CONTAINER))
    {
        // Define the continuity of content
        bool continuity      = false;
        std::string   continuityStr   = helper::GdcmData::getTrimmedTagValue<0x0040,0xa050>(gDsRoot);// Continuity Of Content
        if ( continuityStr == ::gdcmIO::container::DicomDictionarySR::getContinuityString(::gdcmIO::container::DicomDictionarySR::CONTINUOUS) )
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
                {
                    // Try to read a child as a landmark
                    // Read one landmark and add it to m_dicomLandmark
                    this->readLandmark(it->GetNestedDataSet(), continuity);
                }
                catch(::gdcmIO::exception::Failed & e)
                {
                    OSLM_ERROR("Structured reporting reading error : "<<e.what());
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
    // else nothing because root of SR data set is always a container
}

//------------------------------------------------------------------------------

void DicomLandmark::readLandmark(const ::gdcm::DataSet & a_ds, const bool a_continuity/* = false*/) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Continuous content not handled"), a_continuity);

    // Initialization
    std::string         label       = "", refFrame = "";
    ::gdcmIO::container::DicomSCoord              scoord;             // Spatial COORDinates of a frame point
    const std::string   separator   = ",";  // Separator of binary content

    //*****     Get a label     *****//
    const ::gdcm::DataSet * gDsTEXT = helper::DicomSR::getCodedContainer(
            ::gdcmIO::container::DicomDictionarySR::getCodeValue(::gdcmIO::container::DicomDictionarySR::LANDMARK), a_ds);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("No landmark found"), gDsTEXT == 0 );

    label = helper::GdcmData::getTagValue<0x0040,0xa160>(*gDsTEXT);             // Text Value

    //*****     Get a point     *****//
    //***       Get SCOORD      *****//
    const ::gdcm::DataSet * gDsSCOORD = helper::DicomSR::getTypedContainer(
            ::gdcmIO::container::DicomDictionarySR::getTypeString(::gdcmIO::container::DicomDictionarySR::SCOORD), *gDsTEXT);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("No point found"), gDsSCOORD == 0);

    scoord = helper::DicomSR::readSCOORD(*gDsSCOORD);

    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Graphic Type '"+scoord.getCRefGraphicType()+"' not found or unappropriated"),
                scoord.getCRefGraphicType() != ::gdcmIO::container::DicomDictionarySR::getGraphicTypeString(
                                                                    ::gdcmIO::container::DicomDictionarySR::POINT) );

    // Check SCOORD comply with one point of a landmark
    const unsigned int nbCoords = scoord.getGraphicDataSize();
    if ( nbCoords != 2 )
        throw ::gdcmIO::exception::Failed("Graphic data are corrupted.");

    //***       Get IMAGE      *****//
    const ::gdcm::DataSet * gDsIMAGE = helper::DicomSR::getTypedContainer(
            ::gdcmIO::container::DicomDictionarySR::getTypeString(::gdcmIO::container::DicomDictionarySR::IMAGE), *gDsSCOORD);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced Type not found or unappropriated"), gDsIMAGE == 0);

    // Referenced SOP Sequence
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced sequence not found"),
            !gDsIMAGE->FindDataElement( ::gdcm::Tag(0x0008,0x1199) ));

    const ::gdcm::DataElement & gDeRefSeq = gDsIMAGE->GetDataElement( ::gdcm::Tag(0x0008,0x1199) ); // Referenced SOP Sequence
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefSeq = gDeRefSeq.GetValueAsSQ();

    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced sequence empty"), gSqRefSeq->GetNumberOfItems() < 1);

    const ::gdcm::Item &    gItRefSeq = gSqRefSeq->GetItem(1);
    const ::gdcm::DataSet & gDsRefSeq = gItRefSeq.GetNestedDataSet();

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();
    // For multiframe image, we search one indexes of frame
    if (dicomInstance->getIsMultiFrame())
    {
        // Referenced Frame Number
        refFrame   = helper::GdcmData::getTagValue<0x0008,0x1160>(gDsRefSeq);   // Type 1C
        if (refFrame.empty())
        {// If referenced frames number are not found
            refFrame = "1"; // First frame is referenced for 2D image

            ::fwData::Image::csptr image = this->getConcreteObject();
            if (image->getNumberOfDimensions() > 2)
            {
                // All frames are referenced for 3D image
                const unsigned int nbFrames = image->getSize()[2];
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
        const std::string SOPInstanceUID = helper::GdcmData::getTagValue<0x0008,0x1155>(gDsRefSeq);   // Type 1
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Referenced image empty"), SOPInstanceUID.empty());

        const std::vector< std::string >&  referencedSOPInstanceUIDs = dicomInstance->getCRefReferencedSOPInstanceUIDs();
        try
        {// Try to translate the SOP instance UID into the corresponding frame number
            refFrame = ::fwTools::getString( helper::DicomSR::instanceUIDToFrameNumber(SOPInstanceUID, referencedSOPInstanceUIDs) );
        }
        catch(::gdcmIO::exception::Failed & e)
        {
            throw;
        }
    }


    // Add one landmark
    if (!a_continuity)
    {
        // if separate content
        this->m_dicomLandmarks->addLabel( label );
        this->m_dicomLandmarks->addSCoord( scoord );
        this->m_dicomLandmarks->addRefFrame( atoi( refFrame.c_str() ) );
    }
}

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

