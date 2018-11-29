/************************************************************************
 *
 * Copyright (C) 2004-2015 IRCAD France
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

#ifndef __FWATOMCONVERSION_EXCEPTION_OBJECTNOTFOUND_HPP__
#define __FWATOMCONVERSION_EXCEPTION_OBJECTNOTFOUND_HPP__

#include <fwCore/Exception.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

namespace exception
{

/// Object not found exception.
struct ObjectNotFound : ::fwCore::Exception
{
    FWATOMCONVERSION_API ObjectNotFound ( const std::string &err );
};

} // namespace exception

} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_EXCEPTION_OBJECTNOTFOUND_HPP__ */

