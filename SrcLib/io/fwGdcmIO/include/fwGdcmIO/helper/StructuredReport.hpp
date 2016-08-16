/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_STRUCTUREDREPORT_HPP__
#define __FWGDCMIO_HELPER_STRUCTUREDREPORT_HPP__

#include "fwGdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRNode.hpp"
#include "fwGdcmIO/helper/DicomData.hpp"

#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief This class contains helpers for DICOM Structured Reporting (SR).
 */
class FWGDCMIO_CLASS_API StructuredReport
{
public:

    /**
     * @brief Read a Structured Report
     * @param[in] dataset Dataset from which the SR must be created
     */
    FWGDCMIO_API static SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) readSR(const ::gdcm::DataSet& dataset);

    /**
     * @brief Dump the SR in graphviz format
     * @param[in] root Root node
     * @param[in] out Destination stream
     */
    FWGDCMIO_API static void dumpSR(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& root,
                                    std::ostream& out = std::cout);

protected:

    /**
     * @brief Read all sub nodes and add it as children to the parent node
     * @param[in] dataset Dataset from which the sub nodes must been read
     * @param[in] parent Parent node
     */
    static void readSubNodeContainer(const ::gdcm::DataSet& dataset,
                                     SPTR(::fwGdcmIO::container::sr::DicomSRNode) parent);

    /**
     * @brief Read a sub node and add it as a child to the parent node
     * @param[in] dataset Dataset from which the sub node must been read
     * @param[in] parent Parent node
     */
    static void readSubNode(const ::gdcm::DataSet& dataset, SPTR(::fwGdcmIO::container::sr::DicomSRNode) parent);

    /**
     * @brief Dump an SR node in graphviz format
     * @param[in] node Node
     * @param[in] out Destination stream
     * @param[in] index Node index
     */
    FWGDCMIO_API static void dumpSRNode(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& node, std::ostream& out,
                                        int& index);

    /**
     * @brief Read content of a code sequence (eg : Concept Name Code Sequence, ...)
     * @note This method can return a DicomCodedAttribute with empty members.
     * @param[in] dataset   Data set where the code sequence is supposed to be.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template <uint16_t GROUP, uint16_t ELEMENT>
    static ::fwGdcmIO::container::DicomCodedAttribute readCodeSequence(const ::gdcm::DataSet& dataset)
    {
        ::fwGdcmIO::container::DicomCodedAttribute codedAttributes;

        if ( !dataset.FindDataElement(::gdcm::Tag(GROUP,ELEMENT)))
        {
            // Return empty coded attributes
            return codedAttributes;
        }

        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence =
            dataset.GetDataElement(::gdcm::Tag(GROUP,ELEMENT)).GetValueAsSQ();
        if (sequence->GetNumberOfItems() == 0)  // One Item shall be permitted
        {
            // Return empty coded attributes
            return codedAttributes;
        }

        const ::gdcm::DataSet& itemDataset = sequence->GetItem(1).GetNestedDataSet();

        // Code value - Type 1
        codedAttributes.setCodeValue(DicomData::getTrimmedTagValue<0x0008,0x0100>(itemDataset));

        // Coding Scheme Designator - Type 1
        codedAttributes.setCodingSchemeDesignator(DicomData::getTrimmedTagValue<0x0008,0x0102>(itemDataset));

        // Coding Scheme Version - Type 1C
        codedAttributes.setCodingSchemeVersion(DicomData::getTrimmedTagValue<0x0008,0x0103>(itemDataset));

        // Code Meaning - Type 1
        codedAttributes.setCodeMeaning(DicomData::getTrimmedTagValue<0x0008,0x0104>(itemDataset));

        return codedAttributes;
    }
};

} // namespace helper
} // namespace fwGdcmIO

#endif /*__FWGDCMIO_HELPER_STRUCTUREDREPORT_HPP__*/
