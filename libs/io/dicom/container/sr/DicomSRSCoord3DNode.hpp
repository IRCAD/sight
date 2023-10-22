/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

namespace sight::io::dicom::container::sr
{

/**
 * @brief This class defines a SCOORD3D node in a SR document
 */
class IO_DICOM_CLASS_API DicomSRSCoord3DNode : public io::dicom::container::sr::DicomSRNode
{
public:

    typedef std::vector<float> graphic_data_container_t;

    /// Constructor
    IO_DICOM_API DicomSRSCoord3DNode(
        const DicomCodedAttribute& _coded_attribute,
        const std::string& _relationship,
        std::string _graphic_type,
        graphic_data_container_t _graphic_data_container,
        std::string _frame_of_reference_uid
    );

    /// Destructor
    IO_DICOM_API ~DicomSRSCoord3DNode() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;

    /// Get Frame of reference UID
    [[nodiscard]] const std::string& getFrameOfReferenceUID() const
    {
        return m_frameOfReferenceUID;
    }

    ///Set Frame of reference UID
    void setFrameOfReferenceUID(const std::string& _frame_of_reference_uid)
    {
        m_frameOfReferenceUID = _frame_of_reference_uid;
    }

    /// Get Graphic type
    [[nodiscard]] const std::string& get_graphic_type() const
    {
        return m_graphicType;
    }

    ///Set Graphic type
    void set_graphic_type(const std::string& _graphic_type)
    {
        m_graphicType = _graphic_type;
    }

    /// Get Graphic data container
    [[nodiscard]] const graphic_data_container_t& getGraphicDataContainer() const
    {
        return m_graphicDataContainer;
    }

    ///Set Graphic data container
    void setGraphicDataContainer(const graphic_data_container_t& _graphic_data_container)
    {
        m_graphicDataContainer = _graphic_data_container;
    }

protected:

    /// Dump function
    void print(std::ostream& _os) const override;

    /// Referenced Frame of Reference UID
    std::string m_frameOfReferenceUID;

    /// Graphic Type
    std::string m_graphicType;

    /// Graphic Data
    graphic_data_container_t m_graphicDataContainer;
};

} // namespace sight::io::dicom::container::sr
