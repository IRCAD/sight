/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include <core/base.hpp>

#include <map>
#include <string>

namespace sight::io::dicom::helper
{

/**
 * @brief sop_class tools
 */
class IO_DICOM_CLASS_API sop_class
{
public:

    /**
     * @brief Returns SOP Class Name
     * @param sop_classUID SOP Class UID
     * @return SOP Class Name according to UID
     * @note If the UID is not registered, the function returns the UID
     */
    IO_DICOM_API static std::string getSOPClassName(const std::string& sop_classUID);

    /**
     * @brief Returns whether the SOP Class is hazardous or not
     * @param sop_classUID SOP Class UID
     * @return True if the SOP Class is hazardous
     * @note A hazardous SOP class may contain patient's information even after
     * the anonymization process (like pictures, pdf report, etc.)
     * @note If the UID is not registered, the function returns true
     */
    IO_DICOM_API static bool isHazardousSOPClass(const std::string& sop_classUID);

private:

    /// Container holding SOP Class Names and associated Hazardousness
    typedef std::pair<std::string, bool> op_classNameAndHazardousnessPairType;

    /**
     * @brief Map holding associations between SOP Class UID and SOP Class Name/Hazardousness
     * @{ */
    typedef std::map<std::string, op_classNameAndHazardousnessPairType> op_classContainerType;
    static const op_classContainerType s_SOP_CLASS_LIST;
    /**  @} */
};

} // namespace sight::io::dicom::helper
