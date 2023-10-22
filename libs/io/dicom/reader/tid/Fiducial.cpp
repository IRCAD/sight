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

#include "io/dicom/reader/tid/Fiducial.hpp"

#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

namespace sight::io::dicom::reader::tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::DicomInstance::sptr& _instance,
    const data::image::sptr& _image,
    const core::log::logger::sptr& _logger
) :
    io::dicom::reader::tid::TemplateID<data::image>(_dicom_series, _reader, _instance, _image, _logger)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
= default;

//------------------------------------------------------------------------------

void Fiducial::readNode(const SPTR(io::dicom::container::sr::DicomSRNode)& _node)
{
    if(_node->getCodedAttribute() == io::dicom::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature")
       && !_node->getSubNodeContainer().empty())
    {
        std::string label;
        double x            = 0;
        double y            = 0;
        double z            = 0;
        bool found_landmark = false;
        for(const SPTR(io::dicom::container::sr::DicomSRNode) & sub_node : _node->getSubNodeContainer())
        {
            // Read label
            if(sub_node->getCodedAttribute()
               == io::dicom::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"))
            {
                SPTR(io::dicom::container::sr::DicomSRTextNode) intent_node =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRTextNode>(sub_node);
                if(intent_node)
                {
                    label = intent_node->getTextValue();
                }
            }
            // 2D Coordinate
            else if(sub_node->getType() == "SCOORD")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoord_node =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoordNode>(sub_node);
                if(scoord_node && scoord_node->get_graphic_type() == "POINT")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::graphic_data_container_t coordinates =
                        scoord_node->getGraphicDataContainer();

                    x = coordinates[0];
                    y = coordinates[1];

                    if(!scoord_node->getSubNodeContainer().empty())
                    {
                        SPTR(io::dicom::container::sr::DicomSRImageNode) image_node =
                            std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRImageNode>(
                                *scoord_node->getSubNodeContainer().begin()
                            );
                        if(image_node)
                        {
                            const int frame_number = image_node->getFrameNumber();
                            z = io::dicom::helper::DicomDataTools::convertFrameNumberToZCoordinate(
                                m_object,
                                std::size_t(frame_number)
                            );
                            found_landmark = true;
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(sub_node->getType() == "SCOORD3D")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord3_d_node =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoord3DNode>(sub_node);
                if(scoord3_d_node && scoord3_d_node->get_graphic_type() == "POINT")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::graphic_data_container_t coordinates =
                        scoord3_d_node->getGraphicDataContainer();
                    x              = coordinates[0];
                    y              = coordinates[1];
                    z              = coordinates[2];
                    found_landmark = true;
                }
            }
        }

        if(found_landmark)
        {
            this->addLandmark(x, y, z, label);
        }
    }
}

//------------------------------------------------------------------------------

void Fiducial::addLandmark(double _x, double _y, double _z, const std::string& _label)
{
    data::point::sptr point = std::make_shared<data::point>(_x, _y, _z);
    point->setLabel(_label);

    data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*m_object);

    if(!point_list)
    {
        point_list = std::make_shared<data::point_list>();
        data::helper::medical_image::set_landmarks(*m_object, point_list);
    }

    point_list->getPoints().push_back(point);
    data::helper::medical_image::set_landmarks_visibility(*m_object, true);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::tid
