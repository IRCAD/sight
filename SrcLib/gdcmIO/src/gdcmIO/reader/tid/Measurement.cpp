/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/Dictionary.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include "gdcmIO/container/sr/DicomSRImageNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/reader/tid/Measurement.hpp"

namespace gdcmIO
{
namespace reader
{
namespace tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                         SPTR(::gdcm::Reader)reader,
                         SPTR(::gdcmIO::container::DicomInstance)instance,
                         ::fwData::Image::sptr image) :
    ::gdcmIO::reader::tid::TemplateID< ::fwData::Image >(dicomSeries, reader, instance, image)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
{
}

//------------------------------------------------------------------------------

void Measurement::readNode(SPTR(::gdcmIO::container::sr::DicomSRNode)node)
{
    if(node->getCodedAttribute() == ::gdcmIO::container::DicomCodedAttribute("121206", "DCM", "Distance") &&
       !node->getSubNodeContainer().empty())
    {
        for(const SPTR(::gdcmIO::container::sr::DicomSRNode)& subNode : node->getSubNodeContainer())
        {
            if(subNode->getType() == "SCOORD")
            {
                SPTR(::gdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
                    ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRSCoordNode >(subNode);
                if(scoordNode && scoordNode->getGraphicType() == "POLYLINE")
                {
                    // Retrieve coordinates
                    ::gdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoordNode->getGraphicDataContainer();

                    if(!scoordNode->getSubNodeContainer().empty())
                    {
                        SPTR(::gdcmIO::container::sr::DicomSRImageNode) imageNode =
                            ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRImageNode >(
                                *scoordNode->getSubNodeContainer().begin());
                        if(imageNode)
                        {
                            const int frameNumber = imageNode->getFrameNumber();
                            float zCoordinate     = ::gdcmIO::helper::DicomData::convertFrameNumberToZCoordinate(
                                m_object, frameNumber);
                            this->addDistance(::fwData::Point::New(coordinates[0], coordinates[1], zCoordinate),
                                              ::fwData::Point::New(coordinates[2], coordinates[3], zCoordinate));
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(subNode->getType() == "SCOORD3D")
            {
                SPTR(::gdcmIO::container::sr::DicomSRSCoord3DNode) scoord3DNode =
                    ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRSCoord3DNode >(subNode);
                if(scoord3DNode && scoord3DNode->getGraphicType() == "POLYLINE")
                {
                    // Retrieve coordinates
                    ::gdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
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
} // namespace gdcmIO
