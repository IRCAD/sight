/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "io/dicom/container/sr/dicom_sr_container_node.hpp"
#include "io/dicom/container/sr/dicom_sr_node.hpp"
#include "io/dicom/helper/dicom_data_reader.hxx"

#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers for DICOM Structured Reporting (SR).
 */
class IO_DICOM_CLASS_API structured_report
{
public:

    /**
     * @brief Read a Structured Report
     * @param[in] dataset Dataset from which the SR must be created
     */
    IO_DICOM_API static SPTR(io::dicom::container::sr::dicom_sr_container_node)
    read_sr(const gdcm::DataSet& _dataset);

    /**
     * @brief Dump the SR in graphviz format
     * @param[in] root Root node
     * @param[in] out Destination stream
     */
    IO_DICOM_API static void dump_sr(
        const SPTR(io::dicom::container::sr::dicom_sr_node)& _root,
        std::ostream& _out = std::cout
    );

protected:

    /**
     * @brief Read all sub nodes and add it as children to the parent node
     * @param[in] dataset Dataset from which the sub nodes must been read
     * @param[in] parent Parent node
     */
    static void read_sub_node_container(const gdcm::DataSet& _dataset,
                                        SPTR(io::dicom::container::sr::dicom_sr_node) _parent);

    /**
     * @brief Read a sub node and add it as a child to the parent node
     * @param[in] dataset Dataset from which the sub node must been read
     * @param[in] parent Parent node
     */
    static void read_sub_node(const gdcm::DataSet& _dataset,
                              SPTR(io::dicom::container::sr::dicom_sr_node) _parent);

    /**
     * @brief Dump an SR node in graphviz format
     * @param[in] node Node
     * @param[in] out Destination stream
     * @param[in] index Node index
     */
    IO_DICOM_API static void dump_sr_node(
        const SPTR(io::dicom::container::sr::dicom_sr_node)& _node,
        std::ostream& _out,
        int& _index
    );

    /**
     * @brief Read content of a code sequence (eg : Concept Name Code Sequence, ...)
     * @note This method can return a DicomCodedAttribute with empty members.
     * @param[in] dataset   Data set where the code sequence is supposed to be.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template<uint16_t GROUP, uint16_t ELEMENT>
    static io::dicom::container::dicom_coded_attribute read_code_sequence(const gdcm::DataSet& _dataset)
    {
        io::dicom::container::dicom_coded_attribute coded_attributes;

        if(!_dataset.FindDataElement(gdcm::Tag(GROUP, ELEMENT)))
        {
            // Return empty coded attributes
            return coded_attributes;
        }

        gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
            _dataset.GetDataElement(gdcm::Tag(GROUP, ELEMENT)).GetValueAsSQ();
        if(sequence->GetNumberOfItems() == 0) // One Item shall be permitted
        {
            // Return empty coded attributes
            return coded_attributes;
        }

        const gdcm::DataSet& item_dataset = sequence->GetItem(1).GetNestedDataSet();

        // Code value - Type 1
        auto code_value = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0100>(item_dataset);

        // Coding Scheme Designator - Type 1
        auto coding_scheme_designator =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0102>(item_dataset);

        // Coding Scheme Version - Type 1C
        auto coding_scheme_version = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0103>(item_dataset);

        // Code Meaning - Type 1
        auto code_meaning = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0104>(item_dataset);

        return {code_value, coding_scheme_designator, code_meaning, coding_scheme_version};
    }
};

} // namespace sight::io::dicom::helper
