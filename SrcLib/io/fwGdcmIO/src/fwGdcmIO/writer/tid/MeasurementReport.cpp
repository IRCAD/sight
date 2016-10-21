/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/writer/tid/MeasurementReport.hpp"

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRNumNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/writer/tid/Fiducial.hpp"
#include "fwGdcmIO/writer/tid/Measurement.hpp"

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <gdcmUIDGenerator.h>

#include <boost/algorithm/string/split.hpp>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(SPTR(::gdcm::Writer)writer,
                                     SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                     ::fwData::Image::sptr image) :
    ::fwGdcmIO::writer::tid::TemplateID< ::fwData::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
{
}

//------------------------------------------------------------------------------

SPTR(::fwGdcmIO::container::sr::DicomSRNode) MeasurementReport::createRootNode(bool useSCoord3D)
{
    // Create Root Node - Measurement Report Document Titles
    SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) rootNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report"));

    // Create Language of Content Item and Descendants Node (TID 1204)
    SPTR(::fwGdcmIO::container::sr::DicomSRCodeNode) languagesNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRCodeNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("121049", "DCM", "Language of Content Item and Descendants"),
            "HAS CONCEPT MOD",
            ::fwGdcmIO::container::DicomCodedAttribute("en-US", "RFC3066", "English, United States"));          // FIXME Do we keep English US ?
    rootNode->addSubNode(languagesNode);

    // Create Observation Context Node (TID 1001) FIXME Do we need to add that template ?

    // Create Procedure Reported Node
    SPTR(::fwGdcmIO::container::sr::DicomSRCodeNode) procedureReportedNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRCodeNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("121058", "DCM", "Procedure reported"),
            "HAS CONCEPT MOD",
            ::fwGdcmIO::container::DicomCodedAttribute("P5-09051", "SRT", "Magnetic resonance imaging guidance"));          // FIXME Find a good value
    rootNode->addSubNode(procedureReportedNode);

    // Create ImageLibrary Node
    SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) imageLibraryNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("111028", "DCM", "Image Library"), "CONTAINS");
    rootNode->addSubNode(imageLibraryNode);

    // Add landmarks
    if(m_object->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId))
    {
        // Create Fiducial Container
        SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) fiducialNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"), "CONTAINS");
        rootNode->addSubNode(fiducialNode);

        ::fwGdcmIO::writer::tid::Fiducial fiducialTID(m_writer, m_instance, m_object);
        fiducialTID.createNodes(fiducialNode, useSCoord3D);
    }

    // Add distances
    if(m_object->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId))
    {
        // Create Imaging Measurements Container
        SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) imagingMeasurementsNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"), "CONTAINS");
        rootNode->addSubNode(imagingMeasurementsNode);

        // Create Measurement Group node
        SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) measurementGroupNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRContainerNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("125007", "DCM", "Measurement Group"), "CONTAINS");
        imagingMeasurementsNode->addSubNode(measurementGroupNode);

        // Create Tracking ID node
        SPTR(::fwGdcmIO::container::sr::DicomSRTextNode) idNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRTextNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("112039", "DCM", "Tracking Identifier"),
                "HAS OBS CONTEXT", "1");
        imagingMeasurementsNode->addSubNode(idNode);

        // Create Tracking UID node
        ::gdcm::UIDGenerator generator;
        SPTR(::fwGdcmIO::container::sr::DicomSRUIDRefNode) uidNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRUIDRefNode >(
                ::fwGdcmIO::container::DicomCodedAttribute("112040", "DCM", "Tracking Unique Identifier"),
                "HAS OBS CONTEXT", generator.Generate());
        imagingMeasurementsNode->addSubNode(uidNode);

        ::fwGdcmIO::writer::tid::Measurement measurementTID(m_writer, m_instance, m_object);
        measurementTID.createNodes(imagingMeasurementsNode, useSCoord3D);

    }


    return rootNode;
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace fwGdcmIO
