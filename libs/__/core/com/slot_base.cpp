/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/com/slot_base.hpp"

#include "core/com/exception/bad_run.hpp"
#include "core/com/slot_base.hxx"

namespace sight::core::com
{

//------------------------------------------------------------------------------

void slot_base::run() const
{
    using slot_func_type = slot_run<void ()>;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun != nullptr)
    {
        fun->run();
    }
    else
    {
        SIGHT_ERROR("failed to run : " + m_signature + " with " + slot_base::get_type_name<void()>());
        SIGHT_THROW_EXCEPTION(core::com::exception::bad_run("Failed to find right signature for run"));
    }
}

//-----------------------------------------------------------------------------

slot_base::void_shared_future_type slot_base::async_run() const
{
    using slot_func_type = slot_run<void ()>;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun != nullptr)
    {
        return fun->async_run();
    }

    SIGHT_ERROR("failed to async_run : " + m_signature + " with " + slot_base::get_type_name<void()>());
    SIGHT_THROW_EXCEPTION(core::com::exception::bad_run("Failed to find right signature for async_run"));
}

} // namespace sight::core::com
