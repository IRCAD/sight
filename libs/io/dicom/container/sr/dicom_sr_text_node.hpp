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
 * @brief This class defines a TEXT node in a SR document
 */
class IO_DICOM_CLASS_API dicom_sr_text_node : public io::dicom::container::sr::dicom_sr_node
{
public:

    /// Constructor
    IO_DICOM_API dicom_sr_text_node(
        const dicom_coded_attribute& _coded_attribute,
        const std::string& _relationship,
        std::string _text_value
    );

    /// Destructor
    IO_DICOM_API ~dicom_sr_text_node() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] _dataset Destination dataset
     */
    IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;

    /// Get Text value
    [[nodiscard]] const std::string& get_text_value() const
    {
        return m_text_value;
    }

    ///Set Text value
    void set_text_value(const std::string& _text_value)
    {
        m_text_value = _text_value;
    }

protected:

    /// Dump function
    void print(std::ostream& _os) const override;

    /// Text Value (0040, a160)
    std::string m_text_value;
};

} // namespace sight::io::dicom::container::sr
