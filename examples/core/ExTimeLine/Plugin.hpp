/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include <core/runtime/Plugin.hpp>
#include <core/thread/Worker.hpp>

/**
 * Do not mark `EXTIMELINE` as incorrect.
 * cspell:ignore EXTIMELINE
 */
namespace ExTimeLine
{

/// This class is started when the module is loaded.
class EXTIMELINE_CLASS_API Plugin : public sight::core::runtime::Plugin
{
public:

    /// Destroys the plugin.
    EXTIMELINE_API ~Plugin() noexcept override;

    /// Adds the default worker.
    EXTIMELINE_API void start() override;

    /// Does nothing here.
    EXTIMELINE_API void stop() noexcept override;

private:

    sight::core::thread::Worker::sptr m_worker {nullptr};
};

} // namespace ExTimeLine.
