/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmElement.h>
#include <gdcmSequenceOfItems.h>

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers to write information into GDCM datasets.
 */
class IO_DICOM_CLASS_API dicom_data_writer
{
public:

    /**
     * @brief Insert an empty tag in a data set. Useful for empty type 2 tags.
     * @param[in] dataset Data set of tags
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_empty_tag_value(gdcm::DataSet& _dataset)
    {
        gdcm::Attribute<GROUP, ELEMENT> attribute;
        _dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Insert the value of a tag in a data set.
     * @param[in] value Value to set
     * @param[in] dataset Data set of tags
     * @tparam T Type of value
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template<typename T, std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_tag_value(const T& _value, gdcm::DataSet& _dataset)
    {
        gdcm::Attribute<GROUP, ELEMENT> attribute {};
        attribute.SetValue(typename gdcm::Attribute<GROUP, ELEMENT>::ArrayType(_value));
        _dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Insert the string value of a tag in a data set.
     * @param[in] value Value to set
     * @param[in] dataset Data set of tags
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_tag_value(const std::string& _value, gdcm::DataSet& _dataset)
    {
        set_tag_value<std::string, GROUP, ELEMENT>(_value, _dataset);
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
    template<typename T, std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_tag_values(const T* _array, const std::size_t _size, gdcm::DataSet& _dataset)
    {
        gdcm::Attribute<GROUP, ELEMENT> attribute;
        if(_array)
        {
            // Make a copy, in all case, otherwise this could lead to a memory leak...
            attribute.SetValues(_array, static_cast<unsigned int>(_size), true);
        }

        _dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Set and insert a sequence of items with a tag in the specified data set.
     * @param[in] sequence Sequence of items to insert
     * @param[in] dataset Data set
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_sequence_tag_value(
        gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence,
        gdcm::DataSet& _dataset
)
    {
        // Create the sequence of items
        gdcm::DataElement data_element(gdcm::Attribute<GROUP, ELEMENT>::GetTag());
        data_element.SetVR(gdcm::VR::SQ);
        data_element.SetValue(*_sequence);
        data_element.SetVL(_sequence->GetLength());

        // Insert the sequence of items
        _dataset.Insert(data_element);
    }

    /**
     * @brief Create and set a sequence of items with a tag in the specified data set.
     * @param[in] dataset Data set
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static gdcm::SmartPointer<gdcm::SequenceOfItems> create_and_set_sequence_tag_value(gdcm::DataSet& _dataset)
    {
        gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = new gdcm::SequenceOfItems();
        sequence->SetLengthToUndefined();
        set_sequence_tag_value<GROUP, ELEMENT>(sequence, _dataset);
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
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_and_merge_sequence_tag_value(
        gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence,
        gdcm::DataSet& _dataset
)
    {
        // Set or add the SQ
        if(!_dataset.FindDataElement(gdcm::Attribute<GROUP, ELEMENT>::GetTag()))
        {
            set_sequence_tag_value<GROUP, ELEMENT>(_sequence, _dataset);
        }
        else
        {
            // Get old SQ
            const gdcm::DataElement& data_element =
                _dataset.GetDataElement(gdcm::Attribute<GROUP, ELEMENT>::GetTag());
            gdcm::SmartPointer<gdcm::SequenceOfItems> old_sequence = data_element.GetValueAsSQ();

            // Add items of the new SQ to the old SQ
            auto nb_item = _sequence->GetNumberOfItems();
            for(decltype(nb_item) i = 1 ; i <= nb_item ; ++i) // WARN : item start at 1
            {
                old_sequence->AddItem(_sequence->GetItem(i));
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
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static void set_code_sequence_tag_value(
        io::dicom::container::dicom_coded_attribute _attribute,
        gdcm::DataSet& _dataset
)
    {
        auto sequence = createAndSetSequenceTagValue<GROUP, ELEMENT>(_dataset);

        gdcm::Item item;
        item.SetVLToUndefined();
        gdcm::DataSet& item_dataset = item.GetNestedDataSet();

        // Code Value - Type 1C
        set_tag_value<0x0008, 0x0100>(_attribute.get_code_value(), item_dataset);

        // Coding Scheme Designator - Type 1C
        set_tag_value<0x0008, 0x0102>(_attribute.get_coding_scheme_designator(), item_dataset);

        // Coding Scheme Version - Type 1C
        if(!_attribute.get_coding_scheme_version().empty())
        {
            set_tag_value<0x0008, 0x0103>(_attribute.get_coding_scheme_version(), item_dataset);
        }

        // Code Meaning - Type 1
        set_tag_value<0x0008, 0x0104>(_attribute.get_code_meaning(), item_dataset);

        sequence->AddItem(item);
    }
};

} // namespace sight::io::dicom::helper
