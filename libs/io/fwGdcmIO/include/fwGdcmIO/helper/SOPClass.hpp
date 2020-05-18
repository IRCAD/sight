/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
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

#ifndef __FWGDCMIO_HELPER_SOPCLASS_HPP__
#define __FWGDCMIO_HELPER_SOPCLASS_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/base.hpp>

#include <map>
#include <string>

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief SOPClass tools
 */
class FWGDCMIO_CLASS_API SOPClass
{

public:
    /**
     * @brief Returns SOP Class Name
     * @param SOPClassUID SOP Class UID
     * @return SOP Class Name according to UID
     * @note If the UID is not registered, the function returns the UID
     */
    FWGDCMIO_API static std::string getSOPClassName(const std::string& SOPClassUID);

    /**
     * @brief Returns weither the SOP Class is hazardous or not
     * @param SOPClassUID SOP Class UID
     * @return True if the SOP Class is hazardous
     * @note A hazardous SOP class may contain patient's information even after
     * the anonymization process (like pictures, pdf report, etc.)
     * @note If the UID is not registered, the function returns true
     */
    FWGDCMIO_API static bool isHazardousSOPClass(const std::string& SOPClassUID);

private:

    /// Container holding SOP Class Names and associated Hazardousness
    typedef std::pair< std::string, bool > SOPClassNameAndHazardousnessPairType;

    /**
     * @brief Map holding associations between SOP Class UID and SOP Class Name/Hazardousness
     * @{ */
    typedef std::map< std::string, SOPClassNameAndHazardousnessPairType > SOPClassContainerType;
    static const SOPClassContainerType s_SOP_CLASS_LIST;
    /**  @} */

};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_SOPCLASS_HPP__

