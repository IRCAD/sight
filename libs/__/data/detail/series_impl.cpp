/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "series_impl.hpp"

namespace sight::data::detail
{

//------------------------------------------------------------------------------

std::string shrink(const std::string& _source)
{
    std::string result(_source);
    result.erase(result.find_last_not_of(NULL_PADDING_CHAR) + 1);
    return result;
}

//------------------------------------------------------------------------------

std::optional<std::string> get_private_string_value(const gdcm::DataSet& _dataset, const gdcm::Tag& _tag)
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

} // namespace sight::data::detail
