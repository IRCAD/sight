/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/writer/tid/Measurement.hpp"

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRNumNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "fwGdcmIO/helper/DicomDataTools.hpp"

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <fwTools/Stringizer.hpp>

#include <boost/algorithm/string/split.hpp>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(const SPTR(::gdcm::Writer)& writer,
                         const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                         const ::fwData::Image::sptr& image) :
    ::fwGdcmIO::writer::tid::TemplateID< ::fwData::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
{
}

//------------------------------------------------------------------------------

void Measurement::createNodes(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                              bool useSCoord3D)
{
    ::fwData::Vector::sptr distanceVector =
        m_object->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);
    if (distanceVector)
    {
        unsigned int id = 1;
        for(::fwData::Object::sptr object : distanceVector->getContainer())
        {
            ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(object);
            if(pointList)
            {
                this->createMeasurement(parent, pointList, id++, useSCoord3D);
            }
        }
    }
}

//------------------------------------------------------------------------------

void Measurement::createMeasurement(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                                    const ::fwData::PointList::sptr& pointList,
                                    unsigned int id,
                                    bool useSCoord3D)
{
    const ::fwData::Point::sptr point1 = pointList->getPoints()[0];
    const ::fwData::Point::sptr point2 = pointList->getPoints()[1];

    double coordinates[6];
    coordinates[0] = point1->getCoord()[0];
    coordinates[1] = point1->getCoord()[1];
    coordinates[2] = point1->getCoord()[2];
    coordinates[3] = point2->getCoord()[0];
    coordinates[4] = point2->getCoord()[1];
    coordinates[5] = point2->getCoord()[2];

    const double distance = sqrt( (coordinates[0] - coordinates[3]) * (coordinates[0] - coordinates[3]) +
                                  (coordinates[1] - coordinates[4]) * (coordinates[1] - coordinates[4]) +
                                  (coordinates[2] - coordinates[5]) * (coordinates[2] - coordinates[5]) );

    // Retrieve Frame Numbers
    const std::size_t frameNumber1 = ::fwGdcmIO::helper::DicomDataTools::convertPointToFrameNumber(m_object, point1);

    // Create Measurement Node
    SPTR(::fwGdcmIO::container::sr::DicomSRNumNode) numNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRNumNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("121206", "DCM", "Distance"), "CONTAINS", distance,
            ::fwGdcmIO::container::DicomCodedAttribute("mm", "UCUM", "millimeter", "1.4"));
    parent->addSubNode(numNode);

    if(useSCoord3D)
    {
        // Create SCoord Node
        const float scoord[] = {
            static_cast<float>(point1->getCoord()[0]),
            static_cast<float>(point1->getCoord()[1]),
            static_cast<float>(point1->getCoord()[2]),
            static_cast<float>(point2->getCoord()[0]),
            static_cast<float>(point2->getCoord()[1]),
            static_cast<float>(point2->getCoord()[2])
        };
        std::vector<float> scoordVector(scoord, scoord + 6);
        SPTR(::fwGdcmIO::container::sr::DicomSRSCoord3DNode) scoordNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoord3DNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM", "POLYLINE", scoordVector, m_instance->getSOPInstanceUIDContainer()[0]);
        numNode->addSubNode(scoordNode);
    }
    else
    {
        SLM_ASSERT("Unable to save a 3D distance using a SCOORD object.",
                   frameNumber1 == ::fwGdcmIO::helper::DicomDataTools::convertPointToFrameNumber(m_object, point2));

        // Create SCoord Node
        const float scoord[] = {
            static_cast<float>(point1->getCoord()[0]),
            static_cast<float>(point1->getCoord()[1]),
            static_cast<float>(point2->getCoord()[0]),
            static_cast<float>(point2->getCoord()[1])
        };
        std::vector<float> scoordVector(scoord, scoord + 4);
        SPTR(::fwGdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoordNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM", "POLYLINE", scoordVector);
        numNode->addSubNode(scoordNode);

        // Create Image Node
        SPTR(::fwGdcmIO::container::sr::DicomSRImageNode) imageNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRImageNode >(
                ::fwGdcmIO::container::DicomCodedAttribute(), "SELECTED FROM", m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frameNumber1-1], frameNumber1);
        scoordNode->addSubNode(imageNode);
    }

}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace fwGdcmIO
