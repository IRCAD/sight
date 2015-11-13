/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/reader/tid/Measurement.hpp"

#include <fwComEd/Dictionary.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                         SPTR(::gdcm::Reader)reader,
                         SPTR(::fwGdcmIO::container::DicomInstance)instance,
                         ::fwData::Image::sptr image,
                         ::fwLog::Logger::sptr logger) :
    ::fwGdcmIO::reader::tid::TemplateID< ::fwData::Image >(dicomSeries, reader, instance, image, logger)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
{
}

//------------------------------------------------------------------------------

void Measurement::readNode(SPTR(::fwGdcmIO::container::sr::DicomSRNode)node)
{
    if(node->getCodedAttribute() == ::fwGdcmIO::container::DicomCodedAttribute("121206", "DCM", "Distance") &&
       !node->getSubNodeContainer().empty())
    {
        for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& subNode: node->getSubNodeContainer())
        {
            if(subNode->getType() == "SCOORD")
            {
                SPTR(::fwGdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
                    std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRSCoordNode >(subNode);
                if(scoordNode && scoordNode->getGraphicType() == "POLYLINE")
                {
                    // Retrieve coordinates
                    ::fwGdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoordNode->getGraphicDataContainer();

                    if(!scoordNode->getSubNodeContainer().empty())
                    {
                        SPTR(::fwGdcmIO::container::sr::DicomSRImageNode) imageNode =
                            std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRImageNode >(
                                *scoordNode->getSubNodeContainer().begin());
                        if(imageNode)
                        {
                            const int frameNumber = imageNode->getFrameNumber();
                            double zCoordinate    = ::fwGdcmIO::helper::DicomData::convertFrameNumberToZCoordinate(
                                m_object, frameNumber);

                            auto origin = ::fwData::Point::New(static_cast<double>(coordinates[0]),
                                                               static_cast<double>(coordinates[1]), zCoordinate);
                            auto destination = ::fwData::Point::New(static_cast<double>(coordinates[2]),
                                                                    static_cast<double>(coordinates[3]), zCoordinate);
                            this->addDistance(origin, destination);
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(subNode->getType() == "SCOORD3D")
            {
                SPTR(::fwGdcmIO::container::sr::DicomSRSCoord3DNode) scoord3DNode =
                    std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRSCoord3DNode >(subNode);
                if(scoord3DNode && scoord3DNode->getGraphicType() == "POLYLINE")
                {
                    // Retrieve coordinates
                    ::fwGdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoord3DNode->getGraphicDataContainer();
                    this->addDistance(::fwData::Point::New(coordinates[0], coordinates[1], coordinates[2]),
                                      ::fwData::Point::New(coordinates[3], coordinates[4], coordinates[5]));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Measurement::addDistance(SPTR(::fwData::Point)point1, SPTR(::fwData::Point)point2)
{
    ::fwData::Vector::sptr distanceVector =
        m_object->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);

    if(!distanceVector)
    {
        distanceVector = ::fwData::Vector::New();
        m_object->setField(::fwComEd::Dictionary::m_imageDistancesId, distanceVector);
    }

    ::fwData::PointList::sptr pointList = ::fwData::PointList::New();
    pointList->getRefPoints().push_back(point1);
    pointList->getRefPoints().push_back(point2);

    distanceVector->getContainer().push_back(pointList);
    m_object->setField("ShowDistances", ::fwData::Boolean::New(true));
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO
