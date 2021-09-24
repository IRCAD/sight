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

#include "modules/debug/config.hpp"

#include <ui/base/IAction.hpp>

namespace sight::module::debug
{

namespace action
{

/// Increase or decrease the memory consumption by storing a new image, use to experiment dump process
class MODULE_DEBUG_CLASS_API MemoryConsumption : public sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(MemoryConsumption, sight::ui::base::IAction);

    /// Does nothing
    MODULE_DEBUG_API MemoryConsumption() noexcept;

    /// Does nothing
    MODULE_DEBUG_API virtual ~MemoryConsumption() noexcept;

protected:

    /**
     * @brief Implements configuring method derived from IService. .
     *
     * Sample of declaration configuration :
     *
     * @code{.xml}
         <service uid="myIncMemConsum"  type="ui::base::IAction"
            implementation="sight::module::core::debug::action::MemoryConsumption" autoComChannel="false" >
            <config mode="increase" value="256" />
        </service>
       @endcode
     * With:
     * @li mode : increase or decrease.
     * @li value : optional, consuming memory size in Mo (in "increase" mode).
     */
    void configuring() override;

    /// Calls classic IAction methods to start
    void starting() override;

    /// Increase or decrease memory with size defined during configuration
    void updating() override;

    /// Calls classic IAction methods to stop
    void stopping() override;

private:

    /// Intern method to create a new array
    void pushNewArray(size_t memorySizeInBytes);

    /// To manage allocation or free memory ( data are saved in a static member )
    bool m_isIncreaseMode;

    /// Memory allocated in "increase" mode
    size_t m_memorySizeInBytes;
};

} // namespace action

} // namespace sight::module::debug
