/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/ui/__/config.hpp"

#include <ui/__/action.hpp>

namespace sight::module::ui::com
{

/**
 * @brief Runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.
 *
 *
 * @code{.xml}
   <service uid="..." type=sight::module::ui::com::timestamp_slot_caller" auto_connect="false">
        <slots>
            <slot>hasSlotsId/slotKey</slot>
            <slot>hasSlots2Id/slot2Key</slot>
        </slots>
   </service>
   @endcode
 * - \b slot : defines the slot to run
 * - \b hasSlotsId : fwId of the has_slots (usually a service)
 * - \b slotKey : the identifier of the slot in the has_slots
 */
class MODULE_UI_CLASS_API timestamp_slot_caller : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(timestamp_slot_caller, sight::ui::action);

    MODULE_UI_API timestamp_slot_caller()           = default;
    MODULE_UI_API ~timestamp_slot_caller() override = default;

protected:

    using has_slot_id_type      = std::string;
    using slot_info_t           = std::pair<has_slot_id_type, core::com::slots::key_t>;
    using slot_info_container_t = std::vector<slot_info_t>;

    /// Does nothing
    MODULE_UI_API void starting() override;

    /// Configures the service
    MODULE_UI_API void configuring() override;

    /// Does nothing
    MODULE_UI_API void stopping() override;

    /// This method run the specified slots.
    MODULE_UI_API void updating() override;

    /// vector representing slots
    slot_info_container_t m_slot_infos;
};

} // namespace sight::module::ui::com
