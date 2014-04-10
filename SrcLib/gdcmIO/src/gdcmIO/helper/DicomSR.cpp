/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <gdcmGlobal.h>
#include <gdcmDicts.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>
#include <gdcmImageApplyLookupTable.h>
#include <gdcmScanner.h>
#include <gdcmRescaler.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/bimap/bimap.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwComEd/helper/Array.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/helper/DicomSR.hpp"
#include "gdcmIO/helper/DicomTools.hpp"

namespace gdcmIO
{
namespace helper
{

const ::gdcm::DataSet* DicomSR::getCodedContainer(const std::string &codeValue, const ::gdcm::DataSet &dataset)
{
    SLM_TRACE_FUNC();

    // Concept Name Code Sequence
    if (dataset.FindDataElement(::gdcm::Tag(0x0040, 0xa043)))
    {
        // Search coded container at this level
        const ::gdcm::DataElement &contentDataElement = dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa043));
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > contentSequence = contentDataElement.GetValueAsSQ();
        if (contentSequence->GetNumberOfItems() > 0)
        {
            const ::gdcm::Item & itemCode = contentSequence->GetItem(1);
            const ::gdcm::DataSet & codeDataset = itemCode.GetNestedDataSet();
            if (DicomData::getTrimmedTagValue< 0x0008, 0x0100 >(codeDataset) == codeValue) // Code Value
            {
                return &dataset;
            }
        }
    }
    else if (dataset.FindDataElement(::gdcm::Tag(0x0040, 0xa730))) // Content Sequence
    {
        // Search in children
        const ::gdcm::DataElement& contentDataElement = dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa730));
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > contentSequence = contentDataElement.GetValueAsSQ();

        // Traverse each item to find coded container
        ::gdcm::SequenceOfItems::ConstIterator it;
        // For each item
        for (it = contentSequence->Begin(); it != contentSequence->End(); ++it)
        {
            const ::gdcm::DataSet* ans = DicomSR::getCodedContainer(codeValue, it->GetNestedDataSet());
            if (ans != 0)
            {
                return ans;
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

const ::gdcm::DataSet * DicomSR::getTypedContainer(const std::string & typeValue, const ::gdcm::DataSet & dataset,
        unsigned int index)
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Wrong index value", index >= 0);
    if (DicomData::getTrimmedTagValue< 0x0040, 0xa040 >(dataset) == typeValue) // Type Value
    {
        // Search typed container at this level
        return &dataset;
    }
    else if (dataset.FindDataElement(::gdcm::Tag(0x0040, 0xa730))) // Content Sequence
    { // Search in children
        const ::gdcm::DataElement & contentDataset = dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa730));
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > contentSequence = contentDataset.GetValueAsSQ();

        // Traverse each item to find typed container
        ::gdcm::SequenceOfItems::ConstIterator it;
        unsigned int i = 0;
        for (it = contentSequence->Begin(); it != contentSequence->End(); ++it)
        {
            const ::gdcm::DataSet * ans = DicomSR::getTypedContainer(typeValue, it->GetNestedDataSet());
            if (ans != 0)
            {
                if (i == index)
                {
                    return ans;
                }
                ++i;
            }
        }
    }
    // else, the data set is not a SR one

    return 0;
}

//------------------------------------------------------------------------------

void DicomSR::createSCoord(const ::gdcmIO::container::DicomSCoord &scoord, const std::string &refFrame,
        const std::string &classUID, const std::string &instanceUID,
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence)
{
    // Create a SCOORD
    // See: PS 3.3 Table 10-2 Content Item Macro Attributes Description

    ::gdcm::Item itemSCOORD;
    itemSCOORD.SetVLToUndefined();
    ::gdcm::DataSet & scoordDataset = itemSCOORD.GetNestedDataSet();

    // Relationship Type
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa010 >("INFERRED FROM", scoordDataset);

    // Type Value
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa040 >("SCOORD", scoordDataset);

    // Concept Name Code Sequence
    // Inferred from PS 3.3 C.17.3.2.1 and 3.16 Context ID 7003 - Type 1C
    DicomTools::createCodeSequence< 0x0040, 0xa043 >(scoord.getCRefContext(), scoordDataset);

    // Write coordinates
    const unsigned int dataSize = scoord.getGraphicDataSize();
    unsigned int nbCoords;
    if (scoord.getGraphicType() == "POINT")
    {
        nbCoords = 2;
    }
    else
    {
        nbCoords = dataSize;
    }

    OSLM_ASSERT("'"<<nbCoords<<"' isn't even.", nbCoords%2 == 0 );

    float *graphicData = new float[nbCoords];
    for (unsigned int i = 0; i < nbCoords; ++i)
    {
        graphicData[i] = scoord[i];
    }
    // Graphic Data - Type 1
    ::gdcmIO::helper::DicomData::setTagValues< float, 0x0070, 0x0022 >(graphicData, nbCoords, scoordDataset);
    delete[] graphicData;

    // Graphic Type - Type 1
    ::gdcmIO::helper::DicomData::setTagValue< 0x0070, 0x0023 >(scoord.getGraphicType(), scoordDataset);

    // Create a SQ which contains referenced image
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > imageSequence = new ::gdcm::SequenceOfItems();
    imageSequence->SetLengthToUndefined();
    {
        createIMAGE(refFrame, classUID, instanceUID, imageSequence);
        ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa730 >(imageSequence, scoordDataset);
    }

    sequence->AddItem(itemSCOORD);
}

