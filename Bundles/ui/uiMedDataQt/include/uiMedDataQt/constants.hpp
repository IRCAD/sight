/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_CONSTANTS_HPP__
#define __UIMEDDATAQT_CONSTANTS_HPP__

#include "uiMedDataQt/config.hpp"

#include <string>

namespace uiMedDataQt
{

/// Placeholder text used to identify a new patient that must be initialized within the series selector widget.
static const std::string s_NEW_PATIENT_TEXT = "< New patient... >";

/// Placeholder text used to identify a new study that must be initialized within the series selector widget.
static const std::string s_NEW_STUDY_TEXT = "< New study... >";

/// Placeholder text used to identify a new series that must be initialized within the series selector widget.
static const std::string s_INSERT_NEW_SERIES_TEXT = "< Insert new series here... >";

/// Field name used to identify a ::fwMedData::Patient to be initialized.
static const std::string s_NEW_PATIENT_FIELD_NAME = "NEW_PATIENT";

/// Field name used to identify a ::fwMedData::Study to be initialized.
static const std::string s_NEW_STUDY_FIELD_NAME = "NEW_STUDY";

} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_CONSTANTS_HPP__

