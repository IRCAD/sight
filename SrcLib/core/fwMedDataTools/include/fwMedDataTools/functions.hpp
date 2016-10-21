/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATATOOLS_FUNCTIONS_HPP__
#define __FWMEDDATATOOLS_FUNCTIONS_HPP__

#include "fwMedDataTools/config.hpp"

#include <string>

namespace fwMedDataTools
{

// Skip this for now on Android, this allows us to get rid of GDCM
#ifndef ANDROID

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

#endif // ANDROID

} // namespace fwMedDataTools

#endif //__FWMEDDATATOOLS_FUNCTIONS_HPP__
