/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <fwComEd/Dictionary.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>
#include <fwTools/dateAndTime.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/helper/DicomSR.hpp"
#include "gdcmIO/helper/DicomTools.hpp"
#include "gdcmIO/writer/ie/Document.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Document::Document(SPTR(::gdcm::Writer) writer,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwData::Image::sptr image):
        ::gdcmIO::writer::ie::InformationEntity< ::fwData::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Document::~Document()
{
}

//------------------------------------------------------------------------------

void Document::writeSRDocumentGeneralModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    ::boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();

    // Instance Number - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< int, 0x0020, 0x0013 >(0, dataset);

    // Content Date - Type 1 - FIXME: Keep series date ?
    std::string date = ::fwTools::getDate(ptime);
    ::gdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0023 >(date, dataset);

    // Content Time - Type 1 - FIXME: Keep series time ?
    std::string time =  ::fwTools::getTime(ptime);
    ::gdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0033 >(time, dataset);

    // Performed Procedure Code Sequence (0040,A372) // Type 2 (FIXME: CID 7000 ?)
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
    sequence->SetLengthToUndefined();
    ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa372 >(sequence, dataset);

    // Completion flag - Type 1 - (FIXME: Set to COMPLETE ?)
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa491 >("PARTIAL", dataset);

    // Verification Flag - Type 1 - (FIXME: Set to VERIFIED ?)
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa493 >("UNVERIFIED", dataset);

}

//------------------------------------------------------------------------------

void Document::writeSRDocumentContentModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // Type value (0040,A040) - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa040 >("CONTAINER", dataset);

   // Continuity of content (landmarks and distances are separated) - Type 1 - See PS 3.3 C.18.8
   // NOTE : Continuity is fixed to SEPARATE because it provides a better compatibility.
   ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa050 >("SEPARATE", dataset);


   // Concept Name Code Sequence - (0040,A043) - Type 1C
   //TODO: FIX THIS VALUE
   ::gdcmIO::helper::DicomTools::createCodeSequence< 0x0040, 0xa043 >("18748-4", "LN", "Diagnostic Imaging Report ",
           dataset);




}

//------------------------------------------------------------------------------

void Document::writeLandmarks()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // Try to change landmarks into a DICOM form
    SPTR(::gdcmIO::container::DicomLandmark) landmarkContainer =
            ::boost::make_shared< ::gdcmIO::container::DicomLandmark >();
    landmarkContainer->setFromData(m_object);

    // Write Landmarks
    // Get or create a content sequence in order to insert landmark
    const ::gdcm::Tag contentSequenceTag(0x0040, 0xa730);
    if (!dataset.FindDataElement(contentSequenceTag))
    {
        // Create the content sequence
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
        ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa730 >(sequence, dataset);
    }

    // Get the content sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = dataset.GetDataElement(contentSequenceTag).GetValueAsSQ();

    // Write each landmark in a sequence of items
    ::fwData::PointList::sptr pointList =
            m_object->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
    if (pointList)
    {
        for (unsigned int i = 0; i < pointList->getPoints().size(); ++i)
        {
            this->writeLandmark(i, sequence, landmarkContainer);
        }
    }

}

//------------------------------------------------------------------------------

void Document::writeLandmark(const unsigned int index, ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
        SPTR(::gdcmIO::container::DicomLandmark) landmarkContainer)
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // Current Requested Procedure Evidence Sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > evidenceSequence =
            dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa375)).GetValueAsSQ();

    // Create an item for a landmark
    ::gdcm::Item landmarkItem;
    landmarkItem.SetVLToUndefined();
    ::gdcm::DataSet &landmarkItemDataset = landmarkItem.GetNestedDataSet();

    // Relationship Value - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa010 >("CONTAINS", landmarkItemDataset);

    // Type Value - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa040 >("TEXT", landmarkItemDataset);

    // Text Value - Type 1C
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa160 >(landmarkContainer->getLabelContainer()[index],
            landmarkItemDataset);

    // Concept Name Code Sequence - Type 1C
    ::gdcmIO::helper::DicomTools::createCodeSequence< 0x0040, 0xa043 >("121071", "DCM", "Finding", landmarkItemDataset);

    // Spatial coordinates (SCOORD)
    ::gdcmIO::container::DicomSCoord & currentSCOORD = landmarkContainer->getRefSCoordContainer()[index];
    const unsigned int numberOfReferencedFrame = landmarkContainer->getCRefReferencedFrameContainer()[index];

    // 112171 Fiducial mark A location in image space, which may or may not correspond to an anatomical reference,
    // which is often used for registering datasets.
    currentSCOORD.setContext(::gdcmIO::container::DicomCodedAttribute("112171", "DCM", "Fiducial mark"));

    if (!m_instance->getIsMultiFiles())
    {
        ::gdcmIO::helper::DicomSR::createSCoord(landmarkContainer->getSCoordContainer()[index],
                ::fwTools::getString(numberOfReferencedFrame), m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[0], landmarkItemDataset);
    }
    else
    {
        ::gdcmIO::helper::DicomSR::createSCoord(landmarkContainer->getSCoordContainer()[index], m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[numberOfReferencedFrame - 1], // -1 because frame number start at 1
                landmarkItemDataset);
    }

    // Add the landmark in to content sequence of root
    sequence->AddItem(landmarkItem);
}

//------------------------------------------------------------------------------

