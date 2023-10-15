/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "ExTimeLine/config.hpp"

#include <service/base.hpp>

/**
 * Do not mark `EXTIMELINE` as incorrect.
 * cspell:ignore EXTIMELINE
 */
namespace ExTimeLine
{

/**
 * @brief This service is just a hack to block the main thread. Services on the other threads can run.
 * Without that, the application exits immediately since we have no GUI.
 */
class EXTIMELINE_CLASS_API main final : public sight::service::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(main, sight::service::base);

    /// Creates the service.
    EXTIMELINE_API main() noexcept;

    /// Destroys the service.
    EXTIMELINE_API ~main() noexcept override;

protected:

    /// Does nothing.
    EXTIMELINE_API void configuring() override;

    /// Does nothing.
    EXTIMELINE_API void starting() override;

    /// Contains one input instruction to block the main thread.
    EXTIMELINE_API void updating() override;

    /// Does nothing.
    EXTIMELINE_API void stopping() override;
};

} // namespace ExTimeLine.
