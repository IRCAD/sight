/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "updater.hpp"

namespace sight::app
{

/**
 * @brief   This service updates all configured elements in a strict sequential order.
 * The elements can be a service or another updater (inheriting from sight::app::updater).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::app::update_sequence">
            <config loop="true">
                <service uid="..." />
                <service uid="..." />
                <updater uid="..." />
            </config>
        </service>
   @endcode
 * @subsection Configuration Configuration
 *  - \b loop: call the update sequence in loop when the service starts and until it stops.
 *  - \b parent: uid of the parent updater slot, as specified by the parent service in an <updater> element.
 *  - \b service: uid of the service
 *  - \b updater: uid of another updater, identified by a registration id. In this case, the updater
 * must use this id in the "parent" attribute.
 */
class SIGHT_APP_CLASS_API update_sequence final : public app::updater
{
public:

    SIGHT_DECLARE_SERVICE(update_sequence, sight::app::updater);

    /// Destructor
    SIGHT_APP_API ~update_sequence() final = default;

private:

    /// Does nothing
    SIGHT_APP_API void starting() final;

    /// Does nothing
    SIGHT_APP_API void stopping() final;

    /// Does nothing
    SIGHT_APP_API void updating() final;
};

} // namespace sight::app
