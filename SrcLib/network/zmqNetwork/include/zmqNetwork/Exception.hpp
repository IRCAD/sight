/************************************************************************
 *
 * Copyright (C) 2014-2016 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#ifndef __ZMQNETWORK_EXCEPTION_HPP__
#define __ZMQNETWORK_EXCEPTION_HPP__

#include "zmqNetwork/config.hpp"

#include <fwCore/Exception.hpp>

namespace zmqNetwork
{

/// Exception for zmqNetwork
struct ZMQNETWORK_CLASS_API Exception : ::fwCore::Exception
{
    ZMQNETWORK_API Exception( const std::string& err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace zmqNetwork

#endif // __ZMQNETWORK_EXCEPTION_HPP__

