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
class IO_DICOM_CLASS_API DicomSRNode
{
public:

    typedef std::vector<SPTR(DicomSRNode)> SubNodeContainer;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API virtual void write(gdcm::DataSet& _dataset) const;

    /**
     * @brief Add a sub node
     * @param[in] node Node that must be added
     */
    IO_DICOM_API void addSubNode(const SPTR(DicomSRNode)& _node);

    /// Dump operator
    IO_DICOM_API friend std::ostream& operator<<(std::ostream& _os, const DicomSRNode& _node)
    {
        _node.print(_os);
        return _os;
    }

    /// Get Sub node container
    [[nodiscard]] const SubNodeContainer& getSubNodeContainer() const
    {
        return m_subNodeContainer;
    }

    /// Set Sub node container
    void setSubNodeContainer(const SubNodeContainer& _sub_node_container)
    {
        m_subNodeContainer = _sub_node_container;
    }

    /// Get Coded attribute
    [[nodiscard]] const DicomCodedAttribute& getCodedAttribute() const
    {
        return m_codedAttribute;
    }

    /// Set Coded attribute
    void setCodedAttribute(const DicomCodedAttribute& _coded_attribute)
    {
        m_codedAttribute = _coded_attribute;
    }

    /// Get Type
    [[nodiscard]] const std::string& getType() const
    {
        return m_type;
    }

    /// Set Type
    void setType(const std::string& _type)
    {
        m_type = _type;
    }

    /// Get Relationship
    [[nodiscard]] const std::string& getRelationship() const
    {
        return m_relationship;
    }

    /// Set Relationship
    void setRelationship(const std::string& _relationship)
    {
        m_relationship = _relationship;
    }

protected:

    /// Constructor
    DicomSRNode(
        DicomCodedAttribute _coded_attribute,
        std::string _type,
        std::string _relationship = ""
    );

    /// Destructor
    virtual ~DicomSRNode();

    /// Dump function
    virtual void print(std::ostream& _os) const;

    /**
     * @brief Create a concept name code sequence
     * @param[in] dataset Destination dataset
     * @param[in] codedAttribute Coded attribute to write
     */
    [[nodiscard]] gdcm::SmartPointer<gdcm::SequenceOfItems> createConceptNameCodeSequence(
        const DicomCodedAttribute& _coded_attribute
    ) const;

    /**
     * @brief Write a content sequence
     * @see PS 3.3  Table 8.8-1
     * @param[in] dataset Destination dataset
     */
    void writeContentSequence(gdcm::DataSet& _dataset) const;

    /// SubNode container
    SubNodeContainer m_subNodeContainer;

    /// Coded entry of the node
    DicomCodedAttribute m_codedAttribute;

    /// Node type (0040, a040)
    std::string m_type;

    /// Node relationship (0040, a010)
    std::string m_relationship;
};

} // namespace sight::io::dicom::container::sr
