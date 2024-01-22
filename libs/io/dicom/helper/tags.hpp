/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <gdcmTag.h>

#include <filesystem>
#include <string>
#include <vector>

namespace sight::io::dicom::helper
{

/**
 * @brief Parse group and element strings and return a gdcm::Tag instance
 *
 * @param _group the group part of the tag
 * @param _element the element part of the tag
 *
 * @return a gdcm::Tag instance
 */
IO_DICOM_API gdcm::Tag get_gdcm_tag(const std::string& _group, const std::string& _element);

/**
 * @brief List of private GDCM tags type
 *
 * @note Do not use gdcm::PrivateTag because it casts element part as uint8_t
 * and we may have uint16_t (for instance Philips tag 0x00E1,0x1022).
 * See part 05 of Dicom Standard, section 7.8.1.
 */
using private_tag_vec_t = std::vector<gdcm::Tag>;

/**
 * @brief Load a set of private tags from given CSV file.
 * Each line of the file must be formatted this way : group,element,owner
 * Example :
 *  @verbatim
 *  0x00E1,0x1022,Philips
 *  @endverbatim
 *
 * @pre file must exist
 * @param _tags_path path to CSV file containing private tags
 *
 * @return a list of gdcm::PrivateTag instances
 */
IO_DICOM_API private_tag_vec_t load_private_tags(const std::filesystem::path& _tags_path);

} // namespace sight::io::dicom::helper
