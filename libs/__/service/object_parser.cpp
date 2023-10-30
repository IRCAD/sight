/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/object_parser.hpp"

namespace sight::service
{

//------------------------------------------------------------------------------

void object_parser::starting()
{
}

//------------------------------------------------------------------------------

void object_parser::stopping()
{
}

//------------------------------------------------------------------------------

void object_parser::configuring()
{
}

//------------------------------------------------------------------------------

void object_parser::updating()
{
}

//-----------------------------------------------------------------------------

void object_parser::set_object_config(const service::config_t& _cfg_elem)
{
    m_cfg = _cfg_elem;
}

//-----------------------------------------------------------------------------

void object_parser::create_config(core::tools::object::sptr /*unused*/)
{
}

//-----------------------------------------------------------------------------

void object_parser::start_config()
{
}

//-----------------------------------------------------------------------------

void object_parser::update_config()
{
}

//-----------------------------------------------------------------------------

void object_parser::stop_config()
{
}

//-----------------------------------------------------------------------------

void object_parser::destroy_config()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::service
