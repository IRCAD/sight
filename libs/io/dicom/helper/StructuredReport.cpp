/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/helper/StructuredReport.hpp"

#include "io/dicom/container/sr/DicomSRCodeNode.hpp"
#include "io/dicom/container/sr/DicomSRContainerNode.hpp"
#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRNumNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/container/sr/DicomSRUIDRefNode.hpp"
#include "io/dicom/helper/DicomDataReader.hxx"

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

SPTR(io::dicom::container::sr::DicomSRContainerNode) StructuredReport::readSR(const gdcm::DataSet& dataset)
{
    SPTR(io::dicom::container::sr::DicomSRContainerNode) result;

    // Value Type - Type 1
    const std::string type = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa040>(dataset);

    // Concept Name Code Sequence - Type 1C
    const io::dicom::container::DicomCodedAttribute& codedAttribute =
        io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(dataset);

    if(type == "CONTAINER" && !codedAttribute.isEmpty())
    {
        result = std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(codedAttribute);
        io::dicom::helper::StructuredReport::readSubNodeContainer(dataset, result);
    }

    return result;
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNodeContainer(
    const gdcm::DataSet& dataset,
    SPTR(io::dicom::container::sr::DicomSRNode)parent
)
{
    // Retrieve the content sequence
    if(dataset.FindDataElement(gdcm::Tag(0x0040, 0xa730)))
    {
        gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
            dataset.GetDataElement(gdcm::Tag(0x0040, 0xa730)).GetValueAsSQ();

        for(unsigned int i = 1 ; i <= sequence->GetNumberOfItems() ; ++i)
        {
            const gdcm::DataSet& itemDataset = sequence->GetItem(i).GetNestedDataSet();
            io::dicom::helper::StructuredReport::readSubNode(itemDataset, parent);
        }
    }
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNode(
    const gdcm::DataSet& dataset,
    SPTR(io::dicom::container::sr::DicomSRNode)parent
)
{
    SPTR(io::dicom::container::sr::DicomSRNode) node;

    // Value Type - Type 1
    const std::string type = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa040>(dataset);

    // Concept Name Code Sequence - Type 1C
    const io::dicom::container::DicomCodedAttribute& codedAttribute =
        io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(dataset);

    // Relationship Value - Type 1
    const std::string relationship = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa010>(dataset);

    if(type == "TEXT")
    {
        // Text Value - Type 1C
        const std::string textValue = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa160>(dataset);

        // Create Text Node
        node = std::make_shared<io::dicom::container::sr::DicomSRTextNode>(
            codedAttribute,
            relationship,
            textValue
        );
    }
    else if(type == "NUM")
    {
        // Retrieve the measured value sequence
        if(dataset.FindDataElement(gdcm::Tag(0x0040, 0xa300)))
        {
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
                dataset.GetDataElement(gdcm::Tag(0x0040, 0xa300)).GetValueAsSQ();
            if(sequence->GetNumberOfItems() > 0)
            {
                const gdcm::DataSet& itemDataset = sequence->GetItem(1).GetNestedDataSet();

                // Numerical value - Type 1
                const double numValue =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa30a, double>(itemDataset);

                // Measured units code sequence - Type 1
                const io::dicom::container::DicomCodedAttribute& measurementUnits =
                    io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0x08ea>(itemDataset);

                // Create Num Node
                node = std::make_shared<io::dicom::container::sr::DicomSRNumNode>(
                    codedAttribute,
                    relationship,
                    numValue,
                    measurementUnits
                );
            }
        }
    }
    else if(type == "SCOORD")
    {
        // Graphic Data - Type 1C
        gdcm::Attribute<0x0070, 0x0022> graphicDataAttribute;
        graphicDataAttribute.SetFromDataElement(dataset.GetDataElement(gdcm::Tag(0x0070, 0x0022)));
        const float* graphicData = graphicDataAttribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphicType = io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x0023>(dataset);

        if(graphicType == "POINT" && graphicDataAttribute.GetNumberOfValues() == 2)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 2);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                codedAttribute,
                relationship,
                graphicType,
                scoordVector
            );
        }
        else if(graphicType == "POLYLINE" && graphicDataAttribute.GetNumberOfValues() == 4)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 4);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                codedAttribute,
                relationship,
                graphicType,
                scoordVector
            );
        }
        else
        {
            SIGHT_WARN("Graphic type is not handled or data size doesn't match type. The SR node has been ignored.");
        }
    }
    else if(type == "SCOORD3D")
    {
        // Referenced Frame of Reference UID - Type 1
        const std::string frameOfReferenceUID =
            io::dicom::helper::DicomDataReader::getTagValue<0x3006, 0x0024>(dataset);

        // Graphic Data - Type 1C
        gdcm::Attribute<0x0070, 0x0022> graphicDataAttribute;
        graphicDataAttribute.SetFromDataElement(dataset.GetDataElement(gdcm::Tag(0x0070, 0x0022)));
        const float* graphicData = graphicDataAttribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphicType = io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x0023>(dataset);

        if(graphicType == "POINT" && graphicDataAttribute.GetNumberOfValues() == 3)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 3);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                codedAttribute,
                relationship,
                graphicType,
                scoordVector,
                frameOfReferenceUID
            );
        }
        else if(graphicType == "POLYLINE" && graphicDataAttribute.GetNumberOfValues() == 6)
        {
            // Create Text Node
            std::vector<float> scoordVector(graphicData, graphicData + 6);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                codedAttribute,
                relationship,
                graphicType,
                scoordVector,
                frameOfReferenceUID
            );
        }
        else
        {
            SIGHT_WARN("Graphic type is not handled or data size doesn't match type. The SR node has been ignored.");
        }
    }
    else if(type == "IMAGE")
    {
        // Retrieve the referenced SOP sequence
        if(dataset.FindDataElement(gdcm::Tag(0x0008, 0x1199)))
        {
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
                dataset.GetDataElement(gdcm::Tag(0x0008, 0x1199)).GetValueAsSQ();
            if(sequence->GetNumberOfItems() > 0)
            {
                const gdcm::DataSet& itemDataset = sequence->GetItem(1).GetNestedDataSet();

                // Referenced SOP Class UID - Type 1
                const std::string sopClassUID = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1150>(
                    itemDataset
                );

                // Referenced SOP Instance UID  - Type 1
                const std::string sopInstanceUID =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1155>(itemDataset);

                // Referenced Frame Number - Type 1C
                int frameNumber = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1160, int>(itemDataset);

                // Create Num Node
                node = std::make_shared<io::dicom::container::sr::DicomSRImageNode>(
                    codedAttribute,
                    relationship,
                    sopClassUID,
                    sopInstanceUID,
                    frameNumber
                );
            }
        }
    }
    else if(type == "CODE")
    {
        const io::dicom::container::DicomCodedAttribute& codedEntry =
            io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0xa168>(dataset);

        // Create Code Node
        node = std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(codedAttribute, relationship, codedEntry);
    }
    else if(type == "UIDREF")
    {
        // UID Value - Type 1C
        const std::string uidValue = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa124>(dataset);

        // Create UIDRef Node
        node = std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(codedAttribute, relationship, uidValue);
    }
    else if(type == "CONTAINER")
    {
        // Create Container Node
        node = std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(codedAttribute, relationship);
    }

    if(node)
    {
        // Create children
        io::dicom::helper::StructuredReport::readSubNodeContainer(dataset, node);

        // Add node to parent
        parent->addSubNode(node);
    }
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSR(const SPTR(io::dicom::container::sr::DicomSRNode)& root, std::ostream& out)
{
    out << "graph SR {" << std::endl;
    int index = 0;
    StructuredReport::dumpSRNode(root, out, index);
    out << "}" << std::endl;
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSRNode(
    const SPTR(io::dicom::container::sr::DicomSRNode)& root,
    std::ostream& out,
    int& index
)
{
    // Write node
    out << "\t" << index << "[label=\"" << (*root) << "\"][shape=box];" << std::endl;

    const int parentIndex = index;
    for(const SPTR(io::dicom::container::sr::DicomSRNode) & child : root->getSubNodeContainer())
    {
        // Write edge
        out << "\t" << parentIndex << "--" << (index + 1) << "[label=\""
        << child->getRelationship() << "\"];" << std::endl;

        // Write child
        StructuredReport::dumpSRNode(child, out, ++index);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
