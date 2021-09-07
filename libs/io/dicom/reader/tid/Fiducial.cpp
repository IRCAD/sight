/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Boolean.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

namespace sight::io::dicom
{

namespace reader
{

namespace tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(::gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Image::sptr& image,
    const core::log::Logger::sptr& logger
) :
    io::dicom::reader::tid::TemplateID<data::Image>(dicomSeries, reader, instance, image, logger)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
{
}

//------------------------------------------------------------------------------

void Fiducial::readNode(const SPTR(io::dicom::container::sr::DicomSRNode)& node)
{
    if(node->getCodedAttribute() == io::dicom::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature")
       && !node->getSubNodeContainer().empty())
    {
        std::string label = "";
        double x, y, z;
        bool foundLandmark = false;
        for(const SPTR(io::dicom::container::sr::DicomSRNode) & subNode : node->getSubNodeContainer())
        {
            // Read label
            if(subNode->getCodedAttribute()
               == io::dicom::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"))
            {
                SPTR(io::dicom::container::sr::DicomSRTextNode) intentNode =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRTextNode>(subNode);
                if(intentNode)
                {
                    label = intentNode->getTextValue();
                }
            }
            // 2D Coordinate
            else if(subNode->getType() == "SCOORD")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoordNode =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoordNode>(subNode);
                if(scoordNode && scoordNode->getGraphicType() == "POINT")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoordNode->getGraphicDataContainer();

                    x = coordinates[0];
                    y = coordinates[1];

                    if(!scoordNode->getSubNodeContainer().empty())
                    {
                        SPTR(io::dicom::container::sr::DicomSRImageNode) imageNode =
                            std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRImageNode>(
                                *scoordNode->getSubNodeContainer().begin()
                            );
                        if(imageNode)
                        {
                            const int frameNumber = imageNode->getFrameNumber();
                            z = io::dicom::helper::DicomDataTools::convertFrameNumberToZCoordinate(
                                m_object,
                                frameNumber
                            );
                            foundLandmark = true;
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(subNode->getType() == "SCOORD3D")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord3DNode =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoord3DNode>(subNode);
                if(scoord3DNode && scoord3DNode->getGraphicType() == "POINT")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoord3DNode->getGraphicDataContainer();
                    x             = coordinates[0];
                    y             = coordinates[1];
                    z             = coordinates[2];
                    foundLandmark = true;
                }
            }
        }

        if(foundLandmark)
        {
            this->addLandmark(x, y, z, label);
        }
    }
}

//------------------------------------------------------------------------------

void Fiducial::addLandmark(double x, double y, double z, const std::string& label)
{
    data::Point::sptr point = data::Point::New(x, y, z);
    point->setField(data::fieldHelper::Image::m_labelId, data::String::New(label));

    data::PointList::sptr pointList =
        m_object->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);

    if(!pointList)
    {
        pointList = data::PointList::New();
        m_object->setField(data::fieldHelper::Image::m_imageLandmarksId, pointList);
    }

    pointList->getPoints().push_back(point);
    m_object->setField("ShowLandmarks", data::Boolean::New(true));
}

//------------------------------------------------------------------------------

} // namespace tid

} // namespace reader

} // namespace sight::io::dicom
