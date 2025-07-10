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

#pragma once

#include  <optional>

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4702) // warning C4702: unreachable code
#endif

#include <gdcmDataSet.h>

#ifdef WIN32
#pragma warning( pop )
#endif

namespace sight::data::detail
{

static constexpr char SPACE_PADDING_CHAR  = ' ';
static constexpr char NULL_PADDING_CHAR   = '\0';
static constexpr auto BACKSLASH_SEPARATOR = "\\";

/// @see https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_7.8
static constexpr std::uint16_t PRIVATE_GROUP {0x0099};
static constexpr std::uint16_t PRIVATE_CREATOR_ELEMENT {0x0099};
static constexpr std::uint16_t PRIVATE_DATA_ELEMENT {0x9910};
static const std::string PRIVATE_CREATOR {"Sight"};

/// Remove the trailing padding \0 characters from a string.
/// @param[in] _source The string to be trimmed.
/// @return The trimmed string.
std::string shrink(const std::string& _source);

//------------------------------------------------------------------------------

std::optional<std::string> get_private_string_value(const gdcm::DataSet& _dataset, const gdcm::Tag& _tag);

} // sight::data::detail
