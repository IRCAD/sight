/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "data/series.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <functional>
#include <gdcmDataElement.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmSmartPointer.h>
#include <utility>

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4702) // warning C4702: unreachable code
#endif

#include <gdcmDataSet.h>
#include <gdcmExplicitDataElement.h>
#include <gdcmImage.h>
#include <gdcmImageReader.h>
#include <gdcmMediaStorage.h>
#include <gdcmSwapper.h>
#include <gdcmTagKeywords.h>
#include <gdcmTagToVR.h>
#include <gdcmUIDs.h>

#ifdef WIN32
#pragma warning( pop )
#endif

namespace sight::data::detail
{

using SeriesDataset = std::pair<gdcm::DataSet, std::filesystem::path>;
using FrameDatasets = std::vector<SeriesDataset>;

static constexpr char SPACE_PADDING_CHAR  = ' ';
static constexpr char NULL_PADDING_CHAR   = '\0';
static constexpr auto BACKSLASH_SEPARATOR = "\\";

/// @see https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_7.8
static constexpr std::uint16_t PRIVATE_GROUP {0x0099};
static constexpr std::uint16_t PRIVATE_CREATOR_ELEMENT {0x0099};
static constexpr std::uint16_t PRIVATE_DATA_ELEMENT {0x9910};
static const std::string PRIVATE_CREATOR {"Sight"};

/// Allows to check if a VM is fixed (NOT 1..n). Since GDCM Attribute API differs if VM is fixed or not, we need to
/// check to choose the right gdcm::Attributes::SetValues() version.
/// @{

/// Template matching utilities
template<typename ... Ts>
struct IsHelper {};

template<typename T, typename = void>
struct HasFixedMultiplicity : std::true_type {};

template<typename T>
struct HasFixedMultiplicity<
    T,
    std::conditional_t<
        false,
        IsHelper<
            typename T::ArrayType,
            decltype(std::declval<T>().SetValues(nullptr, 0, true))
        >,
        void
    >
>: std::false_type {};
/// @}

/// Remove the trailing padding \0 characters from a string.
/// @param[in] source The string to be trimmed.
/// @return The trimmed string.
static inline std::string shrink(const std::string& _source)
{
    std::string result(_source);
    result.erase(result.find_last_not_of(NULL_PADDING_CHAR) + 1);
    return result;
}

/// Returns the maximum or fixed size of a Value Representation and its padding character.
/// @note the data come from https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
/// @param[in] vr_type The Value Representation to get the size of.
static constexpr std::tuple<std::size_t, bool, char> get_vr_format(gdcm::VR::VRType _vr_type)
{
    switch(_vr_type)
    {
        case gdcm::VR::AE:
            return {16LL, false, NULL_PADDING_CHAR};

        case gdcm::VR::AS:
        case gdcm::VR::AT:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::CS:
            return {16LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::DA:
            return {8LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::DS:
            return {16LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::DT:
            return {26LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::FL:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::FD:
            return {8LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::IS:
            return {12LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::LO:
            return {64LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::LT:
            return {10240LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OB:
            return {(1LL << 32) - 2, false, NULL_PADDING_CHAR};

        case gdcm::VR::OD:
            return {(1LL << 32) - 8, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OF:
        case gdcm::VR::OL:
            return {(1LL << 32) - 4, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OV:
            return {(1LL << 32) - 8, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OW:
            return {(1LL << 32) - 2, false, SPACE_PADDING_CHAR};

        case gdcm::VR::PN:
            return {64 * 5LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SH:
            return {16LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SL:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::SQ:
            return {0LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SS:
            return {2LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::ST:
            return {1024LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SV:
            return {8LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::TM:
            return {14LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::UC:
        case gdcm::VR::UN:
        case gdcm::VR::UR:
        case gdcm::VR::UT:
            return {(1LL << 32) - 2, false, SPACE_PADDING_CHAR};

        case gdcm::VR::UI:
            return {64LL, false, NULL_PADDING_CHAR};

        case gdcm::VR::UL:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::US:
            return {2LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::UV:
            return {8LL, true, SPACE_PADDING_CHAR};

        default:
            return {(1LL << 32) - 2, false, SPACE_PADDING_CHAR};
    }
}

/// Returns a string from a number to be stored in a Integer String or Decimal String. The precision is set
/// coordinately to the VR.
/// @param value The value to be converted.
/// @param vr_type The Value Representation of the value.
/// @return The string representation of the value.
template<typename V>
static inline std::string arithmetic_to_string(const V& _value, gdcm::VR::VRType _vr_type)
{
    const auto [size, fixed, padding] = get_vr_format(_vr_type);

    std::ostringstream oss;

    if(fixed)
    {
        oss << std::setprecision(int(size) - 1) << std::showpoint << std::fixed << _value;
    }
    else
    {
        oss << std::setprecision(int(size) - 1) << std::noshowpoint << std::defaultfloat << _value;
    }

    return oss.str();
}

//------------------------------------------------------------------------------

inline static std::optional<std::string> get_private_string_value(const gdcm::DataSet& _dataset, const gdcm::Tag& _tag)
{
    if(!_dataset.FindDataElement(_tag))
    {
        return std::nullopt;
    }

    const auto& data_element = _dataset.GetDataElement(_tag);

    if(data_element.IsEmpty())
    {
        return std::nullopt;
    }

    const auto* byte_value = data_element.GetByteValue();

    if(byte_value == nullptr || byte_value->GetPointer() == nullptr)
    {
        return std::nullopt;
    }

    return shrink(gdcm::String<>(byte_value->GetPointer(), byte_value->GetLength()).Trim());
}

//------------------------------------------------------------------------------

inline static void set_private_value(
    gdcm::DataSet& _dataset,
    const gdcm::Tag& _tag,
    const std::optional<std::string>& _value
)
{
    if(!_value.has_value())
    {
        _dataset.Remove(_tag);
    }
    else
    {
        // Verify that the creator tag is already there..
        if(const gdcm::Tag creator_tag(detail::PRIVATE_GROUP, detail::PRIVATE_CREATOR_ELEMENT);
           !_dataset.FindDataElement(creator_tag))
        {
            // Add the private creator tag
            gdcm::DataElement creator_data_element(creator_tag, 0, gdcm::VR::LO);
            creator_data_element.SetByteValue(
                detail::PRIVATE_CREATOR.c_str(),
                std::uint32_t(detail::PRIVATE_CREATOR.size())
            );
            _dataset.Replace(creator_data_element);
        }

        // Create the data element
        gdcm::DataElement data_element(_tag, 0, gdcm::VR::UT);

        if(!_value->empty())
        {
            // Get the padding char.
            const auto [size, fixed, padding] = detail::get_vr_format(gdcm::VR::UT);

            const auto& padded =
                [&](char _padding_char)
                {
                    if((_value->size() % 2) != 0)
                    {
                        std::string padded_value(*_value);
                        padded_value.push_back(_padding_char);
                        return padded_value;
                    }

                    return *_value;
                }(padding);

            // Create a new data element and assign the buffer from the string
            data_element.SetByteValue(padded.c_str(), std::uint32_t(padded.size()));
        }

        // Store back the data element to the data set
        _dataset.Replace(data_element);
    }
}

/// Private Series implementation
class SeriesImpl final
{
public:

    /// Delete default constructors and assignment operators
    /// @{
    SeriesImpl(const SeriesImpl&)            = delete;
    SeriesImpl(SeriesImpl&&)                 = delete;
    SeriesImpl& operator=(const SeriesImpl&) = delete;
    SeriesImpl& operator=(SeriesImpl&&)      = delete;
    /// @}

    /// Constructor
    inline explicit SeriesImpl(sight::data::series* const _series) noexcept :
        M_SERIES(_series),
        m_frame_datasets(1)
    {
    }

    /// Default destructor
    inline ~SeriesImpl() noexcept = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] inline const SeriesDataset& getDataPair(std::size_t _instance = 0) const
    {
        return m_frame_datasets[_instance];
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline SeriesDataset& getDataPair(std::size_t _instance = 0)
    {
        if(_instance >= m_frame_datasets.size())
        {
            m_frame_datasets.resize(_instance + 1);
        }

        return m_frame_datasets[_instance];
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline const auto& getDataSet(std::size_t _instance = 0) const
    {
        return getDataPair(_instance).first;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline const auto& get_file(std::size_t _instance = 0) const
    {
        return getDataPair(_instance).second;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline auto& getDataSet(std::size_t _instance = 0)
    {
        return getDataPair(_instance).first;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline auto& get_file(std::size_t _instance = 0)
    {
        return getDataPair(_instance).second;
    }

    /// Retrieve a DICOM tag value. If the tag is not found, an null optional is returned.
    /// @{
    template<typename A>
    auto getValue(std::optional<gdcm::DataElement> _element) const
    {
        using return_t = std::conditional_t<std::is_base_of_v<std::string, typename A::ArrayType>, std::string,
                                            typename A::ArrayType>;
        if(!_element)
        {
            return std::optional<return_t> {};
        }

        gdcm::DataElement data_element = *_element;
        if(!data_element.IsEmpty())
        {
            if(const auto* byte_value = data_element.GetByteValue();
               byte_value != nullptr && byte_value->GetPointer() != nullptr && byte_value->GetLength() > 0)
            {
                // Now, we know that we have a non empty value, so we can safely return it.
                A attribute {};

                attribute.SetFromDataElement(data_element);

                if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
                {
                    // Use trimmed string, as we don't care about DICOM string padding with space
                    return std::optional<return_t> {shrink(attribute.GetValue().Trim())};
                }
                else
                {
                    return std::optional<return_t> {attribute.GetValue()};
                }
            }
        }

        return std::optional<return_t> {};
    }

    //------------------------------------------------------------------------------

    template<typename A>
    auto getValue(
        const gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {}) const
    {
        return getValue<A>(getElement(A::GetTag(), _outer_data_set, _indices));
    }

    template<typename A>
    auto getValue(std::size_t _instance = 0, std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {}) const
    {
        return getValue<A>(getDataSet(_instance), _indices);
    }

    /// @}

    /// Retrieve a string DICOM tag value. If the tag is not found, an empty string is returned.
    template<typename A>
    [[nodiscard]] inline std::string getStringValue(std::size_t _instance = 0) const
    {
        if(_instance >= m_frame_datasets.size())
        {
            return {};
        }

        const auto& value = getValue<A>(_instance);

        if(value)
        {
            if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
            {
                // Use trimmed string, as we don't care about DICOM string padding with space
                return *value;
            }
            else
            {
                // Convert the value to a string
                return toString(*value, A::GetVR());
            }
        }

        // Returns an empty string if the value is not found
        return {};
    }

    /// Retrieve multi-value DICOM tag. If the tag is not found, an empty vector is returned.
    /// @{
    template<typename A>
    auto getValues(std::optional<gdcm::DataElement> _element) const
    {
        using return_t = std::conditional_t<std::is_base_of_v<std::string, typename A::ArrayType>, std::string,
                                            std::vector<typename A::ArrayType> >;
        if(!_element)
        {
            return std::optional<return_t> {};
        }

        gdcm::DataElement data_element = *_element;
        if(!data_element.IsEmpty())
        {
            if(const auto* byte_value = data_element.GetByteValue();
               byte_value != nullptr && byte_value->GetPointer() != nullptr && byte_value->GetLength() > 0)
            {
                // Now, we know that we have a non empty value, so we can safely return it.
                A attribute;

                attribute.SetFromDataElement(data_element);

                if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
                {
                    // Use trimmed string, as we don't care about DICOM string padding with space
                    return std::optional<return_t> {shrink(attribute.GetValue().Trim())};
                }
                else
                {
                    return std::optional<return_t> {std::vector<typename A::ArrayType> {attribute.GetValues(),
                                                                                        attribute.GetValues()
                                                                                        + attribute.GetNumberOfValues()
                                                    }
                    };
                }
            }
        }

        return std::optional<return_t> {};
    }

    //------------------------------------------------------------------------------

    template<typename A>
    auto getValues(
        const gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {}) const
    {
        return getValues<A>(getElement(A::GetTag(), _outer_data_set, _indices));
    }

    template<typename A>
    auto getValues(std::size_t _instance = 0, std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {}) const
    {
        return getValues<A>(getDataSet(_instance), _indices);
    }

    /// @}

    /// Retrieve multi-value DICOM tag as a single joined string. Use '\' as separator.
    template<typename A>
    [[nodiscard]] inline std::string getJoinedValues(std::size_t _instance = 0) const noexcept
    {
        return boost::join(getStringValues<A>(_instance), BACKSLASH_SEPARATOR);
    }

    /// Retrieve a multi-value string DICOM tag. If the tag is not found, an empty vector is returned.
    template<typename A>
    [[nodiscard]] inline std::vector<std::string> getStringValues(std::size_t _instance = 0) const noexcept
    {
        if(_instance >= m_frame_datasets.size())
        {
            return {};
        }

        A attribute;
        attribute.SetFromDataSet(getDataSet(_instance));

        const auto values = attribute.GetValues();

        std::vector<std::string> vector;
        vector.reserve(attribute.GetNumberOfValues());

        for(std::size_t i = 0 ; i < attribute.GetNumberOfValues() ; ++i)
        {
            if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
            {
                // Use trimmed string, as we don't care about DICOM string padding with space
                vector.emplace_back(shrink(values[i].Trim()));
            }
            else
            {
                static_assert(std::is_arithmetic_v<typename A::ArrayType>, "Arithmetic type expected");
                vector.emplace_back(arithmetic_to_string(values[i], A::GetVR()));
            }
        }

        return vector;
    }

    //------------------------------------------------------------------------------

    template<typename A>
    [[nodiscard]] gdcm::SmartPointer<gdcm::SequenceOfItems> getSequence(std::size_t _instance = 0) const noexcept
    {
        gdcm::DataSet data_set = getDataSet(_instance);
        if(!data_set.FindDataElement(A::GetTag()))
        {
            return nullptr;
        }

        return data_set.GetDataElement(A::GetTag()).GetValueAsSQ();
    }

    /// Set a DICOM tag value. If the value is null, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setValue(const std::optional<typename A::ArrayType>& _value, std::size_t _instance = 0)
    {
        if(_value)
        {
            A attribute {};
            attribute.SetValue(*_value);
            getDataSet(_instance).Replace(attribute.GetAsDataElement());
        }
        else
        {
            // We need to put an "empty" value. Since GDCM doesn't allow us to do it with non string values, we need to
            // hack the system. Sorry GDCM, but an Integer String definitively could be "unknown" and an empty string
            // **IS** valid..
            getDataSet(_instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
    }

    /// Set a string DICOM tag value. If the value is null, the tag is replaced by and empty element.
    /// @{
    template<typename A>
    constexpr void setStringValue(const std::string& _value, gdcm::DataSet& _data_set)
    {
        if(_value.empty())
        {
            // Force a real emtpy value..
            _data_set.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute;

            if constexpr(std::is_floating_point_v<typename A::ArrayType>)
            {
                attribute.SetValue(A::ArrayType(std::stod(_value)));
            }
            else if constexpr(std::is_integral_v<typename A::ArrayType>)
            {
                // cspell: ignore stoll
                attribute.SetValue(A::ArrayType(std::stoll(_value)));
            }
            else
            {
                // Maybe it works...
                attribute.SetValue(_value);
            }

            _data_set.Replace(attribute.GetAsDataElement());
        }
    }

    //------------------------------------------------------------------------------

    template<typename A>
    void setStringValue(
        const std::string& _value,
        gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag,
                              std::size_t> > _indices
)
    {
        setStringValue<A>(
            _value,
            getOrCreateDataSet(_outer_data_set, _indices)
        );
    }

    //------------------------------------------------------------------------------

    template<typename A>
    void setStringValue(
        const std::string& _value,
        std::size_t _instance = 0,
        std::vector<std::pair<gdcm::Tag,
                              std::size_t> > _indices = {})
    {
        setStringValue<A>(_value, getDataSet(_instance), _indices);
    }

    /// @}

    /// Set a multi-value DICOM tag. If the vector is empty, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setValues(const std::vector<typename A::ArrayType>& _values, std::size_t _instance = 0)
    {
        if(_values.empty())
        {
            // Force a real emtpy value..
            getDataSet(_instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute;

            if constexpr(HasFixedMultiplicity<A>::value)
            {
                attribute.SetValues(_values.data(), std::uint32_t(_values.size()));
            }
            else
            {
                attribute.SetValues(_values.data(), std::uint32_t(_values.size()), true);
            }

            getDataSet(_instance).Replace(attribute.GetAsDataElement());
        }
    }

    /// Set a multi-value DICOM tag from a single string. Use '\' as separator.
    template<typename A>
    inline void setJoinedValues(const std::string& _values, std::size_t _instance = 0)
    {
        // Split the original string
        std::vector<std::string> split;
        boost::split(split, _values, boost::is_any_of(BACKSLASH_SEPARATOR));

        setStringValues<A>(split, _instance);
    }

    /// Set a string multi-value DICOM tag. If the vector is empty, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setStringValues(const std::vector<std::string>& _values, std::size_t _instance = 0)
    {
        if(!_values.empty())
        {
            std::vector<typename A::ArrayType> array;
            array.reserve(_values.size());

            for(const auto& value : _values)
            {
                /// Yes this could happen, which means empty attribute
                if(!value.empty())
                {
                    if constexpr(std::is_floating_point_v<typename A::ArrayType>)
                    {
                        array.push_back(typename A::ArrayType(std::stod(value)));
                    }
                    else if constexpr(std::is_integral_v<typename A::ArrayType>)
                    {
                        // cspell: ignore stoll
                        array.push_back(typename A::ArrayType(std::stoll(value)));
                    }
                    else
                    {
                        // Maybe it works...
                        array.push_back(value);
                    }
                }
            }

            if(!array.empty())
            {
                A attribute;
                attribute.SetValues(array.data(), std::uint32_t(array.size()), true);
                getDataSet(_instance).Replace(attribute.GetAsDataElement());

                // Exit here...
                return;
            }
        }

        // Default: force a real emtpy value..
        getDataSet(_instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
    }

    /// Little helper function for integral type. Used when tag/VR is not known at compile time.
    template<typename T>
    inline void setArithmeticValue(
        const gdcm::Tag& _tag,
        const gdcm::VR& _vr,
        const T& _value,
        std::size_t _instance = 0
)
    {
        static_assert(std::is_arithmetic_v<T>, "The type must be arithmetic.");

        gdcm::DataElement data_element(_tag);
        data_element.SetVR(_vr);
        data_element.SetByteValue(reinterpret_cast<const char*>(&_value), std::uint32_t(sizeof(_value)));
        getDataSet(_instance).Replace(data_element);
    }

    /// Little helper function for integral type. Used when tag/VR is not known at compile time.
    template<typename T>
    [[nodiscard]] inline std::string getArithmeticValue(const gdcm::Tag& _tag, std::size_t _instance = 0) const
    {
        static_assert(std::is_arithmetic_v<T>, "The type must be arithmetic.");

        if(_instance >= m_frame_datasets.size())
        {
            return {};
        }

        const auto& dataset = getDataSet(_instance);

        if(dataset.FindDataElement(_tag))
        {
            if(const auto& data_element = dataset.GetDataElement(_tag); !data_element.IsEmpty())
            {
                if(const auto* byte_value = data_element.GetByteValue();
                   byte_value != nullptr && byte_value->GetPointer() != nullptr)
                {
                    T value;

                    std::memcpy(&value, byte_value->GetPointer(), sizeof(T));

                    return std::to_string(value);
                }
            }
        }

        return {};
    }

    /// Return the GDCM SequenceOfItems associated to a sequence attribute of a sequence group like
    /// `FrameAcquisitionDateTime`
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @param frameIndex index of the frame
    /// @return GDCM dataset of the attribute
    template<typename G>
    [[nodiscard]] inline gdcm::SmartPointer<gdcm::SequenceOfItems> getMultiFrameGroupSequence(
        std::size_t _frame_index = 0
    ) const
    {
        /// @note We assume that multi-frame dicom have only one instance, IE no instance "Concatenation" here
        /// @note See "Concatenation" related attributes ((0020,9228) and (0020,9162))
        const auto& dataset   = getDataSet(0);
        const auto& group_tag = G::GetTag();

        if(!dataset.FindDataElement(group_tag))
        {
            return {};
        }

        // Retrieve the Functional Groups Sequence
        const auto& frame_sequence = dataset.GetDataElement(group_tag).GetValueAsSQ();

        if(frame_sequence->GetNumberOfItems() <= _frame_index)
        {
            return {};
        }

        return frame_sequence;
    }

    /// Return the GDCM SequenceOfItems associated to a sequence attribute of a sequence group like
    /// `FrameAcquisitionDateTime`
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @tparam S Sequence Attribute (like Frame Content Sequence)
    /// @param frameIndex index of the frame
    /// @return GDCM dataset of the attribute
    template<typename G, typename S>
    [[nodiscard]] inline gdcm::SmartPointer<gdcm::SequenceOfItems> getMultiFrameSequence(
        std::size_t _frame_index = 0
    ) const
    {
        const auto& group_sequence = getMultiFrameGroupSequence<G>(_frame_index);

        if(!group_sequence || group_sequence->IsEmpty())
        {
            return {};
        }

        // Retrieve the frame item and dataset
        const auto& frame_item    = group_sequence->GetItem(_frame_index + 1);
        const auto& frame_dataset = frame_item.GetNestedDataSet();

        const auto& attribute_sequence_tag = S::GetTag();

        if(!frame_dataset.FindDataElement(attribute_sequence_tag))
        {
            return {};
        }

        // Retrieve the attribute sequence
        const auto& attribute_sequence = frame_dataset.GetDataElement(attribute_sequence_tag).GetValueAsSQ();

        if(attribute_sequence->GetNumberOfItems() == 0)
        {
            return {};
        }

        return attribute_sequence;
    }

    /// Retrieve DICOM tag value from a multi-frame sequence attribute of a sequence group like
    /// `FrameAcquisitionDateTime`
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @tparam S Sequence Attribute (like Frame Content Sequence)
    /// @tparam A Attribute (like Frame Acquisition DateTime)
    /// @param frameIndex index of the frame
    /// @return attribute value. If the tag is not found, an empty vector is returned.
    template<typename G, typename S, typename A>
    [[nodiscard]] inline std::optional<typename A::ArrayType> getMultiFrameValue(std::size_t _frame_index = 0) const
    {
        const auto& attribute_sequence = getMultiFrameSequence<G, S>(_frame_index);

        if(!attribute_sequence || attribute_sequence->IsEmpty())
        {
            return std::nullopt;
        }

        // Finally get the value...
        A attribute {};
        attribute.SetFromDataSet(attribute_sequence->GetItem(1).GetNestedDataSet());
        return attribute.GetValue();
    }

    /// Return the GDCM group sequence of a sequence group like `PerFrameFunctionalGroupsSequence`
    /// Construct intermediate DataElements if they don't exist.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @param frameIndex index of the frame
    /// @return GDCM dataset of the attribute
    template<typename G>
    inline gdcm::SmartPointer<gdcm::SequenceOfItems> getMultiFrameGroupSequence(std::size_t _frame_index = 0)
    {
        /// @note We assume that multi-frame dicom have only one instance, IE no instance "Concatenation" here
        /// @note See "Concatenation" related attributes ((0020,9228) and (0020,9162))
        auto& dataset         = getDataSet(0);
        const auto& group_tag = G::GetTag();

        if(!dataset.FindDataElement(group_tag))
        {
            // No Frame Sequence found, create it
            auto group_sequence = new gdcm::SequenceOfItems();
            group_sequence->SetLengthToUndefined();

            gdcm::DataElement group_element(group_tag);
            group_element.SetVR(gdcm::VR::SQ);
            group_element.SetVLToUndefined();
            group_element.SetValue(*group_sequence);

            dataset.Insert(group_element);
        }

        // Retrieve the Frame Sequence
        auto group_sequence = dataset.GetDataElement(group_tag).GetValueAsSQ();

        while(group_sequence->GetNumberOfItems() <= _frame_index)
        {
            // Create all intermediate items, as needed
            gdcm::Item frame_item;
            frame_item.SetVLToUndefined();
            group_sequence->AddItem(frame_item);
        }

        // Adjust the number of frames
        setValue<gdcm::Keywords::NumberOfFrames>(int(group_sequence->GetNumberOfItems()));

        return group_sequence;
    }

    /// Return the GDCM SequenceOfItems associated to a sequence attribute of a sequence group like
    /// `FrameAcquisitionDateTime`
    /// Construct intermediate DataElements if they don't exist.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @tparam S Sequence Attribute (like Frame Content Sequence)
    /// @param frameIndex index of the frame
    /// @return GDCM dataset of the attribute
    template<typename G, typename S>
    inline gdcm::SmartPointer<gdcm::SequenceOfItems> getMultiFrameSequence(std::size_t _frame_index = 0)
    {
        auto group_sequence = getMultiFrameGroupSequence<G>(_frame_index);

        // Retrieve the frame item and dataset
        auto& frame_item    = group_sequence->GetItem(_frame_index + 1);
        auto& frame_dataset = frame_item.GetNestedDataSet();

        const auto& attribute_sequence_tag = S::GetTag();

        if(!frame_dataset.FindDataElement(attribute_sequence_tag))
        {
            // No Attribute Sequence found, create it
            gdcm::SmartPointer<gdcm::SequenceOfItems> attribute_sequence = new gdcm::SequenceOfItems();
            attribute_sequence->SetLengthToUndefined();

            gdcm::DataElement attribute_sequence_element(attribute_sequence_tag);
            attribute_sequence_element.SetVR(gdcm::VR::SQ);
            attribute_sequence_element.SetVLToUndefined();
            attribute_sequence_element.SetValue(*attribute_sequence);

            frame_dataset.Insert(attribute_sequence_element);
        }

        // Retrieve the attribute sequence
        auto attribute_sequence = frame_dataset.GetDataElement(attribute_sequence_tag).GetValueAsSQ();

        if(attribute_sequence->GetNumberOfItems() == 0)
        {
            // Add the missing item
            gdcm::Item attribute_item;
            attribute_item.SetVLToUndefined();
            attribute_sequence->AddItem(attribute_item);
        }

        return attribute_sequence;
    }

    /// Set a DICOM tag value to a multi-frame sequence attribute of a sequence group like `FrameAcquisitionDateTime`
    /// Construct intermediate DataElements if they don't exist.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @tparam S Sequence Attribute (like Frame Content Sequence)
    /// @tparam A Attribute (like Frame Acquisition DateTime)
    /// @param frameIndex index of the frame
    template<typename G, typename S, typename A>
    inline void setMultiFrameValue(const std::optional<typename A::ArrayType>& _value, std::size_t _frame_index = 0)
    {
        auto attribute_sequence = getMultiFrameSequence<G, S>(_frame_index);
        auto& attribute_dataset = attribute_sequence->GetItem(1).GetNestedDataSet();

        if(_value)
        {
            A attribute {};
            attribute.SetFromDataSet(attribute_dataset);
            attribute.SetValue(*_value);
            attribute_dataset.Replace(attribute.GetAsDataElement());
        }
        else
        {
            // We need to put an "empty" value. Since GDCM doesn't allow us to do it with non string values, we need to
            // hack the system. Sorry GDCM, but an Integer String definitively could be "unknown" and an empty string
            // **IS** valid..
            attribute_dataset.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
    }

    /// Retrieve multi-value DICOM tag from a multi-frame sequence attribute of a sequence group.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @tparam S Sequence Attribute (like Frame Content Sequence)
    /// @tparam A Attribute (like Frame Acquisition DateTime)
    /// @param frameIndex index of the frame
    /// @return attribute value. If the tag is not found, an empty vector is returned.
    template<typename G, typename S, typename A>
    [[nodiscard]] inline std::vector<typename A::ArrayType> getMultiFrameValues(std::size_t _frame_index = 0) const
    {
        const auto& attribute_sequence = getMultiFrameSequence<G, S>(_frame_index);

        if(!attribute_sequence || attribute_sequence->IsEmpty())
        {
            return {};
        }

        const auto& attribute_dataset = attribute_sequence->GetItem(1).GetNestedDataSet();

        // Finally get the value...
        A attribute {};
        attribute.SetFromDataSet(attribute_dataset);
        const auto count = attribute.GetNumberOfValues();

        if(count == 0)
        {
            return {};
        }

        // Pointer can be treated as iterator ;)
        const auto values_pointer = attribute.GetValues();
        return std::vector<typename A::ArrayType>(values_pointer, values_pointer + count);
    }

    /// Set a multi-value DICOM tag to a multi-frame sequence attribute of a sequence group.
    /// Construct intermediate DataElements if they don't exist.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @tparam S Sequence Attribute (like Frame Content Sequence)
    /// @tparam A Attribute (like Frame Acquisition DateTime)
    /// @param frameIndex index of the frame
    template<typename G, typename S, typename A>
    inline void setMultiFrameValues(const std::vector<typename A::ArrayType>& _values, std::size_t _frame_index = 0)
    {
        auto attribute_sequence = getMultiFrameSequence<G, S>(_frame_index);
        auto& attribute_dataset = attribute_sequence->GetItem(1).GetNestedDataSet();

        if(_values.empty())
        {
            // Force a real emtpy value..
            attribute_dataset.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            // Finally set the value...
            A attribute {};
            attribute.SetFromDataSet(attribute_dataset);

            if constexpr(HasFixedMultiplicity<A>::value)
            {
                attribute.SetValues(_values.data(), std::uint32_t(_values.size()));
            }
            else
            {
                attribute.SetValues(_values.data(), std::uint32_t(_values.size()), true);
            }

            attribute_dataset.Replace(attribute.GetAsDataElement());
        }
    }

    /// Return a private GDCM sequence group associated to a private sequence attribute of a sequence group
    /// Construct intermediate DataElements if they don't exist.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] frameIndex index of the frame
    /// @return GDCM dataset of the attribute
    template<typename G>
    inline gdcm::SmartPointer<gdcm::SequenceOfItems> getMultiFramePrivateSequence(
        std::uint8_t _element,
        std::size_t _frame_index = 0
)
    {
        SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", _element >= 0x10 && _element <= 0xFF);

        auto group_sequence = getMultiFrameGroupSequence<G>(_frame_index);

        // Retrieve the frame item and dataset
        auto& frame_item    = group_sequence->GetItem(_frame_index + 1);
        auto& frame_dataset = frame_item.GetNestedDataSet();

        // Verify that the creator tag is already there..
        if(const gdcm::Tag creator_tag(PRIVATE_GROUP, PRIVATE_CREATOR_ELEMENT);
           !frame_dataset.FindDataElement(creator_tag))
        {
            // Add the private creator tag
            gdcm::DataElement creator_data_element(creator_tag, 0, gdcm::VR::LO);
            creator_data_element.SetByteValue(PRIVATE_CREATOR.c_str(), std::uint32_t(PRIVATE_CREATOR.size()));
            frame_dataset.Replace(creator_data_element);
        }

        // Get the tag
        const gdcm::Tag attribute_sequence_tag(PRIVATE_GROUP, PRIVATE_DATA_ELEMENT + _element);

        if(!frame_dataset.FindDataElement(attribute_sequence_tag))
        {
            // No Attribute Sequence found, create it
            auto* attribute_sequence = new gdcm::SequenceOfItems();
            attribute_sequence->SetLengthToUndefined();

            gdcm::DataElement attribute_sequence_element(attribute_sequence_tag);
            attribute_sequence_element.SetVR(gdcm::VR::SQ);
            attribute_sequence_element.SetVLToUndefined();
            attribute_sequence_element.SetValue(*attribute_sequence);

            frame_dataset.Insert(attribute_sequence_element);
        }

        // Retrieve the attribute sequence
        auto attribute_sequence = frame_dataset.GetDataElement(attribute_sequence_tag).GetValueAsSQ();

        if(attribute_sequence->GetNumberOfItems() == 0)
        {
            // Add the missing item
            gdcm::Item attribute_item;
            attribute_item.SetVLToUndefined();
            attribute_sequence->AddItem(attribute_item);
        }

        return attribute_sequence;
    }

    /// Return a private GDCM sequence group associated to a private sequence attribute of a sequence group
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @param[in] element private element number in the range of 0x10 to 0xFF
    /// @param[in] frameIndex index of the frame
    /// @return GDCM dataset of the attribute
    template<typename G>
    inline gdcm::SmartPointer<gdcm::SequenceOfItems> getMultiFramePrivateSequence(
        std::uint8_t _element,
        std::size_t _frame_index = 0
    ) const
    {
        SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", _element >= 0x10 && _element <= 0xFF);

        const auto& group_sequence = getMultiFrameGroupSequence<G>(_frame_index);
        if(!group_sequence)
        {
            return {};
        }

        // Retrieve the frame item and dataset
        const auto& frame_item    = group_sequence->GetItem(_frame_index + 1);
        const auto& frame_dataset = frame_item.GetNestedDataSet();

        // Get the tag
        const gdcm::Tag attribute_sequence_tag(PRIVATE_GROUP, PRIVATE_DATA_ELEMENT + _element);

        if(!frame_dataset.FindDataElement(attribute_sequence_tag))
        {
            return {};
        }

        // Retrieve the attribute sequence
        auto attribute_sequence = frame_dataset.GetDataElement(attribute_sequence_tag).GetValueAsSQ();

        if(attribute_sequence->GetNumberOfItems() == 0)
        {
            return {};
        }

        return attribute_sequence;
    }

    /// Retrieve private DICOM tag value from a multi-frame sequence attribute of a sequence group like
    /// `FrameAcquisitionDateTime`
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @param frameIndex index of the frame
    /// @param[in] sequence_element private sequence element number in the range of 0x10 to 0xFF
    /// @param[in] value_element private value element number in the range of 0x10 to 0xFF
    ///                          (must be different from sequence_element)
    /// @return attribute value. If the tag is not found, an empty vector is returned.
    template<typename G>
    [[nodiscard]] inline std::optional<std::string> getMultiFramePrivateValue(
        std::uint8_t _sequence_element,
        std::uint8_t _value_element,
        std::size_t _frame_index = 0
    ) const
    {
        SIGHT_ASSERT(
            "The private element must be between 0x10 and 0xFF and sequence and value element must be different.",
            _sequence_element >= 0x10 && _sequence_element <= 0xFF
            && _value_element >= 0x10 && _value_element <= 0xFF
            && _sequence_element != _value_element
        );

        const auto& attribute_sequence = getMultiFramePrivateSequence<G>(_sequence_element, _frame_index);

        if(!attribute_sequence || attribute_sequence->IsEmpty())
        {
            return std::nullopt;
        }

        // Finally get the value...
        // Get the tag
        gdcm::Tag data_tag(detail::PRIVATE_GROUP, detail::PRIVATE_DATA_ELEMENT + _value_element);

        // Get the dataset
        const auto& nested_data_set = attribute_sequence->GetItem(1).GetNestedDataSet();

        return get_private_string_value(nested_data_set, data_tag);
    }

    /// Set a DICOM private tag value to a multi-frame sequence attribute of a sequence group like
    /// `FrameAcquisitionDateTime`
    /// Construct intermediate DataElements if they don't exist.
    /// @tparam G Functional Groups Sequence Attribute (like Per-frame Functional Groups Sequence)
    /// @param[in] value private string value to set
    /// @param[in] sequence_element private sequence element number in the range of 0x10 to 0xFF
    /// @param[in] value_element private value element number in the range of 0x10 to 0xFF
    ///                          (must be different from sequence_element)
    /// @param[in] frameIndex index of the frame
    template<typename G>
    inline void setMultiFramePrivateValue(
        const std::optional<std::string>& _value,
        std::uint8_t _sequence_element,
        std::uint8_t _value_element,
        std::size_t _frame_index = 0
)
    {
        SIGHT_ASSERT(
            "The private element must be between 0x10 and 0xFF and sequence and value element must be different.",
            _sequence_element >= 0x10 && _sequence_element <= 0xFF
            && _value_element >= 0x10 && _value_element <= 0xFF
            && _sequence_element != _value_element
        );

        auto attribute_sequence = getMultiFramePrivateSequence<G>(_sequence_element, _frame_index);
        auto& attribute_dataset = attribute_sequence->GetItem(1).GetNestedDataSet();

        // Get the tag
        gdcm::Tag attribute_tag(detail::PRIVATE_GROUP, detail::PRIVATE_DATA_ELEMENT + _value_element);

        // Set the value
        detail::set_private_value(attribute_dataset, attribute_tag, _value);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] static std::optional<gdcm::DataElement> getElement(
        gdcm::Tag _final_tag,
        const gdcm::DataSet& _data_set,
        const std::vector<std::pair<gdcm::Tag, std::size_t> >& _indices = {})
    {
        const gdcm::DataSet* current_data_set = &_data_set;
        for(const auto& [tag, index] : _indices)
        {
            if(!current_data_set->FindDataElement(tag))
            {
                return std::nullopt;
            }

            const gdcm::DataElement& data_element              = current_data_set->GetDataElement(tag);
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = data_element.GetValueAsSQ();
            SIGHT_ASSERT("Tried to subscript an item which isn't a sequence", sequence);
            if(sequence->GetNumberOfItems() <= index)
            {
                return std::nullopt;
            }

            current_data_set = &sequence->GetItem(index + 1).GetNestedDataSet(); // GDCM SequenceOfItems are
                                                                                 // 1-indexed...
        }

        if(!current_data_set->FindDataElement(_final_tag))
        {
            return std::nullopt;
        }

        return current_data_set->GetDataElement(_final_tag);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::optional<gdcm::DataElement> getElement(
        gdcm::Tag _final_tag,
        std::size_t _instance,
        const std::vector<std::pair<gdcm::Tag, std::size_t> >& _indices
    ) const
    {
        return getElement(
            _final_tag,
            getDataSet(_instance),
            _indices
        );
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] static std::optional<gdcm::DataSet> getDataSet(
        const gdcm::DataSet& _data_set,
        const std::vector<std::pair<gdcm::Tag,
                                    std::size_t> >& _indices = {})
    {
        const gdcm::DataSet* current_data_set = &_data_set;
        for(const auto& [tag, index] : _indices)
        {
            if(!current_data_set->FindDataElement(tag))
            {
                return std::nullopt;
            }

            const gdcm::DataElement& data_element              = current_data_set->GetDataElement(tag);
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = data_element.GetValueAsSQ();
            SIGHT_ASSERT("Tried to subscript an item which isn't a sequence", sequence);
            if(sequence->GetNumberOfItems() <= index)
            {
                return std::nullopt;
            }

            current_data_set = &sequence->GetItem(index + 1).GetNestedDataSet(); // GDCM SequenceOfItems are
                                                                                 // 1-indexed...
        }

        return *current_data_set;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::optional<gdcm::DataSet> getDataSet(
        std::size_t _instance,
        const std::vector<std::pair<gdcm::Tag,
                                    std::size_t> >& _indices
    ) const
    {
        return getDataSet(getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    static gdcm::DataSet& getOrCreateDataSet(
        gdcm::DataSet& _data_set,
        const std::vector<std::pair<gdcm::Tag, std::size_t> >& _indices = {})
    {
        gdcm::DataSet* current_data_set = &_data_set;
        for(const auto& [tag, index] : _indices)
        {
            if(!current_data_set->FindDataElement(tag))
            {
                auto* sequence = new gdcm::SequenceOfItems();
                sequence->SetLengthToUndefined();

                gdcm::DataElement element(tag);
                element.SetVR(gdcm::VR::SQ);
                element.SetVLToUndefined();
                element.SetValue(*sequence);

                current_data_set->Insert(element);
            }

            const gdcm::DataElement& data_element              = current_data_set->GetDataElement(tag);
            gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = data_element.GetValueAsSQ();
            SIGHT_ASSERT("Tried to subscript an item which isn't a sequence", sequence);
            while(sequence->GetNumberOfItems() <= index)
            {
                sequence->AddItem(gdcm::Item {});
            }

            current_data_set = &sequence->GetItem(index + 1).GetNestedDataSet(); // GDCM SequenceOfItems are
                                                                                 // 1-indexed...
        }

        return *current_data_set;
    }

    //------------------------------------------------------------------------------

    gdcm::DataSet& getOrCreateDataSet(
        std::size_t _instance                                           = 0,
        const std::vector<std::pair<gdcm::Tag, std::size_t> >& _indices = {})
    {
        return getOrCreateDataSet(getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] static gdcm::SmartPointer<gdcm::SequenceOfItems> getSequence(
        gdcm::Tag _final_tag,
        const gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        if(std::optional<gdcm::DataElement> data_element = getElement(_final_tag, _outer_data_set, _indices))
        {
            return *data_element->GetValueAsSQ();
        }

        return nullptr;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] gdcm::SmartPointer<gdcm::SequenceOfItems> getSequence(
        gdcm::Tag _final_tag,
        std::size_t _instance                                    = 0,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        return getSequence(_final_tag, getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    static std::optional<std::string> getPrivateValue(
        std::uint8_t _value_element,
        const gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        std::optional<gdcm::DataSet> data_set = getDataSet(_outer_data_set, _indices);
        if(!data_set)
        {
            return std::nullopt;
        }

        return get_private_string_value(
            *data_set,
            gdcm::Tag(PRIVATE_GROUP, PRIVATE_DATA_ELEMENT + _value_element)
        );
    }

    //------------------------------------------------------------------------------

    std::optional<std::string> getPrivateValue(
        std::uint8_t _value_element,
        std::size_t _instance                                    = 0,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        return getPrivateValue(_value_element, getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    template<typename A>
    constexpr void setValue(const std::optional<typename A::ArrayType>& _value, gdcm::DataSet& _data_set)
    {
        if(_value)
        {
            A attribute {};
            attribute.SetValue(*_value);
            _data_set.Replace(attribute.GetAsDataElement());
        }
        else
        {
            // We need to put an "empty" value. Since GDCM doesn't allow us to do it with non string values, we need to
            // hack the system. Sorry GDCM, but an Integer String definitively could be "unknown" and an empty string
            // **IS** valid..
            _data_set.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
    }

    //------------------------------------------------------------------------------

    template<typename A>
    void setValue(
        const auto& _value,
        gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices
)
    {
        setValue<A>(_value, getOrCreateDataSet(_outer_data_set, _indices));
    }

    //------------------------------------------------------------------------------

    template<typename A>
    void setValue(
        const auto& _value,
        std::size_t _instance                                    = 0,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        setValue<A>(_value, getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    template<typename A>
    constexpr void setValues(const std::vector<typename A::ArrayType>& _values, gdcm::DataSet& _data_set)
    {
        if(_values.empty())
        {
            // Force a real emtpy value..
            _data_set.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute;

            if constexpr(HasFixedMultiplicity<A>::value)
            {
                attribute.SetValues(_values.data(), std::uint32_t(_values.size()));
            }
            else
            {
                attribute.SetValues(_values.data(), std::uint32_t(_values.size()), true);
            }

            _data_set.Replace(attribute.GetAsDataElement());
        }
    }

    //------------------------------------------------------------------------------

    template<typename A>
    void setValues(
        const auto& _values,
        gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices
)
    {
        setValues<A>(_values, getOrCreateDataSet(_outer_data_set, _indices));
    }

    //------------------------------------------------------------------------------

    template<typename A>
    void setValues(
        const auto& _values,
        std::size_t _instance                                    = 0,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        setValues<A>(_values, getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    static void setSequence(gdcm::Tag _tag, gdcm::SmartPointer<gdcm::SequenceOfItems> _value, gdcm::DataSet& _data_set)
    {
        if(_value != nullptr)
        {
            gdcm::DataElement data_element;
            data_element.SetTag(_tag);
            data_element.SetVR(gdcm::VR::SQ);
            data_element.SetVLToUndefined();
            data_element.SetValue(*_value);
            _data_set.Replace(data_element);
        }
        else
        {
            // Force a real empty value.
            _data_set.Replace(gdcm::DataElement(_tag, 0, gdcm::VR::SQ));
        }
    }

    //------------------------------------------------------------------------------

    static void setSequence(
        gdcm::Tag _final_tag,
        gdcm::SmartPointer<gdcm::SequenceOfItems> _value,
        gdcm::DataSet& _outer_data_set,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices
)
    {
        setSequence(_final_tag, _value, getOrCreateDataSet(_outer_data_set, _indices));
    }

    //------------------------------------------------------------------------------

    void setSequence(
        gdcm::Tag _final_tag,
        gdcm::SmartPointer<gdcm::SequenceOfItems> _value,
        std::size_t _instance                                    = 0,
        std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        setSequence(_final_tag, _value, getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    static void setPrivateValue(
        std::uint8_t _value_element,
        const std::optional<std::string>& _value,
        gdcm::DataSet& _outer_data_set,
        const std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        detail::set_private_value(
            getOrCreateDataSet(_outer_data_set, _indices),
            gdcm::Tag(PRIVATE_GROUP, PRIVATE_DATA_ELEMENT + _value_element),
            _value
        );
    }

    //------------------------------------------------------------------------------

    void setPrivateValue(
        std::uint8_t _value_element,
        const std::optional<std::string>& _value,
        std::size_t _instance                                          = 0,
        const std::vector<std::pair<gdcm::Tag, std::size_t> > _indices = {})
    {
        setPrivateValue(_value_element, _value, getDataSet(_instance), _indices);
    }

    //------------------------------------------------------------------------------

    template<typename A>
    inline void copyElement(const gdcm::DataSet& _source, std::size_t _instance = 0)
    {
        copyElement(_source, A::GetTag(), _instance);
    }

    //------------------------------------------------------------------------------

    inline void copyElement(const gdcm::DataSet& _source, const gdcm::Tag& _tag, std::size_t _instance = 0)
    {
        if(_source.FindDataElement(_tag))
        {
            // Make an explicit copy
            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            gdcm::DataElement data_element(_source.GetDataElement(_tag));

            // Replace the element with the copy
            getDataSet(_instance).Replace(data_element);
        }
        else
        {
            // Tag not found, remove it
            getDataSet(_instance).Remove(_tag);
        }
    }

    //------------------------------------------------------------------------------

    inline void copyFrameDatasets(const FrameDatasets& _source)
    {
        m_frame_datasets = _source;

        for(auto& series_dataset : m_frame_datasets)
        {
            // Using streams to perform a deep copy is a required nonsense since GDCM only allows shallow copy
            std::ostringstream os;
            series_dataset.first.Write<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(os);

            std::istringstream is;
            is.str(os.str());

            gdcm::DataSet new_dataset;
            new_dataset.Read<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(is);
            series_dataset.first = new_dataset;
        }
    }

    /// Shrink a multi-frame sequence attribute of a sequence group.
    /// @param size the new number of frames
    inline void shrinkMultiFrame(std::size_t _size)
    {
        if(!m_frame_datasets.empty())
        {
            auto& dataset = m_frame_datasets[0].first;

            // Remove the extra frames
            const auto& group_tag = gdcm::Keywords::PerFrameFunctionalGroupsSequence::GetTag();
            if(dataset.FindDataElement(group_tag))
            {
                // Retrieve the Frame Sequence
                auto group_sequence = dataset.GetDataElement(group_tag).GetValueAsSQ();

                // Resize the sequence
                group_sequence->SetNumberOfItems(_size);

                // Set the number of frames
                setValue<gdcm::Keywords::NumberOfFrames>(std::int32_t(_size));
            }
        }
    }

    /// Pointer to the public class
    sight::data::series* const M_SERIES {nullptr};

    /// Dicom data set instances specific to a frame
    FrameDatasets m_frame_datasets;
};

} // sight::data::detail
