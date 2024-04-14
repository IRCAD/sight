/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <sight/service/config.hpp>

#include "service/base.hpp"

#include <core/com/signals.hpp>

namespace sight::service
{

/**
 * @brief   This interface defines operator service API.

 * @section Signals Signals
 * - \b computed() : Emitted when something was computed.
 */
class SIGHT_SERVICE_CLASS_API filter : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(filter, service::base);

    SIGHT_SERVICE_API static const core::com::signals::key_t COMPUTED_SIG;
    using computed_signal_t = core::com::signal<void ()>;

protected:

    /// filter constructor.
    SIGHT_SERVICE_API filter();

    /// filter destructor.
    SIGHT_SERVICE_API ~filter() override;

    computed_signal_t::sptr m_sig_computed; ///< Signal emitted when operator is computed.
};

} // namespace sight::service
