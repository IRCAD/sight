/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMDATAWRITER_HXX__
#define __FWGDCMIO_HELPER_DICOMDATAWRITER_HXX__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmElement.h>
#include <gdcmSequenceOfItems.h>

namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief This class contains helpers to write information into GDCM datasets.
 */
class FWGDCMIO_CLASS_API DicomDataWriter
{
public:

    /**
     * @brief Insert an empty tag in a data set. Useful for empty type 2 tags.
     * @param[in] dataset Data set of tags
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setEmptyTagValue(::gdcm::DataSet& dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Insert the value of a tag in a data set.
     * @param[in] value Value to set
     * @param[in] dataset Data set of tags
     * @tparam T Type of value
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< typename T, std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setTagValue(const T& value, ::gdcm::DataSet& dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        attribute.SetValue(value);
        dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Insert the string value of a tag in a data set.
     * @param[in] value Value to set
     * @param[in] dataset Data set of tags
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setTagValue(const std::string& value, ::gdcm::DataSet& dataset)
    {
        setTagValue<std::string, GROUP, ELEMENT>(value, dataset);
    }

    /**
     * @brief Insert multiple values of a tag in a data set.
     * @param[in] array An Array.
     * @param[in] size Size of the array.
     * @param[in] dataset Data set of tags.
     * @tparam T Type of pointer.
     * @tparam GROUP Group of the tag.
     * @tparam ELEMENT Element of the tag.
     */
    template< typename T, std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setTagValues(const T* array, const size_t size, ::gdcm::DataSet& dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        if(array)
        {
            attribute.SetValues(array, static_cast<unsigned int>(size));
        }
        dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Set and insert a sequence of items with a tag in the specified data set.
     * @param[in] sequence Sequence of items to insert
     * @param[in] dataset Data set
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setSequenceTagValue(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
                                    ::gdcm::DataSet& dataset)
    {
        // Create the sequence of items
        ::gdcm::DataElement dataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag());
        dataElement.SetVR(::gdcm::VR::SQ);
        dataElement.SetValue(*sequence);
        dataElement.SetVL(sequence->GetLength());

        // Insert the sequence of items
        dataset.Insert(dataElement);
    }

    /**
     * @brief Create and set a sequence of items with a tag in the specified data set.
     * @param[in] dataset Data set
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > createAndSetSequenceTagValue(::gdcm::DataSet& dataset)
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
        sequence->SetLengthToUndefined();
        setSequenceTagValue< GROUP, ELEMENT >(sequence, dataset);
        return sequence;
    }

    /**
     * @brief Insert a sequence of items with a tag in the specified data set.
     * If the tag already exists, items of the old and new sequences are added in a new one.
     * @param[in] sequence Sequence of items to insert
     * @param[in] dataset Data set
     *
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setAndMergeSequenceTagValue(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
                                            ::gdcm::DataSet& dataset)
    {
        // Set or add the SQ
        if (!dataset.FindDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()))
        {
            setSequenceTagValue< GROUP, ELEMENT >(sequence, dataset);
        }
        else
        {
            // Get old SQ
            const ::gdcm::DataElement& dataElement =
                dataset.GetDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag());
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > oldSequence = dataElement.GetValueAsSQ();

            // Add items of the new SQ to the old SQ
            auto nbItem = sequence->GetNumberOfItems();
            for (decltype(nbItem)i = 1; i <= nbItem; ++i)  // WARN : item start at 1
            {
                oldSequence->AddItem(sequence->GetItem(i));
            }
        }
    }

    /**
     * @brief Set tag value for the code sequence.
     * @param[in] attribute attribute to set
     * @param[in] dataset Data set
     *
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static void setCodeSequenceTagValue(::fwGdcmIO::container::DicomCodedAttribute attribute,
                                        ::gdcm::DataSet& dataset)
    {
        auto sequence = createAndSetSequenceTagValue< GROUP, ELEMENT >(dataset);

        ::gdcm::Item item;
        item.SetVLToUndefined();
        ::gdcm::DataSet& itemDataset = item.GetNestedDataSet();

        // Code Value - Type 1C
        setTagValue< 0x0008, 0x0100 >(attribute.getCodeValue(), itemDataset);

        // Coding Scheme Designator - Type 1C
        setTagValue< 0x0008, 0x0102 >(attribute.getCodingSchemeDesignator(), itemDataset);

        // Coding Scheme Version - Type 1C
        if(!attribute.getCodingSchemeVersion().empty())
        {
            setTagValue< 0x0008, 0x0103 >(attribute.getCodingSchemeVersion(), itemDataset);
        }

        // Code Meaning - Type 1
        setTagValue< 0x0008, 0x0104>(attribute.getCodeMeaning(), itemDataset);

        sequence->AddItem(item);
    }

};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_DICOMDATAWRITER_HXX__

