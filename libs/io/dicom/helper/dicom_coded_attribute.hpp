/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <gdcmSegment.h>

namespace sight::io::dicom::helper
{

/**
 * @brief Helper class to manage DICOM coded attributes
 */
class IO_DICOM_CLASS_API dicom_coded_attribute
{
public:

    /// DicomCodedAttribute container type
    using DicomCodedAttributeVectorType = std::vector<io::dicom::container::dicom_coded_attribute>;

    /**
     * @brief Convert an entry of the form '(AAA;BBB;CCC)' into a list of DicomCodedAttributes.
     * Several DicomCodedAttributes can be detected in a single entry.
     * @param[in] _entry Entry containing one or several coded attributes.
     */
    IO_DICOM_API static DicomCodedAttributeVectorType convert_entry_to_coded_attribute(const std::string& _entry);

    /**
     * @brief Convert an entry of the form '(AAA;BBB;CCC)' into GDCM's BasicCodedEntryVector.
     * Several DicomCodedAttributes can be detected in a single entry.
     * @param[in] _entry Entry containing one or several coded attributes.
     */
    IO_DICOM_API static gdcm::Segment::BasicCodedEntryVector convert_entry_to_gdcm_coded_attribute(
        const std::string& _entry
    );

    /**
     * @brief Retrieve '(AAA;BBB;CCC)' patterns inside of the entry string.
     * Remove spaces or other characters that might be present between matches.
     * If a single value is requested, check the uniqueness of the value.
     * @param[in,out] _entry Value that shall be verified and formatted
     * @param[in] _multiple_value If set to 'false', check the uniqueness of the value
     */
    IO_DICOM_API static bool check_and_format_entry(std::string& _entry, bool _multiple_value = false);
};

} // namespace sight::io::dicom::helper
