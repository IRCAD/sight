/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/io/dicom/config.hpp>

#include <string>

namespace sight::io::dicom::helper
{

/**
 * @brief Generates a random Dicom Patient ID using GDCM.
 * It must be at most 64 char long and non null.
 */
SIGHT_IO_DICOM_API std::string generate_patient_id();

/**
 * @brief Generates a random Dicom Study Instance UID using current time.
 * It must be 16 char long and may be null.
 *
 * FIXME : According to Dicom, the Study Instance UID must identify a study from time and location.
 */
SIGHT_IO_DICOM_API std::string generate_study_instance_uid();

} // namespace sight::io::dicom::helper
