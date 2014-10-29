/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/Dictionary.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include "gdcmIO/container/sr/DicomSRImageNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/reader/tid/Fiducial.hpp"

namespace gdcmIO
{
namespace reader
{
namespace tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(SPTR(::fwDicomData::DicomSeries) dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwData::Image::sptr image):
        ::gdcmIO::reader::tid::TemplateID< ::fwData::Image >(dicomSeries, reader, instance, image)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
{
}

//------------------------------------------------------------------------------

void Fiducial::readNode(SPTR(::gdcmIO::container::sr::DicomSRNode) node)
{
    if(node->getCodedAttribute() == ::gdcmIO::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature") &&
            !node->getCRefSubNodeContainer().empty())
    {
        std::string label = "";
        float x, y, z;
        bool foundLandmark = false;
        BOOST_FOREACH(const SPTR(::gdcmIO::container::sr::DicomSRNode)& subNode, node->getCRefSubNodeContainer())
        {
            // Read label
            if(subNode->getCodedAttribute() == ::gdcmIO::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"))
            {
                SPTR(::gdcmIO::container::sr::DicomSRTextNode) intentNode =
                        ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRTextNode >(subNode);
                if(intentNode)
                {
                    label = intentNode->getTextValue();
                }
            }
            // 2D Coordinate
            else if(subNode->getType() == "SCOORD")
            {
                SPTR(::gdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
                        ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRSCoordNode >(subNode);
                if(scoordNode && scoordNode->getCRefGraphicType() == "POINT")
                {
                    // Retrieve coordinates
                    ::gdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                            scoordNode->getGraphicDataContainer();

                    x = coordinates[0];
                    y = coordinates[1];

                    if(!scoordNode->getCRefSubNodeContainer().empty())
                    {
                        SPTR(::gdcmIO::container::sr::DicomSRImageNode) imageNode =
                                ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRImageNode >(
                                        *scoordNode->getCRefSubNodeContainer().begin());
                        if(imageNode)
                        {
                            const int frameNumber = imageNode->getCRefFrameNumber();
                            z = ::gdcmIO::helper::DicomData::convertFrameNumberToZCoordinate(m_object, frameNumber);
                            foundLandmark = true;
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(subNode->getType() == "SCOORD3D")
            {
                SPTR(::gdcmIO::container::sr::DicomSRSCoord3DNode) scoord3DNode =
                        ::boost::dynamic_pointer_cast< ::gdcmIO::container::sr::DicomSRSCoord3DNode >(subNode);
                if(scoord3DNode && scoord3DNode->getCRefGraphicType() == "POINT")
                {
                    // Retrieve coordinates
                    ::gdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                            scoord3DNode->getGraphicDataContainer();
                    x = coordinates[0];
                    y = coordinates[1];
                    z = coordinates[2];
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

void Fiducial::addLandmark(float x, float y, float z, const std::string& label)
{
    ::fwData::Point::sptr point = ::fwData::Point::New(x,y,z);
    point->setField(::fwComEd::Dictionary::m_labelId, ::fwData::String::New(label));

    ::fwData::PointList::sptr pointList =
                m_object->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);

    if(!pointList)
    {
        pointList = ::fwData::PointList::New();
        m_object->setField(::fwComEd::Dictionary::m_imageLandmarksId, pointList);
    }

    pointList->getRefPoints().push_back(point);
    m_object->setField("ShowLandmarks", ::fwData::Boolean::New(true));
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace reader
} // namespace gdcmIO
