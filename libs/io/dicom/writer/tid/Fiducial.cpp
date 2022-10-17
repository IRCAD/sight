/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/helper/MedicalImage.hpp>
#include <data/PointList.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>
#include <data/types.hpp>
#include <data/Vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(
    const SPTR(gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const data::Image::csptr& image
) :
    io::dicom::writer::tid::TemplateID<data::Image>(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
= default;

//------------------------------------------------------------------------------

void Fiducial::createNodes(
    const SPTR(io::dicom::container::sr::DicomSRNode)& parent,
    bool useSCoord3D
)
{
    data::PointList::sptr pointList = data::helper::MedicalImage::getLandmarks(*m_object);
    if(pointList)
    {
        unsigned int id = 1;
        for(const data::Point::sptr& point : pointList->getPoints())
        {
            this->createFiducial(parent, point, id++, useSCoord3D);
        }
    }
}

//------------------------------------------------------------------------------

void Fiducial::createFiducial(
    const SPTR(io::dicom::container::sr::DicomSRNode)& parent,
    const data::Point::csptr& point,
    unsigned int id,
    bool useSCoord3D
)
{
    // Create Fiducial node
    SPTR(io::dicom::container::sr::DicomSRCodeNode) rootNode =
        std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(
            io::dicom::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature"),
            "CONTAINS",
            io::dicom::container::DicomCodedAttribute("111123", "DCM", "Marker placement")
        ); //FIXME : Find a better
           // representation
    parent->addSubNode(rootNode);

    // Create Fiducial ID node
    SPTR(io::dicom::container::sr::DicomSRUIDRefNode) idNode =
        std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(
            io::dicom::container::DicomCodedAttribute(
                "dd1201",
                "DCM",
                "Fiducial ID"
            ),
            "HAS PROPERTIES",
            std::to_string(id)
        );
    rootNode->addSubNode(idNode);

    // Create Fiducial UID node
    gdcm::UIDGenerator generator;
    SPTR(io::dicom::container::sr::DicomSRUIDRefNode) uidNode =
        std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(
            io::dicom::container::DicomCodedAttribute("dd1202", "DCM", "Fiducial UID"),
            "HAS PROPERTIES",
            generator.Generate()
        );
    rootNode->addSubNode(uidNode);

    // Create Fiducial intent node
    const std::string label = point->getLabel();
    SPTR(io::dicom::container::sr::DicomSRTextNode) intentNode =
        std::make_shared<io::dicom::container::sr::DicomSRTextNode>(
            io::dicom::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"),
            "HAS PROPERTIES",
            label
        );
    rootNode->addSubNode(intentNode);

    if(useSCoord3D)
    {
        // Create SCoord3D Node
        std::vector<float> scoordVector {
            static_cast<float>(point->getCoord()[0]),
            static_cast<float>(point->getCoord()[1]),
            static_cast<float>(point->getCoord()[2])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord3DNode =
            std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                io::dicom::container::DicomCodedAttribute(),
                "HAS PROPERTIES",
                "POINT",
                scoordVector,
                m_instance->getSOPInstanceUIDContainer()[0]
            );
        rootNode->addSubNode(scoord3DNode);
    }
    else
    {
        // Create SCoord Node
        std::vector<float> scoordVector {
            static_cast<float>(point->getCoord()[0]),
            static_cast<float>(point->getCoord()[1])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoordNode =
            std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                io::dicom::container::DicomCodedAttribute(),
                "HAS PROPERTIES",
                "POINT",
                scoordVector
            );
        rootNode->addSubNode(scoordNode);

        // Create Image Node
        const std::size_t frameNumber = io::dicom::helper::DicomDataTools::convertPointToFrameNumber(m_object, point);
        SPTR(io::dicom::container::sr::DicomSRImageNode) imageNode =
            std::make_shared<io::dicom::container::sr::DicomSRImageNode>(
                io::dicom::container::DicomCodedAttribute(),
                "SELECTED FROM",
                m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frameNumber - 1],
                int(frameNumber)
            );
        scoordNode->addSubNode(imageNode);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
