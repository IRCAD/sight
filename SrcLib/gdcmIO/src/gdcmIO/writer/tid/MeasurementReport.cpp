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

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRNumNode.hpp"
#include "gdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "gdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "gdcmIO/container/sr/DicomSRTextNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "gdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "gdcmIO/container/sr/DicomSRImageNode.hpp"
#include "gdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "gdcmIO/helper/DicomData.hpp"

#include "gdcmIO/writer/tid/Measurement.hpp"
#include "gdcmIO/writer/tid/Fiducial.hpp"

#include "gdcmIO/writer/tid/MeasurementReport.hpp"

namespace gdcmIO
{
namespace writer
{
namespace tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(SPTR(::gdcm::Writer)writer,
                                     SPTR(::gdcmIO::container::DicomInstance)instance,
                                     ::fwData::Image::sptr image) :
    ::gdcmIO::writer::tid::TemplateID< ::fwData::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
{
}

//------------------------------------------------------------------------------

SPTR(::gdcmIO::container::sr::DicomSRNode) MeasurementReport::createRootNode(bool useSCoord3D)
{
    // Create Root Node - Measurement Report Document Titles
    SPTR(::gdcmIO::container::sr::DicomSRContainerNode) rootNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(
            ::gdcmIO::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report"));

    // Create Language of Content Item and Descendants Node (TID 1204)
    SPTR(::gdcmIO::container::sr::DicomSRCodeNode) languagesNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRCodeNode >(
            ::gdcmIO::container::DicomCodedAttribute("121049", "DCM", "Language of Content Item and Descendants"),
            "HAS CONCEPT MOD",
            ::gdcmIO::container::DicomCodedAttribute("en-US", "RFC3066", "English, United States"));          // FIXME Do we keep English US ?
    rootNode->addSubNode(languagesNode);

    // Create Observation Context Node (TID 1001) FIXME Do we need to add that template ?

    // Create Procedure Reported Node
    SPTR(::gdcmIO::container::sr::DicomSRCodeNode) procedureReportedNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRCodeNode >(
            ::gdcmIO::container::DicomCodedAttribute("121058", "DCM", "Procedure reported"),
            "HAS CONCEPT MOD",
            ::gdcmIO::container::DicomCodedAttribute("P5-09051", "SRT", "Magnetic resonance imaging guidance"));          // FIXME Find a good value
    rootNode->addSubNode(procedureReportedNode);

    // Create ImageLibrary Node
    SPTR(::gdcmIO::container::sr::DicomSRContainerNode) imageLibraryNode =
        std::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(
            ::gdcmIO::container::DicomCodedAttribute("111028", "DCM", "Image Library"), "CONTAINS");
    rootNode->addSubNode(imageLibraryNode);

    // Add landmarks
    if(m_object->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId))
    {
        // Create Fiducial Container
        SPTR(::gdcmIO::container::sr::DicomSRContainerNode) fiducialNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(
                ::gdcmIO::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"), "CONTAINS");
        rootNode->addSubNode(fiducialNode);

        ::gdcmIO::writer::tid::Fiducial fiducialTID(m_writer, m_instance, m_object);
        fiducialTID.createNodes(fiducialNode, useSCoord3D);
    }

    // Add distances
    if(m_object->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId))
    {
        // Create Imaging Measurements Container
        SPTR(::gdcmIO::container::sr::DicomSRContainerNode) imagingMeasurementsNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(
                ::gdcmIO::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"), "CONTAINS");
        rootNode->addSubNode(imagingMeasurementsNode);

        // Create Measurement Group node
        SPTR(::gdcmIO::container::sr::DicomSRContainerNode) measurementGroupNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRContainerNode >(
                ::gdcmIO::container::DicomCodedAttribute("125007", "DCM", "Measurement Group"), "CONTAINS");
        imagingMeasurementsNode->addSubNode(measurementGroupNode);

        // Create Tracking ID node
        SPTR(::gdcmIO::container::sr::DicomSRTextNode) idNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRTextNode >(
                ::gdcmIO::container::DicomCodedAttribute("112039", "DCM", "Tracking Identifier"),
                "HAS OBS CONTEXT", "1");
        imagingMeasurementsNode->addSubNode(idNode);

        // Create Tracking UID node
        ::gdcm::UIDGenerator generator;
        SPTR(::gdcmIO::container::sr::DicomSRUIDRefNode) uidNode =
            std::make_shared< ::gdcmIO::container::sr::DicomSRUIDRefNode >(
                ::gdcmIO::container::DicomCodedAttribute("112040", "DCM", "Tracking Unique Identifier"),
                "HAS OBS CONTEXT", generator.Generate());
        imagingMeasurementsNode->addSubNode(uidNode);

        ::gdcmIO::writer::tid::Measurement measurementTID(m_writer, m_instance, m_object);
        measurementTID.createNodes(imagingMeasurementsNode, useSCoord3D);

    }


    return rootNode;
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace gdcmIO
