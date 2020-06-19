/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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
 * @brief   Run a slot given by HasSlot id and slot key.
 *
 * This action works on a ::fwData::Object. It does the action specify by the specify config.
 * @code{.xml}
   <service uid="..." type="::gui::action::SSlotCaller" >
      <sync>true</sync>
      <slots>
          <slot>hasSlotsId/slotKey</slot>
      </slots>
   </service>
 * @subsection Configuration Configuration
 * - \b sync : Determines whether slots are called asynchronously ("false") or synchronously ("true")
 */

class GUI_CLASS_API SSlotCaller : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceMacro(SSlotCaller, ::fwGui::IActionSrv);
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    typedef std::string HasSlotIDType;
    typedef std::pair< HasSlotIDType, ::fwCom::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    /// Constructor. Does nothing.
    GUI_API SSlotCaller() noexcept;

    /// Destructor. Does nothing.
    GUI_API virtual ~SSlotCaller() noexcept;

protected:

    /// This method gives information about the class.
    GUI_API virtual void info(std::ostream& _sstream ) override;

    /// This method run the specified slots.
    GUI_API void updating() override;

    ///  This method is used to configure the service parameters: specifies which slots must be called.
    GUI_API void configuring() override;

    GUI_API virtual void starting() override;

    GUI_API virtual void stopping() override;

    // Vector representing slots
    SlotInfoContainerType m_slotInfos;

    /// Determines whether slots are called asynchronously or synchronously
    bool m_synchronized{ false };
};

} // namespace action
} // namespace gui
