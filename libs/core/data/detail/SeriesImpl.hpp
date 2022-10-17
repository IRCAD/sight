/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "data/Series.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <gdcmDataSet.h>
#include <gdcmImage.h>
#include <gdcmImageReader.h>
#include <gdcmMediaStorage.h>
#include <gdcmTagKeywords.h>
#include <gdcmTagToVR.h>
#include <gdcmUIDs.h>

namespace sight::data::detail
{

using SeriesDataset = std::pair<gdcm::DataSet, std::filesystem::path>;
using FrameDatasets = std::vector<SeriesDataset>;

static constexpr char SPACE_PADDING_CHAR  = ' ';
static constexpr char NULL_PADDING_CHAR   = '\0';
static constexpr auto BACKSLASH_SEPARATOR = "\\";

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
static inline std::string shrink(const std::string& source)
{
    std::string result(source);
    result.erase(result.find_last_not_of(NULL_PADDING_CHAR) + 1);
    return result;
}

/// Returns the maximum or fixed size of a Value Representation and its padding character.
/// @note the data come from https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
/// @param[in] vr_type The Value Representation to get the size of.
static constexpr std::tuple<std::size_t, bool, char> getVRFormat(gdcm::VR::VRType vr_type)
{
    switch(vr_type)
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
static inline std::string arithmeticToString(const V& value, gdcm::VR::VRType vr_type)
{
    const auto [size, fixed, padding] = getVRFormat(vr_type);

    std::ostringstream oss;

    if(fixed)
    {
        oss << std::setprecision(int(size) - 1) << std::showpoint << std::fixed << value;
    }
    else
    {
        oss << std::setprecision(int(size) - 1) << std::noshowpoint << std::defaultfloat << value;
    }

    return oss.str();
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
    inline explicit SeriesImpl(sight::data::Series* const series) noexcept :
        m_series(series),
        m_frame_datasets(1)
    {
    }

    /// Default destructor
    inline ~SeriesImpl() noexcept = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] inline const auto& getDataPair(std::size_t instance = 0) const
    {
        return m_frame_datasets[instance];
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline auto& getDataPair(std::size_t instance = 0)
    {
        if(instance >= m_frame_datasets.size())
        {
            m_frame_datasets.resize(instance + 1);
        }

        return m_frame_datasets[instance];
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline const auto& getDataSet(std::size_t instance = 0) const
    {
        return getDataPair(instance).first;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline const auto& getFile(std::size_t instance = 0) const
    {
        return getDataPair(instance).second;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline auto& getDataSet(std::size_t instance = 0)
    {
        return getDataPair(instance).first;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline auto& getFile(std::size_t instance = 0)
    {
        return getDataPair(instance).second;
    }

    /// Retrieve a DICOM tag value. If the tag is not found, an null optional is returned.
    template<typename A>
    [[nodiscard]] constexpr std::optional<typename A::ArrayType> getValue(std::size_t instance = 0) const
    {
        const auto& dataset = getDataSet(instance);

        // Unfortunately with GDCM, all non pure string attributes (Integer String, Decimal String, ...) will always
        // be found with a random value, despite the underlying string is "". For example, if the attribute
        // "SeriesNumber" is a "" string, gdcm::Attribute<NULL_CHAR20, NULL_CHAR11>::GetNumberOfValues() will return 1
        // (as 1 is anyway the minimum multiplicity), but with a random integer/decimal value, and there is no way to
        // know if the value is "" (which is valid, it means "unknown") or not.
        //
        // Therefore, we need to access to the underlying data to know if the attribute is empty or not, which is not
        // very efficient or elegant.
        if(dataset.FindDataElement(A::GetTag()))
        {
            if(const auto& data_element = dataset.GetDataElement(A::GetTag()); !data_element.IsEmpty())
            {
                if(const auto byte_value = data_element.GetByteValue();
                   byte_value != nullptr && byte_value->GetPointer() != nullptr && byte_value->GetLength() > 0)
                {
                    // Now, we know that we have a non empty value, so we can safely return it.
                    A attribute {};

                    attribute.SetFromDataSet(dataset);
                    return attribute.GetValue();
                }
            }
        }

        return std::nullopt;
    }

    /// Retrieve a string DICOM tag value. If the tag is not found, an empty string is returned.
    template<typename A>
    [[nodiscard]] inline std::string getStringValue(std::size_t instance = 0) const
    {
        const auto& value = getValue<A>(instance);

        if(value)
        {
            if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
            {
                // Use trimmed string, as we don't care about DICOM string padding with space
                return shrink(value->Trim());
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
    template<typename A>
    [[nodiscard]] constexpr std::vector<typename A::ArrayType> getValues(std::size_t instance = 0) const
    {
        A attribute {};
        attribute.SetFromDataSet(getDataSet(instance));

        const auto count = attribute.GetNumberOfValues();

        if(count > 0)
        {
            auto values_pointer = attribute.GetValues();

            // Pointer can be treated as iterator ;)
            return std::vector<typename A::ArrayType>(
                values_pointer,
                values_pointer + count
            );
        }

        return std::vector<typename A::ArrayType>();
    }

    /// Retrieve multi-value DICOM tag as a single joined string. Use '\' as separator.
    template<typename A>
    [[nodiscard]] inline std::string getJoinedValues(std::size_t instance = 0) const noexcept
    {
        return boost::join(getStringValues<A>(instance), BACKSLASH_SEPARATOR);
    }

    /// Retrieve a multi-value string DICOM tag. If the tag is not found, an empty vector is returned.
    template<typename A>
    [[nodiscard]] inline std::vector<std::string> getStringValues(std::size_t instance = 0) const noexcept
    {
        A attribute;
        attribute.SetFromDataSet(getDataSet(instance));

        const auto values = attribute.GetValues();

        std::vector<std::string> vector;

        for(std::size_t i = 0 ; i < attribute.GetNumberOfValues() ; ++i)
        {
            if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
            {
                // Use trimmed string, as we don't care about DICOM string padding with space
                vector.push_back(shrink(values[i].Trim()));
            }
            else
            {
                static_assert(std::is_arithmetic_v<typename A::ArrayType>, "Arithmetic type expected");
                vector.push_back(arithmeticToString(values[i], A::GetVR()));
            }
        }

        return vector;
    }

    /// Set a DICOM tag value. If the value is null, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setValue(const std::optional<typename A::ArrayType>& value, std::size_t instance = 0)
    {
        if(value)
        {
            A attribute {};
            attribute.SetValue(*value);
            getDataSet(instance).Replace(attribute.GetAsDataElement());
        }
        else
        {
            // We need to put an "empty" value. Since GDCM doesn't allow us to do it with non string values, we need to
            // hack the system. Sorry GDCM, but an Integer String definitively could be "unknown" and an empty string
            // **IS** valid..
            getDataSet(instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
    }

    /// Set a string DICOM tag value. If the value is null, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setStringValue(const std::string& value, std::size_t instance = 0)
    {
        if(value.empty())
        {
            // Force a real emtpy value..
            getDataSet(instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute;

            if constexpr(std::is_floating_point_v<typename A::ArrayType>)
            {
                attribute.SetValue(A::ArrayType(std::stod(value)));
            }
            else if constexpr(std::is_integral_v<typename A::ArrayType>)
            {
                // cspell: ignore stoll
                attribute.SetValue(A::ArrayType(std::stoll(value)));
            }
            else
            {
                // Maybe it works...
                attribute.SetValue(value);
            }

            getDataSet(instance).Replace(attribute.GetAsDataElement());
        }
    }

    /// Set a multi-value DICOM tag. If the vector is empty, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setValues(const std::vector<typename A::ArrayType>& values, std::size_t instance = 0)
    {
        if(values.empty())
        {
            // Force a real emtpy value..
            getDataSet(instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute {};

            if constexpr(HasFixedMultiplicity<A>::value)
            {
                attribute.SetValues(values.data(), std::uint32_t(values.size()));
            }
            else
            {
                attribute.SetValues(values.data(), std::uint32_t(values.size()), true);
            }

            getDataSet(instance).Replace(attribute.GetAsDataElement());
        }
    }

    /// Set a multi-value DICOM tag from a single string. Use '\' as separator.
    template<typename A>
    inline void setJoinedValues(const std::string& values, std::size_t instance = 0)
    {
        // Split the original string
        std::vector<std::string> split;
        boost::split(split, values, boost::is_any_of(BACKSLASH_SEPARATOR));

        setStringValues<A>(split, instance);
    }

    /// Set a string multi-value DICOM tag. If the vector is empty, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setStringValues(const std::vector<std::string>& values, std::size_t instance = 0)
    {
        if(!values.empty())
        {
            std::vector<typename A::ArrayType> array;
            array.reserve(values.size());

            for(const auto& value : values)
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
                getDataSet(instance).Replace(attribute.GetAsDataElement());

                // Exit here...
                return;
            }
        }

        // Default: force a real emtpy value..
        getDataSet(instance).Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
    }

    /// Little helper function for integral type. Used when tag/VR is not known at compile time.
    template<typename T>
    inline void setArithmeticValue(const gdcm::Tag& tag, const gdcm::VR& vr, const T& value, std::size_t instance = 0)
    {
        static_assert(std::is_arithmetic_v<T>, "The type must be arithmetic.");

        gdcm::DataElement data_element(tag);
        data_element.SetVR(vr);
        data_element.SetByteValue(reinterpret_cast<const char*>(&value), std::uint32_t(sizeof(value)));
        getDataSet(instance).Replace(data_element);
    }

    /// Little helper function for integral type. Used when tag/VR is not known at compile time.
    template<typename T>
    [[nodiscard]] inline std::string getArithmeticValue(const gdcm::Tag& tag, std::size_t instance = 0) const
    {
        static_assert(std::is_arithmetic_v<T>, "The type must be arithmetic.");

        const auto& dataset = getDataSet(instance);

        if(dataset.FindDataElement(tag))
        {
            if(const auto& data_element = dataset.GetDataElement(tag); !data_element.IsEmpty())
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

    //------------------------------------------------------------------------------

    template<typename A>
    inline void copyElement(const gdcm::DataSet& source, std::size_t instance = 0)
    {
        copyElement(source, A::GetTag(), instance);
    }

    //------------------------------------------------------------------------------

    inline void copyElement(const gdcm::DataSet& source, const gdcm::Tag& tag, std::size_t instance = 0)
    {
        if(source.FindDataElement(tag))
        {
            // Make an explicit copy
            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            gdcm::DataElement data_element(source.GetDataElement(tag));

            // Replace the element with the copy
            getDataSet(instance).Replace(data_element);
        }
        else
        {
            // Tag not found, remove it
            getDataSet(instance).Remove(tag);
        }
    }

    /// Pointer to the public class
    sight::data::Series* const m_series {nullptr};

    /// Dicom data set instances specific to a frame
    FrameDatasets m_frame_datasets;
};

} // sight::data::detail
