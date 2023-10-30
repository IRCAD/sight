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

#include <core/macros.hpp>

#include <gdcmDataSet.h>

namespace sight::io::dicom::container::sr
{

/**
 * @brief This class defines a node in a SR document
 */
class IO_DICOM_CLASS_API dicom_sr_node
{
public:

    using sub_node_container_t = std::vector<std::shared_ptr<dicom_sr_node> >;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API virtual void write(gdcm::DataSet& _dataset) const;

    /**
     * @brief Add a sub node
     * @param[in] node Node that must be added
     */
    IO_DICOM_API void add_sub_node(const SPTR(dicom_sr_node)& _node);

    /// Dump operator
    IO_DICOM_API friend std::ostream& operator<<(std::ostream& _os, const dicom_sr_node& _node)
    {
        _node.print(_os);
        return _os;
    }

    /// Get Sub node container
    [[nodiscard]] const sub_node_container_t& get_sub_node_container() const
    {
        return m_sub_node_container;
    }

    /// Set Sub node container
    void set_sub_node_container(const sub_node_container_t& _sub_node_container)
    {
        m_sub_node_container = _sub_node_container;
    }

    /// Get Coded attribute
    [[nodiscard]] const dicom_coded_attribute& get_coded_attribute() const
    {
        return m_coded_attribute;
    }

    /// Set Coded attribute
    void set_coded_attribute(const dicom_coded_attribute& _coded_attribute)
    {
        m_coded_attribute = _coded_attribute;
    }

    /// Get Type
    [[nodiscard]] const std::string& type() const
    {
        return m_type;
    }

    /// Set Type
    void set_type(const std::string& _type)
    {
        m_type = _type;
    }

    /// Get Relationship
    [[nodiscard]] const std::string& get_relationship() const
    {
        return m_relationship;
    }

    /// Set Relationship
    void set_relationship(const std::string& _relationship)
    {
        m_relationship = _relationship;
    }

protected:

    /// Constructor
    dicom_sr_node(
        dicom_coded_attribute _coded_attribute,
        std::string _type,
        std::string _relationship = ""
    );

    /// Destructor
    virtual ~dicom_sr_node();

    /// Dump function
    virtual void print(std::ostream& _os) const;

    /**
     * @brief Create a concept name code sequence
     * @param[in] dataset Destination dataset
     * @param[in] codedAttribute Coded attribute to write
     */
    [[nodiscard]] gdcm::SmartPointer<gdcm::SequenceOfItems> create_concept_name_code_sequence(
        const dicom_coded_attribute& _coded_attribute
    ) const;

    /**
     * @brief Write a content sequence
     * @see PS 3.3  Table 8.8-1
     * @param[in] dataset Destination dataset
     */
    void write_content_sequence(gdcm::DataSet& _dataset) const;

    /// SubNode container
    sub_node_container_t m_sub_node_container;

    /// Coded entry of the node
    dicom_coded_attribute m_coded_attribute;

    /// Node type (0040, a040)
    std::string m_type;

    /// Node relationship (0040, a010)
    std::string m_relationship;
};

} // namespace sight::io::dicom::container::sr
