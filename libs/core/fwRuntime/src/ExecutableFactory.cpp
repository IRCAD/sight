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

#include "fwRuntime/ExecutableFactory.hpp"

namespace fwRuntime
{

//------------------------------------------------------------------------------

ExecutableFactory::ExecutableFactory( const std::string & type )
    :   m_type( type )
{
}

//------------------------------------------------------------------------------

ExecutableFactory::~ExecutableFactory()
{
}

//------------------------------------------------------------------------------

const std::string ExecutableFactory::getType() const
{
    return m_type;
}


} // namespace fwRuntime
