/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SBOOLEANSLOTCALLER_HPP__
#define __GUI_ACTION_SBOOLEANSLOTCALLER_HPP__

#include "gui/config.hpp"
#include "gui/action/SSlotCaller.hpp"

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
 * @class   SBooleanSlotCaller
 *
 * @see SSlotCaller
 * @warning This action should be configured with style="check".
 */

class GUI_CLASS_API SBooleanSlotCaller : public ::gui::action::SSlotCaller
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SBooleanSlotCaller)(::fwGui::IActionSrv) );

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
    GUI_API void updating();

};


} // namespace action
} // namespace gui


#endif /*__GUI_ACTION_SBOOLEANSLOTCALLER_HPP__*/
