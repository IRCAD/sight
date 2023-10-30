/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <ui/__/action.hpp>

namespace sight::module::debug::action
{

/// Increase or decrease the memory consumption by storing a new image, use to experiment dump process
class MODULE_DEBUG_CLASS_API memory_consumption : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(memory_consumption, sight::ui::action);

    /// Does nothing
    MODULE_DEBUG_API memory_consumption() noexcept;

    /// Does nothing
    MODULE_DEBUG_API ~memory_consumption() noexcept override;

protected:

    /**
     * @brief Implements configuring method derived from base. .
     *
     * Sample of declaration configuration :
     *
     * @code{.xml}
         <service uid="myIncMemConsum"  type="ui::action"
            implementation="sight::module::debug::action::memory_consumption" autoComChannel="false" >
            <config mode="increase" value="256" />
        </service>
       @endcode
     * With:
     * @li mode : increase or decrease.
     * @li value : optional, consuming memory size in Mo (in "increase" mode).
     */
    void configuring() override;

    /// Calls classic action methods to start
    void starting() override;

    /// Increase or decrease memory with size defined during configuration
    void updating() override;

    /// Calls classic action methods to stop
    void stopping() override;

private:

    /// Intern method to create a new array
    static void push_new_array(std::size_t _memory_size_in_bytes);

    /// To manage allocation or free memory ( data are saved in a static member )
    bool m_is_increase_mode {true};

    /// Memory allocated in "increase" mode
    std::size_t m_memory_size_in_bytes;
};

} // namespace sight::module::debug::action
