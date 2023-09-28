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

#include "SDumpAll.hpp"

#include <core/base.hpp>
#include <core/memory/buffer_info.hpp>
#include <core/memory/buffer_manager.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::debug::action
{

//------------------------------------------------------------------------------

SDumpAll::SDumpAll() noexcept =
    default;

//------------------------------------------------------------------------------

SDumpAll::~SDumpAll() noexcept =
    default;

//------------------------------------------------------------------------------

void SDumpAll::updating()
{
    std::size_t nbBuffDumped = 0;
    core::memory::buffer_manager::buffer_info_map_t buffInfoMap;
    core::memory::buffer_manager::sptr buffManager = core::memory::buffer_manager::get();
    if(buffManager)
    {
        buffInfoMap = buffManager->get_buffer_infos().get();
    }

    for(const core::memory::buffer_manager::buffer_info_map_t::value_type& elt : buffInfoMap)
    {
        core::memory::buffer_info dumpBuffInfo = elt.second;
        bool loaded                            = dumpBuffInfo.loaded;
        bool isLock                            = dumpBuffInfo.lock_count() > 0;
        if(loaded && !isLock)
        {
            bool dumped = buffManager->dump_buffer(elt.first).get();
            if(dumped)
            {
                ++nbBuffDumped;
            }
        }
    }

    std::stringstream stream;
    stream << nbBuffDumped << " buffer dumped (" << nbBuffDumped << "/" << buffInfoMap.size() << ").";
    sight::ui::dialog::message::show(
        "Dump all",
        stream.str(),
        sight::ui::dialog::message::INFO
    );
}

//------------------------------------------------------------------------------

void SDumpAll::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void SDumpAll::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------
void SDumpAll::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
