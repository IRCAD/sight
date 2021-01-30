/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "monitor/action/SDumpAll.hpp"

#include <core/base.hpp>
#include <core/memory/BufferInfo.hpp>
#include <core/memory/BufferManager.hpp>

#include <services/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::monitor::action::SDumpAll, ::sight::data::Object )

//------------------------------------------------------------------------------

SDumpAll::SDumpAll( ) noexcept
{
}

//------------------------------------------------------------------------------

SDumpAll::~SDumpAll() noexcept
{
}

//------------------------------------------------------------------------------

void SDumpAll::updating( )
{
    size_t nbBuffDumped = 0;
    core::memory::BufferManager::BufferInfoMapType buffInfoMap;
    core::memory::BufferManager::sptr buffManager = core::memory::BufferManager::getDefault();
    if(buffManager)
    {
        buffInfoMap = buffManager->getBufferInfos().get();
    }
    for(core::memory::BufferManager::BufferInfoMapType::value_type elt :  buffInfoMap)
    {
        core::memory::BufferInfo dumpBuffInfo = elt.second;
        bool loaded                           = dumpBuffInfo.loaded;
        bool isLock                           = dumpBuffInfo.lockCount() > 0;
        if(loaded && !isLock)
        {
            bool dumped = buffManager->dumpBuffer(elt.first).get();
            if(dumped)
            {
                ++nbBuffDumped;
            }
        }
    }
    std::stringstream stream;
    stream << nbBuffDumped << " buffer dumped (" << nbBuffDumped <<"/"<<buffInfoMap.size()<<").";
    ui::base::dialog::MessageDialog::show(
        "Dump all",
        stream.str(),
        ui::base::dialog::IMessageDialog::INFO);
}

//------------------------------------------------------------------------------

void SDumpAll::configuring()
{
    this->ui::base::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SDumpAll::starting()
{
    this->ui::base::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------
void SDumpAll::stopping()
{
    this->ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace monitor
