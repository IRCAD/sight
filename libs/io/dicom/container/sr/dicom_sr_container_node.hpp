/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/container/dicom_coded_attribute.hpp"
#include "io/dicom/container/sr/dicom_sr_node.hpp"

#include <gdcmDataSet.h>

namespace sight::io::dicom::container::sr
{

/**
 * @brief This class defines a CONTAINER node in a SR document
 */
class SIGHT_IO_DICOM_CLASS_API dicom_sr_container_node : public io::dicom::container::sr::dicom_sr_node
{
public:

    /// Constructor
    SIGHT_IO_DICOM_API dicom_sr_container_node(
        const dicom_coded_attribute& _coded_attribute,
        const std::string& _relationship = ""
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~dicom_sr_container_node() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] _dataset Destination dataset
     */
    SIGHT_IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;
};

} // namespace sight::io::dicom::container::sr
