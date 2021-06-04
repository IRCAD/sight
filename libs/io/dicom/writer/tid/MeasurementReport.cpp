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

#include "io/dicom/writer/tid/MeasurementReport.hpp"

#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRCodeNode.hpp"
#include "io/dicom/container/sr/DicomSRContainerNode.hpp"
#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRNumNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/container/sr/DicomSRUIDRefNode.hpp"
#include "io/dicom/writer/tid/Fiducial.hpp"
#include "io/dicom/writer/tid/Measurement.hpp"

#include <data/fieldHelper/Image.hpp>
#include <data/PointList.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>
#include <data/types.hpp>
#include <data/Vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom
{

namespace writer
{

namespace tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(
    const SPTR(::gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const data::Image::csptr& image
) :
    io::dicom::writer::tid::TemplateID<data::Image>(writer, instance, image)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
{
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::sr::DicomSRNode) MeasurementReport::createRootNode(bool useSCoord3D)
{
    // Create Root Node - Measurement Report Document Titles
    SPTR(io::dicom::container::sr::DicomSRContainerNode) rootNode =
        std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
            io::dicom::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report")
        );

    // Create Language of Content Item and Descendants Node (TID 1204)
    SPTR(io::dicom::container::sr::DicomSRCodeNode) languagesNode =
        std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(
            io::dicom::container::DicomCodedAttribute("121049", "DCM", "Language of Content Item and Descendants"),
            "HAS CONCEPT MOD",
            io::dicom::container::DicomCodedAttribute("en-US", "RFC3066", "English, United States")
        ); // FIXME Do we
           // keep English
           // US ?
    rootNode->addSubNode(languagesNode);

    // Create Observation Context Node (TID 1001) FIXME Do we need to add that template ?

    // Create Procedure Reported Node
    SPTR(io::dicom::container::sr::DicomSRCodeNode) procedureReportedNode =
        std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(
            io::dicom::container::DicomCodedAttribute("121058", "DCM", "Procedure reported"),
            "HAS CONCEPT MOD",
            io::dicom::container::DicomCodedAttribute("P5-09051", "SRT", "Magnetic resonance imaging guidance")
        ); // FIXME
           // Find
           // a good
           // value
    rootNode->addSubNode(procedureReportedNode);

    // Create ImageLibrary Node
    SPTR(io::dicom::container::sr::DicomSRContainerNode) imageLibraryNode =
        std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
            io::dicom::container::DicomCodedAttribute("111028", "DCM", "Image Library"),
            "CONTAINS"
        );
    rootNode->addSubNode(imageLibraryNode);

    // Add landmarks
    if(m_object->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId))
    {
        // Create Fiducial Container
        SPTR(io::dicom::container::sr::DicomSRContainerNode) fiducialNode =
            std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
                io::dicom::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"),
                "CONTAINS"
            );
        rootNode->addSubNode(fiducialNode);

        io::dicom::writer::tid::Fiducial fiducialTID(m_writer, m_instance, m_object);
        fiducialTID.createNodes(fiducialNode, useSCoord3D);
    }

    // Add distances
    if(m_object->getField<data::Vector>(data::fieldHelper::Image::m_imageDistancesId))
    {
        // Create Imaging Measurements Container
        SPTR(io::dicom::container::sr::DicomSRContainerNode) imagingMeasurementsNode =
            std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
                io::dicom::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"),
                "CONTAINS"
            );
        rootNode->addSubNode(imagingMeasurementsNode);

        // Create Measurement Group node
        SPTR(io::dicom::container::sr::DicomSRContainerNode) measurementGroupNode =
            std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
                io::dicom::container::DicomCodedAttribute("125007", "DCM", "Measurement Group"),
                "CONTAINS"
            );
        imagingMeasurementsNode->addSubNode(measurementGroupNode);

        // Create Tracking ID node
        SPTR(io::dicom::container::sr::DicomSRTextNode) idNode =
            std::make_shared<io::dicom::container::sr::DicomSRTextNode>(
                io::dicom::container::DicomCodedAttribute("112039", "DCM", "Tracking Identifier"),
                "HAS OBS CONTEXT",
                "1"
            );
        imagingMeasurementsNode->addSubNode(idNode);

        // Create Tracking UID node
        ::gdcm::UIDGenerator generator;
        SPTR(io::dicom::container::sr::DicomSRUIDRefNode) uidNode =
            std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(
                io::dicom::container::DicomCodedAttribute("112040", "DCM", "Tracking Unique Identifier"),
                "HAS OBS CONTEXT",
                generator.Generate()
            );
        imagingMeasurementsNode->addSubNode(uidNode);

        io::dicom::writer::tid::Measurement measurementTID(m_writer, m_instance, m_object);
        measurementTID.createNodes(imagingMeasurementsNode, useSCoord3D);
    }

    return rootNode;
}

//------------------------------------------------------------------------------

} // namespace tid

} // namespace writer

} // namespace sight::io::dicom
