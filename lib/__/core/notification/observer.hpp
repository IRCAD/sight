/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#pragma once

#include <sight/core/config.hpp>

#include "monitor.hpp"

#include <cstdint>
#include <string>

namespace sight::core::notification
{

/**
 * @brief This class manages a implementation of monitor that observes a long task.
 */
class SIGHT_CORE_CLASS_API observer final : public monitor
{
public:

    /**
     * @name Typedefs
     * @{ */
    using sptr = std::shared_ptr<core::notification::observer>;
    using wptr = std::weak_ptr<core::notification::observer>;
    /**  @} */

    /**
     * @brief Constructor
     *
     * @param _name         The monitor name
     * @param _cancelable   Whether the observer can be canceled
     * @param _cancel_hook  Callback to be called on cancel request
     * @param _work_units   Number of work units to reach to complete the monitor
     */
    SIGHT_CORE_API explicit observer(
        std::string _name,
        bool _cancelable          = false,
        cancel_hook _cancel_hook  = nullptr,
        std::uint64_t _work_units = 100
    );

    SIGHT_CORE_API ~observer() final;

    /**
     * @name Exposed base methods
     * @{
     */
    using monitor::done;
    using monitor::done_work;
    using monitor::add_done_work;
    using monitor::set_total_work_units;
    /**  @} */

    /**
     * @brief Call for finishing observer progress
     */
    SIGHT_CORE_API void finish() final;

private:

    /// Task type
    using packaged_task = std::packaged_task<void ()>;

    /// Task observed
    packaged_task m_finish_task;
};

} //namespace sight::core::notification
