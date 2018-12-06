/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWZIP_EXCEPTION_WRITE_HPP__
#define __FWZIP_EXCEPTION_WRITE_HPP__

#include <fwCore/Exception.hpp>

#include "fwZip/config.hpp"

namespace fwZip
{

namespace exception
{

/// Write exception.
struct Write : ::fwCore::Exception
{
    FWZIP_API Write ( const std::string &err );
};

} // namespace exception

} // namespace fwZip

#endif /* __FWZIP_EXCEPTION_WRITE_HPP__ */

