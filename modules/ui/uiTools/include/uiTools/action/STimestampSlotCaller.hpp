/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "uiTools/config.hpp"

#include <core/base.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>

#include <ui/base/IActionSrv.hpp>

namespace uiTools
{
namespace action
{

/**
 * @brief Runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.
 *
 * @code{.xml}
     core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
     slot->asyncRun(timestamp);
   @endcode
 */
class UITOOLS_CLASS_API STimestampSlotCaller : public ::sight::ui::base::IActionSrv
{
public:

    fwCoreServiceMacro(STimestampSlotCaller, ::sight::ui::base::IActionSrv)

    /// Constructor
    UITOOLS_API STimestampSlotCaller();

    /// Destructor
    UITOOLS_API ~STimestampSlotCaller();

protected:

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string HasSlotIDType;
    typedef std::pair< HasSlotIDType, core::com::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    /// Does nothing
    UITOOLS_API virtual void starting() override;

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::ui::base::IActionSrv" impl="::uiTools::STimestampSlotCaller" autoConnect="no">
            <slots>
              <slot>hasSlotsId/slotKey</slot>
              <slot>hasSlots2Id/slot2Key</slot>
          </slots>
        </service>
       @endcode
     * - \b slot : defines the slot to run
     *   - \b hasSlotsId : fwId of the HasSlots (usually a service)
     *   - \b slotKey : the identifier of the slot in the HasSlots
     */
    UITOOLS_API virtual void configuring() override;

    /// Does nothing
    UITOOLS_API virtual void stopping() override;

    /// This method run the specified slots.
    UITOOLS_API virtual void updating() override;

    /// vector representing slots
    SlotInfoContainerType m_slotInfos;
};

} // namespace action
} // namespace uiTools
