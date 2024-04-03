/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include <functional>

namespace sight::viz::scene3d
{

/**
 * @brief Interface for graphics worker.
 *
 * Graphics worker are used to run gpu resource handling tasks in parallel.
 *
 * /!\ DISCLAIMER: can not be used for parallel rendering as it is not supported by OGRE.
 * graphics workers should mainly be used to fill large gpu buffers in the background.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API graphics_worker
{
public:

    using task_t = std::function<void ()>;

    /// Constructor, initializes the worker.
    SIGHT_VIZ_SCENE3D_API graphics_worker()
    = default;

    /// Destructor, clears all waiting tasks and waits on the one being executed.
    SIGHT_VIZ_SCENE3D_API virtual ~graphics_worker()
    = default;

    /// Adds a task at the back of the worker's task queue.
    SIGHT_VIZ_SCENE3D_API virtual void push_task(task_t _task) = 0;
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
