/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

SPTR(io::dicom::container::sr::DicomSRContainerNode) StructuredReport::readSR(const gdcm::DataSet& _dataset)
{
    SPTR(io::dicom::container::sr::DicomSRContainerNode) result;

    // Value Type - Type 1
    const std::string type = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa040>(_dataset);

    // Concept Name Code Sequence - Type 1C
    const io::dicom::container::DicomCodedAttribute& coded_attribute =
        io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(_dataset);

    if(type == "CONTAINER" && !coded_attribute.isEmpty())
    {
        result = std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(coded_attribute);
        io::dicom::helper::StructuredReport::readSubNodeContainer(_dataset, result);
    }

    return result;
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNodeContainer(
    const gdcm::DataSet& _dataset,
    SPTR(io::dicom::container::sr::DicomSRNode)_parent
)
{
    // Retrieve the content sequence
    if(_dataset.FindDataElement(gdcm::Tag(0x0040, 0xa730)))
    {
        gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
            _dataset.GetDataElement(gdcm::Tag(0x0040, 0xa730)).GetValueAsSQ();

        for(unsigned int i = 1 ; i <= sequence->GetNumberOfItems() ; ++i)
        {
            const gdcm::DataSet& item_dataset = sequence->GetItem(i).GetNestedDataSet();
            io::dicom::helper::StructuredReport::readSubNode(item_dataset, _parent);
        }
    }
}

//------------------------------------------------------------------------------

void StructuredReport::readSubNode(
    const gdcm::DataSet& _dataset,
    SPTR(io::dicom::container::sr::DicomSRNode)_parent
)
{
    SPTR(io::dicom::container::sr::DicomSRNode) node;

    // Value Type - Type 1
    const std::string type = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa040>(_dataset);

    // Concept Name Code Sequence - Type 1C
    const io::dicom::container::DicomCodedAttribute& coded_attribute =
        io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0xa043>(_dataset);

    // Relationship Value - Type 1
    const std::string relationship = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa010>(_dataset);

    if(type == "TEXT")
    {
        // Text Value - Type 1C
        const std::string text_value = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa160>(_dataset);

        // Create Text Node
        node = std::make_shared<io::dicom::container::sr::DicomSRTextNode>(
            coded_attribute,
            relationship,
            text_value
        );
    }
    else if(type == "NUM")
    {
        // Retrieve the measured value sequence
        if(_dataset.FindDataElement(gdcm::Tag(0x0040, 0xa300)))
        {
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
                _dataset.GetDataElement(gdcm::Tag(0x0040, 0xa300)).GetValueAsSQ();
            if(sequence->GetNumberOfItems() > 0)
            {
                const gdcm::DataSet& item_dataset = sequence->GetItem(1).GetNestedDataSet();

                // Numerical value - Type 1
                const double num_value =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa30a, double>(item_dataset);

                // Measured units code sequence - Type 1
                const io::dicom::container::DicomCodedAttribute& measurement_units =
                    io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0x08ea>(item_dataset);

                // Create Num Node
                node = std::make_shared<io::dicom::container::sr::DicomSRNumNode>(
                    coded_attribute,
                    relationship,
                    num_value,
                    measurement_units
                );
            }
        }
    }
    else if(type == "SCOORD")
    {
        // Graphic Data - Type 1C
        gdcm::Attribute<0x0070, 0x0022> graphic_data_attribute;
        graphic_data_attribute.SetFromDataElement(_dataset.GetDataElement(gdcm::Tag(0x0070, 0x0022)));
        const float* graphic_data = graphic_data_attribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphic_type = io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x0023>(_dataset);

        if(graphic_type == "POINT" && graphic_data_attribute.GetNumberOfValues() == 2)
        {
            // Create Text Node
            std::vector<float> scoord_vector(graphic_data, graphic_data + 2);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                coded_attribute,
                relationship,
                graphic_type,
                scoord_vector
            );
        }
        else if(graphic_type == "POLYLINE" && graphic_data_attribute.GetNumberOfValues() == 4)
        {
            // Create Text Node
            std::vector<float> scoord_vector(graphic_data, graphic_data + 4);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                coded_attribute,
                relationship,
                graphic_type,
                scoord_vector
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
        const std::string frame_of_reference_uid =
            io::dicom::helper::DicomDataReader::getTagValue<0x3006, 0x0024>(_dataset);

        // Graphic Data - Type 1C
        gdcm::Attribute<0x0070, 0x0022> graphic_data_attribute;
        graphic_data_attribute.SetFromDataElement(_dataset.GetDataElement(gdcm::Tag(0x0070, 0x0022)));
        const float* graphic_data = graphic_data_attribute.GetValues();

        // Graphic Type - Type 1
        const std::string graphic_type = io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x0023>(_dataset);

        if(graphic_type == "POINT" && graphic_data_attribute.GetNumberOfValues() == 3)
        {
            // Create Text Node
            std::vector<float> scoord_vector(graphic_data, graphic_data + 3);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                coded_attribute,
                relationship,
                graphic_type,
                scoord_vector,
                frame_of_reference_uid
            );
        }
        else if(graphic_type == "POLYLINE" && graphic_data_attribute.GetNumberOfValues() == 6)
        {
            // Create Text Node
            std::vector<float> scoord_vector(graphic_data, graphic_data + 6);
            node = std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                coded_attribute,
                relationship,
                graphic_type,
                scoord_vector,
                frame_of_reference_uid
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
        if(_dataset.FindDataElement(gdcm::Tag(0x0008, 0x1199)))
        {
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
                _dataset.GetDataElement(gdcm::Tag(0x0008, 0x1199)).GetValueAsSQ();
            if(sequence->GetNumberOfItems() > 0)
            {
                const gdcm::DataSet& item_dataset = sequence->GetItem(1).GetNestedDataSet();

                // Referenced SOP Class UID - Type 1
                const std::string sop_class_uid = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1150>(
                    item_dataset
                );

                // Referenced SOP Instance UID  - Type 1
                const std::string sop_instance_uid =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1155>(item_dataset);

                // Referenced Frame Number - Type 1C
                int frame_number = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1160, int>(item_dataset);

                // Create Num Node
                node = std::make_shared<io::dicom::container::sr::DicomSRImageNode>(
                    coded_attribute,
                    relationship,
                    sop_class_uid,
                    sop_instance_uid,
                    frame_number
                );
            }
        }
    }
    else if(type == "CODE")
    {
        const io::dicom::container::DicomCodedAttribute& coded_entry =
            io::dicom::helper::StructuredReport::readCodeSequence<0x0040, 0xa168>(_dataset);

        // Create Code Node
        node = std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(coded_attribute, relationship, coded_entry);
    }
    else if(type == "UIDREF")
    {
        // UID Value - Type 1C
        const std::string uid_value = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xa124>(_dataset);

        // Create UIDRef Node
        node = std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(coded_attribute, relationship, uid_value);
    }
    else if(type == "CONTAINER")
    {
        // Create Container Node
        node = std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(coded_attribute, relationship);
    }

    if(node)
    {
        // Create children
        io::dicom::helper::StructuredReport::readSubNodeContainer(_dataset, node);

        // Add node to parent
        _parent->addSubNode(node);
    }
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSR(const SPTR(io::dicom::container::sr::DicomSRNode)& _root, std::ostream& _out)
{
    _out << "graph SR {" << std::endl;
    int index = 0;
    StructuredReport::dumpSRNode(_root, _out, index);
    _out << "}" << std::endl;
}

//------------------------------------------------------------------------------

void StructuredReport::dumpSRNode(
    const SPTR(io::dicom::container::sr::DicomSRNode)& _root,
    std::ostream& _out,
    int& _index
)
{
    // Write node
    _out << "\t" << _index << "[label=\"" << (*_root) << "\"][shape=box];" << std::endl;

    const int parent_index = _index;
    for(const SPTR(io::dicom::container::sr::DicomSRNode) & child : _root->getSubNodeContainer())
    {
        // Write edge
        _out << "\t" << parent_index << "--" << (_index + 1) << "[label=\""
        << child->getRelationship() << "\"];" << std::endl;

        // Write child
        StructuredReport::dumpSRNode(child, _out, ++_index);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
