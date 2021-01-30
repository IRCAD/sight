/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "viz/ogre/config.hpp"

#include <functional>

namespace sight::viz::ogre
{

/**
 * @brief Interface for graphics worker.
 *
 * Graphics worker are used to run gpu resource handling tasks in parallel.
 *
 * /!\ DISCLAIMER: can not be used for parallel rendering as it is not supported by OGRE.
 * graphics workers should mainly be used to fill large gpu buffers in the background.
 */
class VIZ_OGRE_CLASS_API IGraphicsWorker
{
public:

    using TaskType = std::function<void()>;

    /// Constructor, initializes the worker.
    VIZ_OGRE_API IGraphicsWorker()
    {
    }

    /// Destructor, clears all waiting tasks and waits on the one being executed.
    VIZ_OGRE_API virtual ~IGraphicsWorker()
    {
    }

    /// Adds a task at the back of the worker's task queue.
    VIZ_OGRE_API virtual void pushTask(TaskType _task) = 0;

};

//------------------------------------------------------------------------------

} // namespace sight::viz::ogre