void Document::writeDistances()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // Try to change distances into a DICOM form
    SPTR(::gdcmIO::container::DicomDistance) distanceContainer =
            ::boost::make_shared< ::gdcmIO::container::DicomDistance >();
    distanceContainer->setFromData(m_object);

    // Write Distances
    // Get or create a content sequence in order to insert distances
    const ::gdcm::Tag contentSequenceTag(0x0040, 0xa730);
    if (!dataset.FindDataElement(contentSequenceTag))
    {
        // Create a content sequence
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
        ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa730 >(sequence, dataset);
    }

    // Get the content sequence of the root of document SR
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
            dataset.GetDataElement(contentSequenceTag).GetValueAsSQ();

    ::fwData::Vector::sptr distanceVector =
            m_object->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
    if (distanceVector)
    {
        for (unsigned int i = 0; i < distanceVector->size(); ++i)
        {
            // Write one distance as a child of root of document SR
            this->writeDistance(i, sequence, distanceContainer);
        }
    }
}

//------------------------------------------------------------------------------

void Document::writeDistance(const unsigned int index, ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
        SPTR(::gdcmIO::container::DicomDistance) distanceContainer)
{
    // Create one item as child - PS 3.16 TID 1400
    ::gdcm::Item measureItem;
    measureItem.SetVLToUndefined();
    ::gdcm::DataSet &measureDataset = measureItem.GetNestedDataSet();

    // Relationship Value - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa010 >("CONTAINS", measureDataset);

    // Type Value
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa040 >("NUM", measureDataset);

    // Concept Name Code Sequence - Type 1C
    ::gdcmIO::helper::DicomTools::createCodeSequence< 0x0040, 0xa043 >("121206", "DCM", "Distance", measureDataset);

    // Create a SQ which contains NUM (measured value sequence)
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > numericSequence = new ::gdcm::SequenceOfItems();
    numericSequence->SetLengthToUndefined();
    {
        // Create a measured value
        ::gdcm::Item numericItem;
        numericItem.SetVLToUndefined();
        ::gdcm::DataSet & numericDataset = numericItem.GetNestedDataSet();

        // Add numerical value - Type 1
        const double numericValue = ::boost::lexical_cast< double >(distanceContainer->getDistanceContainer()[index]);
        ::gdcmIO::helper::DicomData::setTagValues< double, 0x0040, 0xa30a >(&numericValue, 1, numericDataset);

        // Add measured units code sequence
        ::gdcmIO::helper::DicomTools::createCodeSequence< 0x0040, 0x08ea >("mm", "UCUM", "millimeter", "1.4",
                numericDataset);

        // Add Measured value to the sequence
        numericSequence->AddItem(numericItem);
    }
    // Add Measured value sequence to the data set
    ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa300 >(numericSequence, measureDataset);

    // Add Spatial coordinates (SCOORD)
    ::gdcmIO::container::DicomSCoord &currentSCOORD = distanceContainer->getRefSCoordContainer()[index];
    const std::string& referencedFrameNumbers = distanceContainer->getCRefReferencedFrameContainer()[index];

    // Split referenced frame numbers into 2 string
    std::vector< std::string > splitedRefFrame;
    ::boost::split(splitedRefFrame, referencedFrameNumbers, ::boost::is_any_of(",")); // Get each referenced frame number

    SLM_ASSERT("Referenced frame numbers are corrupted", splitedRefFrame.size() == 2);

    // Get the graphic data of 2 points
    const std::vector< float > graphicData = currentSCOORD.getGraphicData();

    // If the image storage is multi-frame
    if (!m_instance->getIsMultiFiles())
    {
        currentSCOORD.setContext(::gdcmIO::container::DicomCodedAttribute("121055", "DCM", "Path"));

        // Split graphic data into 2 points
        float graphicDataTmp[2];

        // Get first point
        graphicDataTmp[0] = graphicData[0];
        graphicDataTmp[1] = graphicData[1];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);
        // Write first SCOORD
        ::gdcmIO::helper::DicomSR::createSCoord(currentSCOORD, splitedRefFrame[0], m_instance->getCRefSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[0], measureDataset);

        // Get second point
        graphicDataTmp[0] = graphicData[2];
        graphicDataTmp[1] = graphicData[3];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);

        // Write second SCOORD
        ::gdcmIO::helper::DicomSR::createSCoord(currentSCOORD, splitedRefFrame[1], m_instance->getCRefSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[0], measureDataset);
    }
    else
    {
        // Else (image save in several files)
        currentSCOORD.setGraphicType("POINT");
        currentSCOORD.setContext(::gdcmIO::container::DicomCodedAttribute("121230", "DCM", "Path Vertex"));

        // Split graphic data into 2 points
        float graphicDataTmp[2];

        // Get first point
        graphicDataTmp[0] = graphicData[0];
        graphicDataTmp[1] = graphicData[1];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);

        // Write first SCOORD
        ::gdcmIO::helper::DicomSR::createSCoord(currentSCOORD, m_instance->getCRefSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[::boost::lexical_cast< int >(splitedRefFrame[0]) - 1], // -1 because frame number start at 1
                measureDataset);

        // Get second point
        graphicDataTmp[0] = graphicData[2];
        graphicDataTmp[1] = graphicData[3];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);

        // Write second SCOORD
        ::gdcmIO::helper::DicomSR::createSCoord(currentSCOORD, m_instance->getCRefSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[::boost::lexical_cast< int >(splitedRefFrame[1]) - 1], // -1 because frame number start at 1
                measureDataset);
    }

    // Add the distance to the content sequence of root
    sequence->AddItem(measureItem);
}

//------------------------------------------------------------------------------

void Document::writeSOPCommonModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    std::string sopClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::EnhancedSR);
    ::gdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0016 >(sopClassUID, dataset);

    // SOP Instance UID
    ::gdcm::UIDGenerator uidGenerator;
    std::string sopInstanceUID = uidGenerator.Generate();
    ::gdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0018 >(sopInstanceUID, dataset);
}


} // namespace ie
} // namespace writer
} // namespace gdcmIO
