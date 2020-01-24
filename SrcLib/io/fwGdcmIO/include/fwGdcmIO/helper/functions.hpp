/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/config.hpp"

#include <string>

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief Generates a random Dicom Patient ID using GDCM.
 * It must be at most 64 char long and non null.
 */
FWGDCMIO_API std::string generatePatientId();

/**
 * @brief Generates a random Dicom Study Instance UID using current time.
 * It must be 16 char long and may be null.
 *
 * FIXME : According to Dicom, the Study Instance UID must identify a study from time and location.
 */
FWGDCMIO_API std::string generateStudyInstanceUid();

} // namespace helper
} // namespace fwGdcmIO
