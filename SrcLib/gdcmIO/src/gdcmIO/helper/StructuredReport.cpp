/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "gdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "gdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "gdcmIO/container/sr/DicomSRNumNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "gdcmIO/container/sr/DicomSRImageNode.hpp"
#include "gdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/helper/StructuredReport.hpp"

namespace gdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

SPTR(::gdcmIO::container::sr::DicomSRContainerNode) StructuredReport::readSR(const ::gdcm::DataSet& dataset)
{
    SPTR(::gdcmIO::container::sr::DicomSRContainerNode) result;

    // Value Type - Type 1
    const std::string type = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0040, 0xa040 >(dataset);

    // Concept Name Code Sequence - Type 1C
    const ::gdcmIO::container::DicomCodedAttribute& codedAttribute =
            ::gdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(dataset);

    if(type == "CONTAINER" && !codedAttribute.isEmpty())
    {
        result = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(codedAttribute);
        ::gdcmIO::helper::StructuredReport::readSubNodeContainer(dataset, result);
    }

    return result;
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNodeContainer(const ::gdcm::DataSet& dataset, SPTR(::gdcmIO::container::sr::DicomSRNode) parent)
{
    // Retrieve the content sequence
    if(dataset.FindDataElement(::gdcm::Tag(0x0040, 0xa730)))
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
                dataset.GetDataElement(::gdcm::Tag(0x0040, 0xa730)).GetValueAsSQ();

        for(unsigned int i=1; i <= sequence->GetNumberOfItems(); ++i)
        {
            const ::gdcm::DataSet& itemDataset = sequence->GetItem(i).GetNestedDataSet();
            ::gdcmIO::helper::StructuredReport::readSubNode(itemDataset, parent);
        }

    }
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNode(const ::gdcm::DataSet& dataset, SPTR(::gdcmIO::container::sr::DicomSRNode) parent)
{
    SPTR(::gdcmIO::container::sr::DicomSRNode) node;

    // Value Type - Type 1
    const std::string type = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0040, 0xa040 >(dataset);

    // Concept Name Code Sequence - Type 1C
    const ::gdcmIO::container::DicomCodedAttribute& codedAttribute =
            ::gdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(dataset);

    // Relationship Value - Type 1
    const std::string relationship = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0040, 0xa010 >(dataset);

    if(type == "TEXT")
    {
        // Text Value - Type 1C
        const std::string textValue = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0040, 0xa160 >(dataset);

        // Create Text Node
        node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRTextNode >(
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
                const double numValue = ::gdcmIO::helper::DicomData::getTagValue< 0x0040, 0xa30a, double >(itemDataset);

                // Measured units code sequence - Type 1
                const ::gdcmIO::container::DicomCodedAttribute& measurementUnits =
                        ::gdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0x08ea>(itemDataset);

                // Create Num Node
                node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRNumNode >(
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
        const std::string graphicType = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0070, 0x0023 >(dataset);

        if(graphicType == "POINT" && graphicDataAttribute.GetNumberOfValues() == 2)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 2);
            node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRSCoordNode >(
                    codedAttribute, relationship, graphicType, scoordVector);
        }
        else if(graphicType == "POLYLINE" && graphicDataAttribute.GetNumberOfValues() == 4)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 4);
            node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRSCoordNode >(
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
        const std::string frameOfReferenceUID = ::gdcmIO::helper::DicomData::getTagValue< 0x3006, 0x0024 >(dataset);

        // Graphic Data - Type 1C
        ::gdcm::Attribute< 0x0070, 0x0022 > graphicDataAttribute;
        graphicDataAttribute.SetFromDataElement(dataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* graphicData = graphicDataAttribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphicType = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0070, 0x0023 >(dataset);

        if(graphicType == "POINT" && graphicDataAttribute.GetNumberOfValues() == 3)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 3);
            node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRSCoord3DNode >(
                    codedAttribute, relationship, graphicType, scoordVector, frameOfReferenceUID);
        }
        else if(graphicType == "POLYLINE" && graphicDataAttribute.GetNumberOfValues() == 6)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 6);
            node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRSCoord3DNode >(
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
                const std::string sopClassUID = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x1150 >(itemDataset);

                // Referenced SOP Instance UID  - Type 1
                const std::string sopInstanceUID = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x1155 >(itemDataset);

                // Referenced Frame Number - Type 1C
                int frameNumber = ::gdcmIO::helper::DicomData::getTagValue< 0x0008, 0x1160, int >(itemDataset);

                // Create Num Node
                node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRImageNode >(
                        codedAttribute, relationship, sopClassUID, sopInstanceUID, frameNumber);

            }
        }
    }
    else if(type == "CODE")
    {
        const ::gdcmIO::container::DicomCodedAttribute& codedEntry =
                ::gdcmIO::helper::StructuredReport::readCodeSequence<0x0040, 0xa168>(dataset);

        // Create Code Node
        node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRCodeNode >(codedAttribute, relationship, codedEntry);


    }
    else if(type == "UIDREF")
    {
        // UID Value - Type 1C
        const std::string uidValue = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0040, 0xa124 >(dataset);

        // Create UIDRef Node
        node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRUIDRefNode >(codedAttribute, relationship, uidValue);
    }
    else if(type == "CONTAINER")
    {
        // Create Container Node
        node = ::boost::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(codedAttribute, relationship);
    }

    if(node)
    {
        // Create children
        ::gdcmIO::helper::StructuredReport::readSubNodeContainer(dataset, node);

        // Add node to parent
        parent->addSubNode(node);
    }
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSR(const SPTR(::gdcmIO::container::sr::DicomSRNode)& root, std::ostream& out)
{
    out << "graph SR {" << std::endl;
    int index = 0;
    StructuredReport::dumpSRNode(root, out, index);
    out << "}" << std::endl;
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSRNode(const SPTR(::gdcmIO::container::sr::DicomSRNode)& root, std::ostream& out, int& index)
{
    // Write node
    out << "\t" << index << "[label=\"" << (*root) << "\"][shape=box];" << std::endl;

    const int parentIndex = index;
    BOOST_FOREACH(const SPTR(::gdcmIO::container::sr::DicomSRNode)& child, root->getCRefSubNodeContainer())
    {
        // Write edge
        out << "\t" << parentIndex  << "--" << (index+1) << "[label=\"" << child->getRefRelationship() << "\"];" << std::endl;

        // Write child
        StructuredReport::dumpSRNode(child, out, ++index);


    }
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace gdcmIO