//------------------------------------------------------------------------------

void DicomSR::createSCoord(const ::gdcmIO::container::DicomSCoord &scoord, const std::string &refFrame,
        const std::string &classUID, const std::string &instanceUID, ::gdcm::DataSet &dataset)
{
    // Get or create a content sequence in order to insert a SCOORD
    const ::gdcm::Tag contentSQTag(0x0040, 0xa730); // Content Sequence
    if (!dataset.FindDataElement(contentSQTag))
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
        ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa730 >(sequence, dataset);
    }
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > contentSequence =
            dataset.GetDataElement(contentSQTag).GetValueAsSQ();

    // Write a SCOORD
    DicomSR::createSCoord(scoord, refFrame, classUID, instanceUID, contentSequence);
}

//------------------------------------------------------------------------------

void DicomSR::createSCoord(const ::gdcmIO::container::DicomSCoord &scoord, const std::string &classUID,
        const std::string &instanceUID, ::gdcm::DataSet &dataset)
{
    // Get or create a content sequence in order to insert a SCOORD
    const ::gdcm::Tag contentSQTag(0x0040, 0xa730); // Content Sequence
    if (!dataset.FindDataElement(contentSQTag))
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
        ::gdcmIO::helper::DicomData::setSQ< 0x0040, 0xa730 >(sequence, dataset);
    }
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > contentSequence =
            dataset.GetDataElement(contentSQTag).GetValueAsSQ();

    // Write a SCOORD
    DicomSR::createSCoord(scoord, "", classUID, instanceUID, contentSequence);
}

//------------------------------------------------------------------------------

void DicomSR::createIMAGE(const std::string &refFrame, const std::string &classUID, const std::string &instanceUID,
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence)
{
    ::gdcm::Item imageItem;
    imageItem.SetVLToUndefined();
    ::gdcm::DataSet &imageDataset = imageItem.GetNestedDataSet();

    // Relationship Type
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa010 >("SELECTED FROM", imageDataset);

    // Type Value
    ::gdcmIO::helper::DicomData::setTagValue< 0x0040, 0xa040 >("IMAGE", imageDataset);

    //** Create a SQ which contains referenced frames of an image **//
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > imageSequence = new ::gdcm::SequenceOfItems();
    imageSequence->SetLengthToUndefined();

    {
        //see: PS 3.3 Table 10-3 IMAGE SOP INSTANCE REFERENCE MACRO ATTRIBUTES
        ::gdcm::Item referencedImageItem;
        referencedImageItem.SetVLToUndefined();
        ::gdcm::DataSet & referencedImageDataset = referencedImageItem.GetNestedDataSet();

        // Referenced SOP Class UID - Type 1
        ::gdcmIO::helper::DicomData::setTagValue< 0x0008, 0x1150 >(classUID, referencedImageDataset);

        // Referenced SOP Instance UID  - Type 1
        ::gdcmIO::helper::DicomData::setTagValue< 0x0008, 0x1155 >(instanceUID, referencedImageDataset);

        if (!refFrame.empty())
        {
            const int referencedFrame = ::boost::lexical_cast<int>(refFrame.c_str());

            // Referenced Frame Number - Type 1C
            ::gdcmIO::helper::DicomData::setTagValues< int, 0x0008, 0x1160 >(&referencedFrame, 1, referencedImageDataset);
        }

        imageSequence->AddItem(referencedImageItem);

        // Referenced SOP Sequence
        ::gdcmIO::helper::DicomData::setSQ< 0x0008, 0x1199 >(imageSequence, imageDataset);

    } // End container of referenced frames

    sequence->AddItem(imageItem);
}

//------------------------------------------------------------------------------

unsigned int DicomSR::instanceUIDToFrameNumber(const std::string &instanceUID,
        const std::vector< std::string > &referencedInstanceUIDs) throw (::gdcmIO::exception::Failed)
{
    unsigned int frameNumber = 0;
    const unsigned int numberOfFrames = referencedInstanceUIDs.size();
    bool stop = false;

    // Traverse referenced SOP instance UIDs to find the good one
    while (frameNumber < numberOfFrames && !stop)
    {
        if (referencedInstanceUIDs[frameNumber] == instanceUID)
        {
            stop = true;
        }
        else
        {
            ++frameNumber;
        }
    }

    if (frameNumber < numberOfFrames)
    {
        return frameNumber + 1; // +1 because frame number start at 1.
    }
    else
    {
        throw ::gdcmIO::exception::Failed("Referenced image not found");
    }

    return 0;
}

//------------------------------------------------------------------------------

::gdcmIO::container::DicomSCoord DicomSR::readSCOORD(const ::gdcm::DataSet & dataset)
{
    ::gdcmIO::container::DicomSCoord scoord;

    // Graphic Type
    scoord.setGraphicType(DicomData::getTrimmedTagValue< 0x0070, 0x0023 >(dataset));

    // Graphic Data
    ::gdcm::Attribute< 0x0070, 0x0022 > attribute;
    attribute.SetFromDataElement(dataset.GetDataElement(::gdcm::Attribute< 0x0070, 0x0022 >::GetTag()));
    const unsigned int nbCoords = attribute.GetNumberOfValues();
    scoord.setGraphicData(attribute.GetValues(), nbCoords);

    // Concept Name Code Sequence (Context)
    scoord.setContext(DicomTools::readCodeSequence< 0x0040, 0xa043 >(dataset));

    return scoord;
}

} //namespace helper

} //namespace gdcmIO

