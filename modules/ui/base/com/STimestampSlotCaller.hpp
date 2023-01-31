/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#pragma once

#include "modules/ui/base/config.hpp"

#include <ui/base/IAction.hpp>

namespace sight::module::ui::base::com
{

/**
 * @brief Runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.
 *
 *
 * @code{.xml}
   <service uid="..." type=sight::module::ui::base::com::STimestampSlotCaller" autoConnect="false">
        <slots>
            <slot>hasSlotsId/slotKey</slot>
            <slot>hasSlots2Id/slot2Key</slot>
        </slots>
   </service>
   @endcode
 * - \b slot : defines the slot to run
 * - \b hasSlotsId : fwId of the HasSlots (usually a service)
 * - \b slotKey : the identifier of the slot in the HasSlots
 */
class MODULE_UI_BASE_CLASS_API STimestampSlotCaller : public sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(STimestampSlotCaller, sight::ui::base::IAction);

    MODULE_UI_BASE_API STimestampSlotCaller()           = default;
    MODULE_UI_BASE_API ~STimestampSlotCaller() override = default;

protected:

    using HasSlotIDType         = std::string;
    using SlotInfoType          = std::pair<HasSlotIDType, core::com::Slots::SlotKeyType>;
    using SlotInfoContainerType = std::vector<SlotInfoType>;

    /// Does nothing
    MODULE_UI_BASE_API void starting() override;

    /// Configures the service
    MODULE_UI_BASE_API void configuring() override;

    /// Does nothing
    MODULE_UI_BASE_API void stopping() override;

    /// This method run the specified slots.
    MODULE_UI_BASE_API void updating() override;

    /// vector representing slots
    SlotInfoContainerType m_slotInfos;
};

} // namespace sight::module::ui::base::com
