/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __GUI_ACTION_SBOOLEANSLOTCALLER_HPP__
#define __GUI_ACTION_SBOOLEANSLOTCALLER_HPP__

#include "gui/action/SSlotCaller.hpp"
#include "gui/config.hpp"

#include <fwCom/Slots.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

#include <vector>

namespace gui
{
namespace action
{

/**
 * @brief   This action runs a slot given by HasSlot id and slot key with a boolean in paramter representing the action
 * state (active or not).
 *
 * @see SSlotCaller
 * @warning This action should be configured with style="check".
 */

class GUI_CLASS_API SBooleanSlotCaller : public ::gui::action::SSlotCaller
{

public:

    fwCoreServiceClassDefinitionsMacro( (SBooleanSlotCaller)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SBooleanSlotCaller() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SBooleanSlotCaller() noexcept;

protected:

    /**
     * @brief This method run the specified slots.
     */
    GUI_API void updating() override;

};

} // namespace action
} // namespace gui

#endif /*__GUI_ACTION_SBOOLEANSLOTCALLER_HPP__*/
