/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMedDataTools/config.hpp"

#include <string>

namespace fwMedDataTools
{

/**
 * @brief Generates a random Dicom Patient ID using GDCM.
 * It must be at most 64 char long and non null.
 */
FWMEDDATATOOLS_API std::string generatePatientId();

/**
 * @brief Generates a random Dicom Study Instance UID using current time.
 * It must be 16 char long and may be null.
 *
 * FIXME : According to Dicom, the Study Instance UID must identify a study from time and location.
 */
FWMEDDATATOOLS_API std::string generateStudyInstanceUid();

} // namespace fwMedDataTools
