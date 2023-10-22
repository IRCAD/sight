/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/writer/tid/Fiducial.hpp"

#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRCodeNode.hpp"
#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/container/sr/DicomSRUIDRefNode.hpp"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/helper/medical_image.hpp>
#include <data/point_list.hpp>
#include <data/series.hpp>
#include <data/string.hpp>
#include <data/types.hpp>
#include <data/vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const data::image::csptr& _image
) :
    io::dicom::writer::tid::TemplateID<data::image>(_writer, _instance, _image)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
= default;

//------------------------------------------------------------------------------

void Fiducial::createNodes(
    const SPTR(io::dicom::container::sr::DicomSRNode)& _parent,
    bool _use_s_coord3_d
)
{
    data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*m_object);
    if(point_list)
    {
        unsigned int id = 1;
        for(const data::point::sptr& point : point_list->getPoints())
        {
            this->createFiducial(_parent, point, id++, _use_s_coord3_d);
        }
    }
}

//------------------------------------------------------------------------------

void Fiducial::createFiducial(
    const SPTR(io::dicom::container::sr::DicomSRNode)& _parent,
    const data::point::csptr& _point,
    unsigned int _id,
    bool _use_s_coord3_d
)
{
    // Create Fiducial node
    SPTR(io::dicom::container::sr::DicomSRCodeNode) root_node =
        std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(
            io::dicom::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature"),
            "CONTAINS",
            io::dicom::container::DicomCodedAttribute("111123", "DCM", "Marker placement")
        ); //FIXME : Find a better
           // representation
    _parent->addSubNode(root_node);

    // Create Fiducial ID node
    SPTR(io::dicom::container::sr::DicomSRUIDRefNode) id_node =
        std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(
            io::dicom::container::DicomCodedAttribute(
                "dd1201",
                "DCM",
                "Fiducial ID"
            ),
            "HAS PROPERTIES",
            std::to_string(_id)
        );
    root_node->addSubNode(id_node);

    // Create Fiducial UID node
    gdcm::UIDGenerator generator;
    SPTR(io::dicom::container::sr::DicomSRUIDRefNode) uid_node =
        std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(
            io::dicom::container::DicomCodedAttribute("dd1202", "DCM", "Fiducial UID"),
            "HAS PROPERTIES",
            generator.Generate()
        );
    root_node->addSubNode(uid_node);

    // Create Fiducial intent node
    const std::string label = _point->getLabel();
    SPTR(io::dicom::container::sr::DicomSRTextNode) intent_node =
        std::make_shared<io::dicom::container::sr::DicomSRTextNode>(
            io::dicom::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"),
            "HAS PROPERTIES",
            label
        );
    root_node->addSubNode(intent_node);

    if(_use_s_coord3_d)
    {
        // Create SCoord3D Node
        std::vector<float> scoord_vector {
            static_cast<float>(_point->getCoord()[0]),
            static_cast<float>(_point->getCoord()[1]),
            static_cast<float>(_point->getCoord()[2])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord3_d_node =
            std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                io::dicom::container::DicomCodedAttribute(),
                "HAS PROPERTIES",
                "POINT",
                scoord_vector,
                m_instance->getSOPInstanceUIDContainer()[0]
            );
        root_node->addSubNode(scoord3_d_node);
    }
    else
    {
        // Create SCoord Node
        std::vector<float> scoord_vector {
            static_cast<float>(_point->getCoord()[0]),
            static_cast<float>(_point->getCoord()[1])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoord_node =
            std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                io::dicom::container::DicomCodedAttribute(),
                "HAS PROPERTIES",
                "POINT",
                scoord_vector
            );
        root_node->addSubNode(scoord_node);

        // Create Image Node
        const std::size_t frame_number = io::dicom::helper::DicomDataTools::convertPointToFrameNumber(m_object, _point);
        SPTR(io::dicom::container::sr::DicomSRImageNode) image_node =
            std::make_shared<io::dicom::container::sr::DicomSRImageNode>(
                io::dicom::container::DicomCodedAttribute(),
                "SELECTED FROM",
                m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frame_number - 1],
                int(frame_number)
            );
        scoord_node->addSubNode(image_node);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
