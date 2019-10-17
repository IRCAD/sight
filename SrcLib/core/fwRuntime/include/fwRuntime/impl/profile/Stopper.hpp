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

#include "fwRuntime/config.hpp"
#include "fwRuntime/Version.hpp"

#include <boost/utility.hpp>

#include <string>

namespace fwRuntime
{

namespace impl
{

namespace profile
{

/**
 * @brief   Stops a given bundle.
 */
class Stopper :  public boost::noncopyable
{

public:

    /**
     * @brief       Constructor
     *
     * @param[in]   _identifier  a string containing a bundle identifier
     */
    Stopper( const std::string& _identifier, const Version& version = Version() );

    /**
     * @brief   Applies the Stopper on the system.
     *
     * @remark  This method should be called directly.
     */
    void apply();

private:

    const std::string m_identifier;     ///< the bundle identifier
    const Version m_version;            ///< the bundle version
};

} // namespace profile

} // namespace impl

} // namespace fwRuntime
