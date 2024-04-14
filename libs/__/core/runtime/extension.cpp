/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/runtime/extension.hpp"

#include "core/runtime/module.hpp"
#include "core/runtime/runtime.hpp"

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

extension::extension(
    std::shared_ptr<module> _module,
    const std::string& _id,
    const std::string& _point
) :
    module_element(_module),
    m_id(filter_id(_id)),
    m_point(filter_id(_point))
{
}

//------------------------------------------------------------------------------

const std::string& extension::identifier() const
{
    return m_id;
}

//------------------------------------------------------------------------------

const std::string& extension::point() const
{
    return m_point;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime
