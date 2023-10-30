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

measurement_report::measurement_report(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::image::csptr& _image
) :
    io::dicom::writer::tid::template_id<data::image>(_writer, _instance, _image)
{
}

//------------------------------------------------------------------------------

measurement_report::~measurement_report()
= default;

//------------------------------------------------------------------------------

SPTR(io::dicom::container::sr::dicom_sr_node) measurement_report::create_root_node(bool _use_s_coord_3d)
{
    // Create Root Node - Measurement Report Document Titles
    SPTR(io::dicom::container::sr::dicom_sr_container_node) root_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_container_node>(
            io::dicom::container::dicom_coded_attribute("dd1dd1", "DCM", "Imaging Measurement Report")
        );

    // Create Language of Content Item and Descendants Node (TID 1204)
    SPTR(io::dicom::container::sr::dicom_sr_code_node) languages_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_code_node>(
            io::dicom::container::dicom_coded_attribute("121049", "DCM", "Language of Content Item and Descendants"),
            "HAS CONCEPT MOD",
            io::dicom::container::dicom_coded_attribute("en-US", "RFC3066", "English, United States")
        ); // FIXME Do we
           // keep English
           // US ?
    root_node->add_sub_node(languages_node);

    // Create Observation Context Node (TID 1001) FIXME Do we need to add that template ?

    // Create Procedure Reported Node
    SPTR(io::dicom::container::sr::dicom_sr_code_node) procedure_reported_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_code_node>(
            io::dicom::container::dicom_coded_attribute("121058", "DCM", "Procedure reported"),
            "HAS CONCEPT MOD",
            io::dicom::container::dicom_coded_attribute("P5-09051", "SRT", "Magnetic resonance imaging guidance")
        ); // FIXME
           // Find
           // a good
           // value
    root_node->add_sub_node(procedure_reported_node);

    // Create ImageLibrary Node
    SPTR(io::dicom::container::sr::dicom_sr_container_node) image_library_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_container_node>(
            io::dicom::container::dicom_coded_attribute("111028", "DCM", "Image Library"),
            "CONTAINS"
        );
    root_node->add_sub_node(image_library_node);

    // Add landmarks
    if(data::helper::medical_image::get_landmarks(*m_object))
    {
        // Create Fiducial Container
        SPTR(io::dicom::container::sr::dicom_sr_container_node) fiducial_node =
            std::make_shared<io::dicom::container::sr::dicom_sr_container_node>(
                io::dicom::container::dicom_coded_attribute("dd1d93", "DCM", "Fiducials"),
                "CONTAINS"
            );
        root_node->add_sub_node(fiducial_node);

        io::dicom::writer::tid::fiducial fiducial_tid(m_writer, m_instance, m_object);
        fiducial_tid.create_nodes(fiducial_node, _use_s_coord_3d);
    }

    // Add distances
    if(data::helper::medical_image::get_distances(*m_object))
    {
        // Create Imaging Measurements Container
        SPTR(io::dicom::container::sr::dicom_sr_container_node) imaging_measurements_node =
            std::make_shared<io::dicom::container::sr::dicom_sr_container_node>(
                io::dicom::container::dicom_coded_attribute("dd1d91", "DCM", "Imaging Measurements"),
                "CONTAINS"
            );
        root_node->add_sub_node(imaging_measurements_node);

        // Create Measurement Group node
        SPTR(io::dicom::container::sr::dicom_sr_container_node) measurement_group_node =
            std::make_shared<io::dicom::container::sr::dicom_sr_container_node>(
                io::dicom::container::dicom_coded_attribute("125007", "DCM", "Measurement Group"),
                "CONTAINS"
            );
        imaging_measurements_node->add_sub_node(measurement_group_node);

        // Create Tracking ID node
        SPTR(io::dicom::container::sr::dicom_sr_text_node) id_node =
            std::make_shared<io::dicom::container::sr::dicom_sr_text_node>(
                io::dicom::container::dicom_coded_attribute("112039", "DCM", "Tracking Identifier"),
                "HAS OBS CONTEXT",
                "1"
            );
        imaging_measurements_node->add_sub_node(id_node);

        // Create Tracking UID node
        gdcm::UIDGenerator generator;
        SPTR(io::dicom::container::sr::dicom_sruid_ref_node) uid_node =
            std::make_shared<io::dicom::container::sr::dicom_sruid_ref_node>(
                io::dicom::container::dicom_coded_attribute("112040", "DCM", "Tracking Unique Identifier"),
                "HAS OBS CONTEXT",
                generator.Generate()
            );
        imaging_measurements_node->add_sub_node(uid_node);

        io::dicom::writer::tid::measurement measurement_tid(m_writer, m_instance, m_object);
        measurement_tid.create_nodes(imaging_measurements_node, _use_s_coord_3d);
    }

    return root_node;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
