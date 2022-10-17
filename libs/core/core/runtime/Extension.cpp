/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/runtime/Extension.hpp"

#include "core/runtime/Module.hpp"
#include "core/runtime/runtime.hpp"

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

Extension::Extension(
    std::shared_ptr<Module> module,
    const std::string& id,
    const std::string& point
) :
    ModuleElement(module),
    m_id(filterID(id)),
    m_point(filterID(point))
{
}

//------------------------------------------------------------------------------

const std::string& Extension::getIdentifier() const
{
    return m_id;
}

//------------------------------------------------------------------------------

const std::string& Extension::getPoint() const
{
    return m_point;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime
