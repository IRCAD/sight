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

#include "dump_all.hpp"

#include <core/base.hpp>
#include <core/memory/buffer_info.hpp>
#include <core/memory/buffer_manager.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::debug::action
{

//------------------------------------------------------------------------------

dump_all::dump_all() noexcept =
    default;

//------------------------------------------------------------------------------

dump_all::~dump_all() noexcept =
    default;

//------------------------------------------------------------------------------

void dump_all::updating()
{
    std::size_t nb_buff_dumped = 0;
    core::memory::buffer_manager::buffer_info_map_t buff_info_map;
    core::memory::buffer_manager::sptr buff_manager = core::memory::buffer_manager::get();
    if(buff_manager)
    {
        buff_info_map = buff_manager->get_buffer_infos().get();
    }

    for(const auto& elt : buff_info_map)
    {
        core::memory::buffer_info dump_buff_info = elt.second;
        bool loaded                              = dump_buff_info.loaded;
        bool is_lock                             = dump_buff_info.lock_count() > 0;
        if(loaded && !is_lock)
        {
            bool dumped = buff_manager->dump_buffer(elt.first).get();
            if(dumped)
            {
                ++nb_buff_dumped;
            }
        }
    }

    std::stringstream stream;
    stream << nb_buff_dumped << " buffer dumped (" << nb_buff_dumped << "/" << buff_info_map.size() << ").";
    sight::ui::dialog::message::show(
        "Dump all",
        stream.str(),
        sight::ui::dialog::message::info
    );
}

//------------------------------------------------------------------------------

void dump_all::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void dump_all::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------
void dump_all::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
