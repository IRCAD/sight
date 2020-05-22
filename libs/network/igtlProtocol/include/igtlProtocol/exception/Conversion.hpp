/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __IGTLPROTOCOL_EXCEPTION_CONVERSION_HPP__
#define __IGTLPROTOCOL_EXCEPTION_CONVERSION_HPP__

#include "igtlProtocol/config.hpp"

#include <fwCore/Exception.hpp>

namespace igtlProtocol
{
namespace exception
{

/// Throw this exception when a conversion between data and igtl message is not possible (error message explains the
/// reasons)
struct IGTLPROTOCOL_CLASS_API Conversion : ::fwCore::Exception
{
    IGTLPROTOCOL_API Conversion( const std::string& err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_EXCEPTION_CONVERSION_HPP__

