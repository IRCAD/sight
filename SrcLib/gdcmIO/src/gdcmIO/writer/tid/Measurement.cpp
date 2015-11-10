/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <fwComEd/Dictionary.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>
#include <fwTools/Stringizer.hpp>

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRNumNode.hpp"
#include "gdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "gdcmIO/container/sr/DicomSRImageNode.hpp"
#include "gdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/writer/tid/Measurement.hpp"

namespace gdcmIO
{
namespace writer
{
namespace tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(SPTR(::gdcm::Writer)writer,
                         SPTR(::gdcmIO::container::DicomInstance)instance,
                         ::fwData::Image::sptr image) :
    ::gdcmIO::writer::tid::TemplateID< ::fwData::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
{
}

//------------------------------------------------------------------------------

void Measurement::createNodes(SPTR(::gdcmIO::container::sr::DicomSRNode)parent, bool useSCoord3D)
{
    ::fwData::Vector::sptr distanceVector =
        m_object->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
    if (distanceVector)
    {
        unsigned int id = 1;
        BOOST_FOREACH(::fwData::Object::sptr object, distanceVector->getContainer())
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

void Measurement::createMeasurement(SPTR(::gdcmIO::container::sr::DicomSRNode)parent,
                                    const ::fwData::PointList::sptr& pointList, unsigned int id, bool useSCoord3D)
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    const ::fwData::Point::sptr point1 = pointList->getPoints()[0];
    const ::fwData::Point::sptr point2 = pointList->getPoints()[1];

    float coordinates[6];
    coordinates[0] = point1->getCoord()[0];
    coordinates[1] = point1->getCoord()[1];
    coordinates[2] = point1->getCoord()[2];
    coordinates[3] = point2->getCoord()[0];
    coordinates[4] = point2->getCoord()[1];
    coordinates[5] = point2->getCoord()[2];

    float distance = sqrt( (coordinates[0] - coordinates[3]) * (coordinates[0] - coordinates[3]) +
                           (coordinates[1] - coordinates[4]) * (coordinates[1] - coordinates[4]) +
                           (coordinates[2] - coordinates[5]) * (coordinates[2] - coordinates[5]) );

    // Retrieve Frame Numbers
    int frameNumber1 = ::gdcmIO::helper::DicomData::convertPointToFrameNumber(m_object, point1);
    int frameNumber2 = ::gdcmIO::helper::DicomData::convertPointToFrameNumber(m_object, point2);

    // Create Measurement Node
    SPTR(::gdcmIO::container::sr::DicomSRNumNode) numNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRNumNode >(
            ::gdcmIO::container::DicomCodedAttribute("121206", "DCM", "Distance"), "CONTAINS", distance,
            ::gdcmIO::container::DicomCodedAttribute("mm", "UCUM", "millimeter", "1.4"));
    parent->addSubNode(numNode);

    if(useSCoord3D)
    {
        // Create SCoord Node

        const float scoord[] = { static_cast<float>(point1->getCoord()[0]),
                                 static_cast<float>(point1->getCoord()[1]),
                                 static_cast<float>(point1->getCoord()[2]),
                                 static_cast<float>(point2->getCoord()[0]),
                                 static_cast<float>(point2->getCoord()[1]),
                                 static_cast<float>(point2->getCoord()[2]) };
        std::vector<float> scoordVector (scoord, scoord + 6);
        SPTR(::gdcmIO::container::sr::DicomSRSCoord3DNode) scoordNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRSCoord3DNode >(
                ::gdcmIO::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM", "POLYLINE", scoordVector, m_instance->getSOPInstanceUIDContainer()[0]);
        numNode->addSubNode(scoordNode);
    }
    else
    {
        SLM_ASSERT("Unable to save a 3D distance using a SCOORD object.", frameNumber1 == frameNumber2);

        // Create SCoord Node
        const float scoord[] = { static_cast<float>(point1->getCoord()[0]),
                                 static_cast<float>(point1->getCoord()[1]),
                                 static_cast<float>(point2->getCoord()[0]),
                                 static_cast<float>(point2->getCoord()[1]) };
        std::vector<float> scoordVector (scoord, scoord + 4);
        SPTR(::gdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRSCoordNode >(
                ::gdcmIO::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM", "POLYLINE", scoordVector);
        numNode->addSubNode(scoordNode);

        // Create Image Node
        SPTR(::gdcmIO::container::sr::DicomSRImageNode) imageNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRImageNode >(
                ::gdcmIO::container::DicomCodedAttribute(), "SELECTED FROM", m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frameNumber1-1], frameNumber1);
        scoordNode->addSubNode(imageNode);
    }



}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace gdcmIO
