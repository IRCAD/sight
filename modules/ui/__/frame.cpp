/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "frame.hpp"

#include <core/id.hpp>

#include <service/macros.hpp>

namespace sight::module::ui
{

frame::frame() noexcept =
    default;

//-----------------------------------------------------------------------------

frame::~frame() noexcept =
    default;

//-----------------------------------------------------------------------------

void frame::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void frame::starting()
{
    this->create();
}

//-----------------------------------------------------------------------------

void frame::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void frame::info(std::ostream& /*_sstream*/)
{
}

//-----------------------------------------------------------------------------

void frame::updating()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
