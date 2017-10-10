/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRNumNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/helper/StructuredReport.hpp"

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) StructuredReport::readSR(const ::gdcm::DataSet& dataset)
{
    SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) result;

    // Value Type - Type 1
    const std::string type = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0040, 0xa040 >(dataset);

    // Concept Name Code Sequence - Type 1C
    const ::fwGdcmIO::container::DicomCodedAttribute& codedAttribute =
        ::fwGdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(dataset);

    if(type == "CONTAINER" && !codedAttribute.isEmpty())
    {
        result = std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(codedAttribute);
        ::fwGdcmIO::helper::StructuredReport::readSubNodeContainer(dataset, result);
    }

    return result;
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNodeContainer(const ::gdcm::DataSet& dataset,
                                            SPTR(::fwGdcmIO::container::sr::DicomSRNode) parent)
{
    // Retrieve the content sequence
    if(dataset.FindDataElement(::gdcm::Tag(0x0040, 0xa730)))
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa730)).GetValueAsSQ();

        for(unsigned int i = 1; i <= sequence->GetNumberOfItems(); ++i)
        {
            const ::gdcm::DataSet& itemDataset = sequence->GetItem(i).GetNestedDataSet();
            ::fwGdcmIO::helper::StructuredReport::readSubNode(itemDataset, parent);
        }

    }
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNode(const ::gdcm::DataSet& dataset,
                                   SPTR(::fwGdcmIO::container::sr::DicomSRNode) parent)
{
    SPTR(::fwGdcmIO::container::sr::DicomSRNode) node;

    // Value Type - Type 1
    const std::string type = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0040, 0xa040 >(dataset);

    // Concept Name Code Sequence - Type 1C
    const ::fwGdcmIO::container::DicomCodedAttribute& codedAttribute =
        ::fwGdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(dataset);

    // Relationship Value - Type 1
    const std::string relationship = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0040, 0xa010 >(dataset);

    if(type == "TEXT")
    {
        // Text Value - Type 1C
        const std::string textValue = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0040, 0xa160 >(dataset);

        // Create Text Node
        node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRTextNode >(
                codedAttribute, relationship, textValue);
    }
    else if(type == "NUM")
    {
        // Retrieve the measured value sequence
        if(dataset.FindDataElement(::gdcm::Tag(0x0040, 0xa300)))
        {
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                            dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa300)).GetValueAsSQ();
            if(sequence->GetNumberOfItems() > 0)
            {
                const ::gdcm::DataSet& itemDataset = sequence->GetItem(1).GetNestedDataSet();

                // Numerical value - Type 1
                const double numValue =
                    ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0040, 0xa30a, double >(itemDataset);

                // Measured units code sequence - Type 1
                const ::fwGdcmIO::container::DicomCodedAttribute& measurementUnits =
                    ::fwGdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0x08ea>(itemDataset);

                // Create Num Node
                node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRNumNode >(
                    codedAttribute, relationship, numValue, measurementUnits);

            }
        }
    }
    else if(type == "SCOORD")
    {
        // Graphic Data - Type 1C
        ::gdcm::Attribute< 0x0070, 0x0022 > graphicDataAttribute;
        graphicDataAttribute.SetFromDataElement(dataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* graphicData = graphicDataAttribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphicType = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0070, 0x0023 >(dataset);

        if(graphicType == "POINT" && graphicDataAttribute.GetNumberOfValues() == 2)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 2);
            node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoordNode >(
                    codedAttribute, relationship, graphicType, scoordVector);
        }
        else if(graphicType == "POLYLINE" && graphicDataAttribute.GetNumberOfValues() == 4)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 4);
            node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoordNode >(
                    codedAttribute, relationship, graphicType, scoordVector);
        }
        else
        {
            SLM_WARN("Graphic type is not handled or data size doesn't match type. The SR node has been ignored.");
        }

    }
    else if(type == "SCOORD3D")
    {
        // Referenced Frame of Reference UID - Type 1
        const std::string frameOfReferenceUID =
            ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x3006, 0x0024 >(dataset);

        // Graphic Data - Type 1C
        ::gdcm::Attribute< 0x0070, 0x0022 > graphicDataAttribute;
        graphicDataAttribute.SetFromDataElement(dataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* graphicData = graphicDataAttribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphicType = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0070, 0x0023 >(dataset);

        if(graphicType == "POINT" && graphicDataAttribute.GetNumberOfValues() == 3)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 3);
            node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoord3DNode >(
                    codedAttribute, relationship, graphicType, scoordVector, frameOfReferenceUID);
        }
        else if(graphicType == "POLYLINE" && graphicDataAttribute.GetNumberOfValues() == 6)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 6);
            node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoord3DNode >(
                    codedAttribute, relationship, graphicType, scoordVector, frameOfReferenceUID);
        }
        else
        {
            SLM_WARN("Graphic type is not handled or data size doesn't match type. The SR node has been ignored.");
        }

    }
    else if(type == "IMAGE")
    {
        // Retrieve the referenced SOP sequence
        if(dataset.FindDataElement(::gdcm::Tag(0x0008, 0x1199)))
        {
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                            dataset.GetDataElement(::gdcm::Tag(0x0008, 0x1199)).GetValueAsSQ();
            if(sequence->GetNumberOfItems() > 0)
            {
                const ::gdcm::DataSet& itemDataset = sequence->GetItem(1).GetNestedDataSet();

                // Referenced SOP Class UID - Type 1
                const std::string sopClassUID = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x1150 >(itemDataset);

                // Referenced SOP Instance UID  - Type 1
                const std::string sopInstanceUID = 
                ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x1155 >(itemDataset);

                // Referenced Frame Number - Type 1C
                int frameNumber = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x1160, int >(itemDataset);

                // Create Num Node
                node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRImageNode >(
                        codedAttribute, relationship, sopClassUID, sopInstanceUID, frameNumber);

            }
        }
    }
    else if(type == "CODE")
    {
        const ::fwGdcmIO::container::DicomCodedAttribute& codedEntry =
            ::fwGdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0xa168>(dataset);

        // Create Code Node
        node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRCodeNode >(codedAttribute, relationship, codedEntry);


    }
    else if(type == "UIDREF")
    {
        // UID Value - Type 1C
        const std::string uidValue = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0040, 0xa124 >(dataset);

        // Create UIDRef Node
        node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRUIDRefNode >(codedAttribute, relationship, uidValue);
    }
    else if(type == "CONTAINER")
    {
        // Create Container Node
        node = std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(codedAttribute, relationship);
    }

    if(node)
    {
        // Create children
        ::fwGdcmIO::helper::StructuredReport::readSubNodeContainer(dataset, node);

        // Add node to parent
        parent->addSubNode(node);
    }
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSR(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& root, std::ostream& out)
{
    out << "graph SR {" << std::endl;
    int index = 0;
    StructuredReport::dumpSRNode(root, out, index);
    out << "}" << std::endl;
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSRNode(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& root,
                                  std::ostream& out,
                                  int& index)
{
    // Write node
    out << "\t" << index << "[label=\"" << (*root) << "\"][shape=box];" << std::endl;

    const int parentIndex = index;
    for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& child : root->getSubNodeContainer())
    {
        // Write edge
        out << "\t" << parentIndex  << "--" << (index+1) << "[label=\""
            << child->getRelationship() << "\"];" << std::endl;

        // Write child
        StructuredReport::dumpSRNode(child, out, ++index);


    }
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwGdcmIO

