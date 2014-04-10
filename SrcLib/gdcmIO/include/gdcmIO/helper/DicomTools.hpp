/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_HELEMENTPER_DICOMTOOLS_HPP__
#define __GDCMIO_HELEMENTPER_DICOMTOOLS_HPP__

#include <boost/algorithm/string/trim.hpp>

// GDCM
#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmImage.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmSurface.h>
#include <gdcmString.h>

// fwTools
#include <fwTools/DynamicType.hpp>
#include <fwTools/Type.hpp>

// fwData
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/container/DicomSCoord.hpp"
#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/helper/DicomData.hpp"

namespace gdcmIO
{

namespace helper
{

/**
 * @class   DicomTools
 * @brief   This class contains helpers to set data into DICOM form.
 */
class GDCMIO_CLASS_API DicomTools
{

public:

    /**
     * @brief Convert a representation mode into a DICOM form (eg : SURFACE, POINTS, WIREFRAME).
     * @see DICOM tag (0x0066,0x000d).
     * @param[in] representationMode Reconstruction's material representation mode (eg : SURFACE, POINT, ...).
     */
    GDCMIO_API static const char* convertToPresentationTypeString(
            ::fwData::Material::REPRESENTATION_MODE representationMode);

    /**
     * @brief Write a basic coded entry (see PS 3.3  Table 8.8-1).
     * @param[in] codedAt Coded attributes to write.
     * @param[in] dataset Data set where the Code Sequence will be write.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template <uint16_t GROUP, uint16_t ELEMENT>
    GDCMIO_TEMPLATE_API static void createCodeSequence(const ::gdcmIO::container::DicomCodedAttribute & codedAttribute,
                                                       ::gdcm::DataSet& dataset)
    {
        createCodeSequence<GROUP,ELEMENT>(
                codedAttribute.m_codeValue,
                codedAttribute.m_codingSchemeDesignator,
                codedAttribute.m_codeMeaning,
                codedAttribute.m_codingSchemeVersion, dataset);
    }

    /**
     * @brief Write a basic coded entry (see PS 3.3  Table 8.8-1).
     * The coding scheme version is undefined.
     * @param[in] codeValue Code Value.
     * @param[in] codingSchemeDesignator Coding Scheme Designator.
     * @param[in] codeMeaning Code Meaning.
     * @param[in] dataset Data set where the Code Sequence will be write.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template <uint16_t GROUP, uint16_t ELEMENT>
    GDCMIO_TEMPLATE_API static void createCodeSequence(const std::string & codeValue,
                                                       const std::string & codingSchemeDesignator,
                                                       const std::string & codeMeaning,
                                                       ::gdcm::DataSet &   dataset)
    {
        createCodeSequence<GROUP,ELEMENT>(codeValue, codingSchemeDesignator, codeMeaning, "", dataset);
    }

    /**
     * @brief Write a basic coded entry (see PS 3.3  Table 8.8-1).
     * @param[in] codeValue Code Value.
     * @param[in] codingSchemeDesignator Coding Scheme Designator.
     * @param[in] codeMeaning Code Meaning.
     * @param[in] codingSchemeVersion Coding Scheme Version;
     * @param[in] dataset Data set where the Code Sequence will be write.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template <uint16_t GROUP, uint16_t ELEMENT>
    GDCMIO_TEMPLATE_API static void createCodeSequence(const std::string & codeValue,
                                                       const std::string & codingSchemeDesignator,
                                                       const std::string & codeMeaning,
                                                       const std::string & codingSchemeVersion,
                                                       ::gdcm::DataSet &   dataset)
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSequence = new ::gdcm::SequenceOfItems();
        codeSequence->SetLengthToUndefined();
        {
            // Create item (shall be one)
            ::gdcm::Item item;
            item.SetVLToUndefined();
            ::gdcm::DataSet &itemDataset = item.GetNestedDataSet();

            //see: PS.3.3 Table 8.8-1
            DicomData::setTagValue<0x0008,0x0100>(codeValue, itemDataset);    // Code Value
            DicomData::setTagValue<0x0008,0x0102>(codingSchemeDesignator, itemDataset);    // Coding Scheme Designator
            if (!codingSchemeVersion.empty())
            {
                DicomData::setTagValue<0x0008,0x0103>(codingSchemeVersion, itemDataset);// Coding Scheme Version
            }
            DicomData::setTagValue<0x0008,0x0104>(codeMeaning, itemDataset);    // Code Meaning

            // Insert in a sequence
            codeSequence->AddItem(item);

            DicomData::insertSQ<GROUP,ELEMENT>(codeSequence, dataset);
        }
    }

    /**
     * @brief Read content of a code sequence (eg : Concept Name Code Sequence, ...)
     * @note This method can return a DicomCodedAttribute with empty members.
     * @param[in] dataset   Data set where the code sequence is supposed to be.
     * @tparam GROUP Tag group of the code sequence.
     * @tparam ELEMENT Element group of the code sequence.
     */
    template <uint16_t GROUP, uint16_t ELEMENT>
    GDCMIO_TEMPLATE_API static ::gdcmIO::container::DicomCodedAttribute readCodeSequence(
            const ::gdcm::DataSet & dataset)
    {
        ::gdcmIO::container::DicomCodedAttribute codedAttributes;

        const ::gdcm::Tag codeSQTag(GROUP,ELEMENT);
        if ( !dataset.FindDataElement(codeSQTag))
        {
            // Return empty coded attributes
            return codedAttributes;
        }
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSQ = dataset.GetDataElement(codeSQTag).GetValueAsSQ();

        if ( codeSQ->GetNumberOfItems() == 0)  // One Item shall be permitted
        {
            // Return empty coded attributes
            return codedAttributes;
        }

        const ::gdcm::DataSet & codeDS = codeSQ->GetItem(1).GetNestedDataSet();
        codedAttributes.m_codeValue  = DicomData::getTagValue<0x0008,0x0100>(codeDS);
        codedAttributes.m_codingSchemeDesignator = DicomData::getTagValue<0x0008,0x0102>(codeDS);
        codedAttributes.m_codingSchemeVersion = DicomData::getTagValue<0x0008,0x0103>(codeDS);
        codedAttributes.m_codeMeaning  = DicomData::getTagValue<0x0008,0x0104>(codeDS);

        return codedAttributes;
    }

};

} // namespace helper

} // namespace gdcmIO

#endif /*__GDCMIO_HELEMENTPER_DICOMTOOLS_HPP__*/
