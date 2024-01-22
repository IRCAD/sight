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

#include "io/dicom/config.hpp"
#include "io/dicom/container/dicom_coded_attribute.hpp"
#include "io/dicom/container/sr/dicom_sr_node.hpp"

#include <gdcmDataSet.h>

namespace sight::io::dicom::container::sr
{

/**
 * @brief This class defines a SCOORD node in a SR document
 */
class IO_DICOM_CLASS_API dicom_srs_coord_node : public io::dicom::container::sr::dicom_sr_node
{
public:

    using graphic_data_container_t = std::vector<float>;

    /// Constructor
    IO_DICOM_API dicom_srs_coord_node(
        const dicom_coded_attribute& _coded_attribute,
        const std::string& _relationship,
        std::string _graphic_type,
        graphic_data_container_t _graphic_data_container
    );

    /// Destructor
    IO_DICOM_API ~dicom_srs_coord_node() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] _dataset Destination dataset
     */
    IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;

    /// Get Graphic type
    [[nodiscard]] const std::string& get_graphic_type() const
    {
        return m_graphic_type;
    }

    ///Set Graphic type
    void set_graphic_type(const std::string& _graphic_type)
    {
        m_graphic_type = _graphic_type;
    }

    /// Get Graphic data container
    [[nodiscard]] const graphic_data_container_t& get_graphic_data_container() const
    {
        return m_graphic_data_container;
    }

    ///Set Graphic data container
    void set_graphic_data_container(const graphic_data_container_t& _graphic_data_container)
    {
        m_graphic_data_container = _graphic_data_container;
    }

protected:

    /// Dump function
    void print(std::ostream& _os) const override;

    /// Graphic Type
    std::string m_graphic_type;

    /// Graphic Data
    graphic_data_container_t m_graphic_data_container;
};

} // namespace sight::io::dicom::container::sr
