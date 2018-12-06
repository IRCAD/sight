/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWRUNTIME_PROFILE_UNINITIALIZER_HPP__
#define __FWRUNTIME_PROFILE_UNINITIALIZER_HPP__

#include "fwRuntime/config.hpp"
#include "fwRuntime/Version.hpp"

#include <boost/utility.hpp>

#include <string>

namespace fwRuntime
{

namespace profile
{

/**
 * @brief   Starts a given bundle.
 */
class Uninitializer : public boost::noncopyable
{
public:

    friend class Stopper;
    /**
     * @brief       Constructor
     *
     * @param[in]   identifier a string containing a bundle identifier
     */
    FWRUNTIME_API Uninitializer( const std::string& identifier, const Version& version = Version() );

    /**
     * @brief   Applies the uninitializer on the bundle.
     *
     * @remark  This method should be called directly.
     */
    void apply();

protected:

    const std::string m_identifier;     ///< the bundle identifier
    const Version m_version;            ///< the bundle version

};

} // namespace profile

} // namespace fwRuntime

#endif /*__FWRUNTIME_PROFILE_UNINITIALIZER_HPP__*/
