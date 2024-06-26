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
#include "io/dicom/reader/tid/template_id.hpp"

#include <data/dicom_series.hpp>
#include <data/image.hpp>

namespace sight::data
{

class point;

} // namespace sight::data

namespace sight::io::dicom::reader::tid
{

/**
 * @brief Measurement TID
 * @see TID 300
 */
class SIGHT_IO_DICOM_CLASS_API measurement : public io::dicom::reader::tid::template_id<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries used to access computed tag values.
     * @param[in] _reader GDCM reader that must be used to read series.
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _image Sight data object
     * @param[in] _logger Logger
     */
    SIGHT_IO_DICOM_API measurement(
        const CSPTR(data::dicom_series)& _dicom_series,
        const SPTR(gdcm::Reader)& _reader,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::sptr& _image,
        const core::log::logger::sptr& _logger
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~measurement() override;

    /**
     * @brief Read a measurement node
     * @param[in] _node Node that must be read
     */
    SIGHT_IO_DICOM_API virtual void read_node(const SPTR(io::dicom::container::sr::dicom_sr_node)& _node);

protected:

    /**
     * @brief Add a distance into the image
     * @param _point1 First point
     * @param _point2 Second point
     */
    void add_distance(
        const SPTR(data::point)& _point1,
        const SPTR(data::point)& _point2
    );
};

} // namespace sight::io::dicom::reader::tid
