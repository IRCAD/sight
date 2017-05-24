/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_VERSION_HPP__
#define __FWRUNTIME_VERSION_HPP__

#include "fwRuntime/config.hpp"

#include <iostream>
#include <string>

namespace fwRuntime
{

/**
 * @brief   Holds version information for libraries and bundles.
 */
struct Version
{

    /**
     * @brief   Constructor.
     *
     * Builds an undefined version.
     */
    FWRUNTIME_API Version();

    /**
     * @brief       Constructor.
     *
     * @param[in]   version     a string representing the version number.
     */
    FWRUNTIME_API Version(const std::string& version);

    /**
     * @brief       Constructor.
     *
     * @param[in]   major   the version major number
     * @param[in]   minor   the version minor number (0 by default)
     * @param[in]   patch   the version patch number (-1 by default)
     */
    FWRUNTIME_API Version(const int major, const int minor = 0, const int patch = -1);

    /**
     * @brief   Equality test.
     */
    FWRUNTIME_API bool operator==(const Version& version) const;

    /**
     * @brief   String converter.
     */
    FWRUNTIME_API const std::string string() const;

    /**
     * @brief   Return an ostream representation of a version.
     */
    FWRUNTIME_API friend std::ostream& operator<<(std::ostream& os, const Version& version);

    private:

        int m_major; ///< version major
        int m_minor; ///< version minor
        int m_patch; ///< version patch
        bool m_defined; ///< to know if the version is defined
};

} // namespace fwRuntime

#endif /*__FWRUNTIME_VERSION_HPP__*/
