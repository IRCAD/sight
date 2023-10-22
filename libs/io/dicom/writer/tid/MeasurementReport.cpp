/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/helper/medical_image.hpp>
#include <data/point_list.hpp>
#include <data/series.hpp>
#include <data/string.hpp>
#include <data/types.hpp>
#include <data/vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const data::image::csptr& _image
) :
    io::dicom::writer::tid::TemplateID<data::image>(_writer, _instance, _image)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
= default;

//------------------------------------------------------------------------------

SPTR(io::dicom::container::sr::DicomSRNode) MeasurementReport::createRootNode(bool _use_s_coord3_d)
{
    // Create Root Node - Measurement Report Document Titles
    SPTR(io::dicom::container::sr::DicomSRContainerNode) root_node =
        std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
            io::dicom::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report")
        );

    // Create Language of Content Item and Descendants Node (TID 1204)
    SPTR(io::dicom::container::sr::DicomSRCodeNode) languages_node =
        std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(
            io::dicom::container::DicomCodedAttribute("121049", "DCM", "Language of Content Item and Descendants"),
            "HAS CONCEPT MOD",
            io::dicom::container::DicomCodedAttribute("en-US", "RFC3066", "English, United States")
        ); // FIXME Do we
           // keep English
           // US ?
    root_node->addSubNode(languages_node);

    // Create Observation Context Node (TID 1001) FIXME Do we need to add that template ?

    // Create Procedure Reported Node
    SPTR(io::dicom::container::sr::DicomSRCodeNode) procedure_reported_node =
        std::make_shared<io::dicom::container::sr::DicomSRCodeNode>(
            io::dicom::container::DicomCodedAttribute("121058", "DCM", "Procedure reported"),
            "HAS CONCEPT MOD",
            io::dicom::container::DicomCodedAttribute("P5-09051", "SRT", "Magnetic resonance imaging guidance")
        ); // FIXME
           // Find
           // a good
           // value
    root_node->addSubNode(procedure_reported_node);

    // Create ImageLibrary Node
    SPTR(io::dicom::container::sr::DicomSRContainerNode) image_library_node =
        std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
            io::dicom::container::DicomCodedAttribute("111028", "DCM", "Image Library"),
            "CONTAINS"
        );
    root_node->addSubNode(image_library_node);

    // Add landmarks
    if(data::helper::medical_image::get_landmarks(*m_object))
    {
        // Create Fiducial Container
        SPTR(io::dicom::container::sr::DicomSRContainerNode) fiducial_node =
            std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
                io::dicom::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"),
                "CONTAINS"
            );
        root_node->addSubNode(fiducial_node);

        io::dicom::writer::tid::Fiducial fiducial_tid(m_writer, m_instance, m_object);
        fiducial_tid.createNodes(fiducial_node, _use_s_coord3_d);
    }

    // Add distances
    if(data::helper::medical_image::get_distances(*m_object))
    {
        // Create Imaging Measurements Container
        SPTR(io::dicom::container::sr::DicomSRContainerNode) imaging_measurements_node =
            std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
                io::dicom::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"),
                "CONTAINS"
            );
        root_node->addSubNode(imaging_measurements_node);

        // Create Measurement Group node
        SPTR(io::dicom::container::sr::DicomSRContainerNode) measurement_group_node =
            std::make_shared<io::dicom::container::sr::DicomSRContainerNode>(
                io::dicom::container::DicomCodedAttribute("125007", "DCM", "Measurement Group"),
                "CONTAINS"
            );
        imaging_measurements_node->addSubNode(measurement_group_node);

        // Create Tracking ID node
        SPTR(io::dicom::container::sr::DicomSRTextNode) id_node =
            std::make_shared<io::dicom::container::sr::DicomSRTextNode>(
                io::dicom::container::DicomCodedAttribute("112039", "DCM", "Tracking Identifier"),
                "HAS OBS CONTEXT",
                "1"
            );
        imaging_measurements_node->addSubNode(id_node);

        // Create Tracking UID node
        gdcm::UIDGenerator generator;
        SPTR(io::dicom::container::sr::DicomSRUIDRefNode) uid_node =
            std::make_shared<io::dicom::container::sr::DicomSRUIDRefNode>(
                io::dicom::container::DicomCodedAttribute("112040", "DCM", "Tracking Unique Identifier"),
                "HAS OBS CONTEXT",
                generator.Generate()
            );
        imaging_measurements_node->addSubNode(uid_node);

        io::dicom::writer::tid::Measurement measurement_tid(m_writer, m_instance, m_object);
        measurement_tid.createNodes(imaging_measurements_node, _use_s_coord3_d);
    }

    return root_node;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
