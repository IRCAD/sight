/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/com/SlotBase.hpp"

#include "core/com/exception/BadRun.hpp"
#include "core/com/SlotBase.hxx"

namespace sight::core::com
{

//------------------------------------------------------------------------------

void SlotBase::run() const
{
    typedef SlotRun<void ()> SlotFuncType;
    const SlotFuncType* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        fun->run();
    }
    else
    {
        SIGHT_ERROR("failed to run : " + m_signature + " with " + SlotBase::getTypeName<void()>());
        SIGHT_THROW_EXCEPTION(core::com::exception::BadRun("Failed to find right signature for run"));
    }
}

//-----------------------------------------------------------------------------

SlotBase::VoidSharedFutureType SlotBase::asyncRun() const
{
    typedef SlotRun<void ()> SlotFuncType;
    const SlotFuncType* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncRun();
    }
    else
    {
        SIGHT_ERROR("failed to asyncRun : " + m_signature + " with " + SlotBase::getTypeName<void()>());
        SIGHT_THROW_EXCEPTION(core::com::exception::BadRun("Failed to find right signature for asyncRun"));
    }
}

} // namespace sight::core::com
