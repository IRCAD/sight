/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <sight/ex_timeline/config.hpp>

#include <service/base.hpp>

/**
 * Do not mark `ex_timeline` as incorrect.
 * cspell:ignore ex_timeline
 */
namespace ex_timeline
{

/**
 * @brief This service is just a hack to block the main thread. Services on the other threads can run.
 * Without that, the application exits immediately since we have no GUI.
 */
class SIGHT_EX_TIMELINE_CLASS_API main final : public sight::service::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(main, sight::service::base);

    /// Creates the service.
    SIGHT_EX_TIMELINE_API main() noexcept;

    /// Destroys the service.
    SIGHT_EX_TIMELINE_API ~main() noexcept override;

protected:

    /// Does nothing.
    SIGHT_EX_TIMELINE_API void configuring() override;

    /// Does nothing.
    SIGHT_EX_TIMELINE_API void starting() override;

    /// Contains one input instruction to block the main thread.
    SIGHT_EX_TIMELINE_API void updating() override;

    /// Does nothing.
    SIGHT_EX_TIMELINE_API void stopping() override;
};

} // namespace ex_timeline.
