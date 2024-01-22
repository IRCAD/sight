/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include "io/dicom/container/sr/dicom_sr_node.hpp"
#include "io/dicom/writer/tid/template_id.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::writer::tid
{

/**
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class IO_DICOM_CLASS_API measurement_report : public io::dicom::writer::tid::template_id<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _image Image data
     */
    IO_DICOM_API measurement_report(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::csptr& _image
    );

    /// Destructor
    IO_DICOM_API ~measurement_report() override;

    /**
     * @brief Create Measurement Report template root node
     * @param[in] _use_s_coord_3d True if we must write SCOORD3D, false if we must write SCOORD
     */
    IO_DICOM_API virtual SPTR(io::dicom::container::sr::dicom_sr_node) create_root_node(bool _use_s_coord_3d = true);
};

} // namespace sight::io::dicom::writer::tid
