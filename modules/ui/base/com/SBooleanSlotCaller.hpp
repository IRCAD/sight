/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/ui/base/com/SSlotCaller.hpp"
#include "modules/ui/base/config.hpp"

#include <core/com/Slots.hpp>

#include <service/IService.hpp>

#include <ui/base/IAction.hpp>

#include <vector>

namespace sight::modules::ui::base
{
namespace com
{

/**
 * @brief   This action runs a slot given by HasSlot id and slot key with a boolean in paramter representing the action
 * state (active or not).
 *
 * @see SSlotCaller
 * @warning This action should be configured with style="check".
 */

class MODULE_UI_BASE_CLASS_API SBooleanSlotCaller : public modules::ui::base::com::SSlotCaller
{

public:

    fwCoreServiceMacro(SBooleanSlotCaller, ::sight::ui::base::IAction)

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SBooleanSlotCaller() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API ~SBooleanSlotCaller() noexcept override;

protected:

    /**
     * @brief This method run the specified slots.
     */
    MODULE_UI_BASE_API void updating() override;

};

} // namespace com
} // namespace sight::modules::ui::base
