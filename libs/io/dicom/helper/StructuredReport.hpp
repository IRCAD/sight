/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/dicom/container/sr/DicomSRContainerNode.hpp"
#include "io/dicom/container/sr/DicomSRNode.hpp"
#include "io/dicom/helper/DicomDataReader.hxx"

#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers for DICOM Structured Reporting (SR).
 */
class IO_DICOM_CLASS_API StructuredReport
{
public:

    /**
     * @brief Read a Structured Report
     * @param[in] dataset Dataset from which the SR must be created
     */
    IO_DICOM_API static SPTR(io::dicom::container::sr::DicomSRContainerNode)
    readSR(const gdcm::DataSet& dataset);

    /**
     * @brief Dump the SR in graphviz format
     * @param[in] root Root node
     * @param[in] out Destination stream
     */
    IO_DICOM_API static void dumpSR(
        const SPTR(io::dicom::container::sr::DicomSRNode)& root,
        std::ostream& out = std::cout
    );

protected:

    /**
     * @brief Read all sub nodes and add it as children to the parent node
     * @param[in] dataset Dataset from which the sub nodes must been read
     * @param[in] parent Parent node
     */
    static void readSubNodeContainer(const gdcm::DataSet& dataset,
                                     SPTR(io::dicom::container::sr::DicomSRNode) parent);

    /**
     * @brief Read a sub node and add it as a child to the parent node
     * @param[in] dataset Dataset from which the sub node must been read
     * @param[in] parent Parent node
     */
    static void readSubNode(const gdcm::DataSet& dataset,
                            SPTR(io::dicom::container::sr::DicomSRNode) parent);

    /**
     * @brief Dump an SR node in graphviz format
     * @param[in] node Node
     * @param[in] out Destination stream
     * @param[in] index Node index
     */
    IO_DICOM_API static void dumpSRNode(
        const SPTR(io::dicom::container::sr::DicomSRNode)& node,
        std::ostream& out,
        int& index
    );

    /**
     * @brief Read content of a code sequence (eg : Concept Name Code Sequence, ...)
     * @note This method can return a DicomCodedAttribute with empty members.
     * @param[in] dataset   Data set where the code sequence is supposed to be.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template<uint16_t GROUP, uint16_t ELEMENT>
    static io::dicom::container::DicomCodedAttribute readCodeSequence(const gdcm::DataSet& dataset)
    {
        io::dicom::container::DicomCodedAttribute codedAttributes;

        if(!dataset.FindDataElement(gdcm::Tag(GROUP, ELEMENT)))
        {
            // Return empty coded attributes
            return codedAttributes;
        }

        gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
            dataset.GetDataElement(gdcm::Tag(GROUP, ELEMENT)).GetValueAsSQ();
        if(sequence->GetNumberOfItems() == 0) // One Item shall be permitted
        {
            // Return empty coded attributes
            return codedAttributes;
        }

        const gdcm::DataSet& itemDataset = sequence->GetItem(1).GetNestedDataSet();

        // Code value - Type 1
        auto codeValue = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0100>(itemDataset);

        // Coding Scheme Designator - Type 1
        auto codingSchemeDesignator = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0102>(itemDataset);

        // Coding Scheme Version - Type 1C
        auto codingSchemeVersion = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0103>(itemDataset);

        // Code Meaning - Type 1
        auto codeMeaning = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0104>(itemDataset);

        return {codeValue, codingSchemeDesignator, codeMeaning, codingSchemeVersion};
    }
};

} // namespace sight::io::dicom::helper
