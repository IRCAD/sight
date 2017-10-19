/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_SOPCLASS_HPP__
#define __FWGDCMIO_HELPER_SOPCLASS_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/base.hpp>

#include <map>
#include <string>
#include <utility>

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

