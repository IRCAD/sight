/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include <core/base.hpp>

#include <map>
#include <string>

namespace sight::io::dicom::helper
{

/**
 * @brief sop_class tools
 */
class SIGHT_IO_DICOM_CLASS_API sop_class
{
public:

    /**
     * @brief Returns SOP Class Name
     * @param _sop_class_uid SOP Class UID
     * @return SOP Class Name according to UID
     * @note If the UID is not registered, the function returns the UID
     */
    SIGHT_IO_DICOM_API static std::string get_sop_class_name(const std::string& _sop_class_uid);

    /**
     * @brief Returns whether the SOP Class is hazardous or not
     * @param _sop_class_uid SOP Class UID
     * @return True if the SOP Class is hazardous
     * @note A hazardous SOP class may contain patient's information even after
     * the anonymization process (like pictures, pdf report, etc.)
     * @note If the UID is not registered, the function returns true
     */
    SIGHT_IO_DICOM_API static bool is_hazardous_sop_class(const std::string& _sop_class_uid);

private:

    /// Container holding SOP Class Names and associated Hazardousness
    using op_class_name_and_hazardousness_pair_t = std::pair<std::string, bool>;

    /**
     * @brief Map holding associations between SOP Class UID and SOP Class Name/Hazardousness
     * @{ */
    using op_class_container_t = std::map<std::string, op_class_name_and_hazardousness_pair_t>;
    static const op_class_container_t SOP_CLASS_LIST;
    /**  @} */
};

} // namespace sight::io::dicom::helper
