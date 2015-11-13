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
#include "gdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "gdcmIO/container/sr/DicomSRImageNode.hpp"
#include "gdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/writer/tid/Fiducial.hpp"

namespace gdcmIO
{
namespace writer
{
namespace tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(SPTR(::gdcm::Writer)writer,
                   SPTR(::gdcmIO::container::DicomInstance)instance,
                   ::fwData::Image::sptr image) :
    ::gdcmIO::writer::tid::TemplateID< ::fwData::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
{
}

//------------------------------------------------------------------------------

void Fiducial::createNodes(SPTR(::gdcmIO::container::sr::DicomSRNode)parent, bool useSCoord3D)
{
    ::fwData::PointList::sptr pointList =
        m_object->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
    if (pointList)
    {
        unsigned int id = 1;
        for(const ::fwData::Point::sptr& point :  pointList->getRefPoints())
        {
            this->createFiducial(parent, point, id++, useSCoord3D);
        }
    }
}

//------------------------------------------------------------------------------

void Fiducial::createFiducial(SPTR(::gdcmIO::container::sr::DicomSRNode)parent, const ::fwData::Point::sptr& point,
                              unsigned int id, bool useSCoord3D)
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // Create Fiducial node
    SPTR(::gdcmIO::container::sr::DicomSRCodeNode) rootNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRCodeNode >(
            ::gdcmIO::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature"), "CONTAINS",
            ::gdcmIO::container::DicomCodedAttribute("111123", "DCM", "Marker placement"));     //FIXME : Find a better representation
    parent->addSubNode(rootNode);

    // Create Fiducial ID node
    SPTR(::gdcmIO::container::sr::DicomSRUIDRefNode) idNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRUIDRefNode >(
            ::gdcmIO::container::DicomCodedAttribute("dd1201", "DCM",
                                                     "Fiducial ID"), "HAS PROPERTIES", ::fwTools::getString(id));
    rootNode->addSubNode(idNode);

    // Create Fiducial UID node
    ::gdcm::UIDGenerator generator;
    SPTR(::gdcmIO::container::sr::DicomSRUIDRefNode) uidNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRUIDRefNode >(
            ::gdcmIO::container::DicomCodedAttribute("dd1202", "DCM",
                                                     "Fiducial UID"), "HAS PROPERTIES", generator.Generate());
    rootNode->addSubNode(uidNode);

    // Create Fiducial intent node
    const std::string label = point->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value();
    SPTR(::gdcmIO::container::sr::DicomSRTextNode) intentNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRTextNode >(
            ::gdcmIO::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"), "HAS PROPERTIES", label);
    rootNode->addSubNode(intentNode);

    if(useSCoord3D)
    {
        // Create SCoord3D Node
        const float scoord[] = { static_cast<float>(point->getCoord()[0]),
                                 static_cast<float>(point->getCoord()[1]),
                                 static_cast<float>(point->getCoord()[2]) };
        std::vector<float> scoordVector (scoord, scoord + 3);
        SPTR(::gdcmIO::container::sr::DicomSRSCoord3DNode) scoord3DNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRSCoord3DNode >(
                ::gdcmIO::container::DicomCodedAttribute(), "HAS PROPERTIES", "POINT", scoordVector,
                m_instance->getSOPInstanceUIDContainer()[0]);
        rootNode->addSubNode(scoord3DNode);
    }
    else
    {
        // Create SCoord Node
        const float scoord[] = { static_cast<float>(point->getCoord()[0]),
                                 static_cast<float>(point->getCoord()[1]) };
        std::vector<float> scoordVector (scoord, scoord + 2);
        SPTR(::gdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRSCoordNode >(
                ::gdcmIO::container::DicomCodedAttribute(), "HAS PROPERTIES", "POINT", scoordVector);
        rootNode->addSubNode(scoordNode);

        // Create Image Node
        int frameNumber = ::gdcmIO::helper::DicomData::convertPointToFrameNumber(m_object, point);
        SPTR(::gdcmIO::container::sr::DicomSRImageNode) imageNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRImageNode >(
                ::gdcmIO::container::DicomCodedAttribute(), "SELECTED FROM", m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frameNumber-1], frameNumber);
        scoordNode->addSubNode(imageNode);
    }

}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace gdcmIO
